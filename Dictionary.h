#pragma once
#include <math.h>
namespace HL
{
	namespace System
	{
		namespace Generic
		{
			//字典容器接口
			template<class TKey, class TValue>
			class IDictionary
			{
			public:
				virtual void Add(TKey const&, TValue const&) = 0;
				virtual bool Remove(TKey const&) = 0;
				virtual bool Contains(TKey const&)const = 0;
				virtual void Clear() = 0;
				virtual TValue const&operator[](TKey const&)const = 0;
				virtual TValue &operator[](TKey const&) = 0;
				virtual ~IDictionary() {}
			};

			namespace Inner
			{
				class Helper
				{
					static size_t m_count;
					static size_t m_primes[];
				public:
					static bool IsPrime(size_t candidate)
					{
						if ((candidate & 1) != 0)
						{
							size_t limit = (size_t)sqrt(candidate);
							for (size_t divisor = 3; divisor <= limit; divisor += 2)
							{
								if ((candidate % divisor) == 0)
									return false;
							}
							return true;
						}
						return (candidate == 2);
					}
					static size_t GetPrime(size_t min) {
						for (int i = 0; i < m_count; i++)
						{
							if (m_primes[i] >= min) return m_primes[i];
						}
						//ToDo
						for (int i = (min | 1); i < 2147483647; i += 2)
						{
							if (IsPrime(i) && ((i - 1) % 101 != 0))
								return i;
						}
						return min;
					}
				};
				size_t Helper::m_primes[] = {
					3, 7, 11, 17, 23, 29, 37, 47, 59, 71, 89, 107, 131, 163, 197, 239, 293, 353, 431, 521, 631, 761, 919,
					1103, 1327, 1597, 1931, 2333, 2801, 3371, 4049, 4861, 5839, 7013, 8419, 10103, 12143, 14591,
					17519, 21023, 25229, 30293, 36353, 43627, 52361, 62851, 75431, 90523, 108631, 130363, 156437,
					187751, 225307, 270371, 324449, 389357, 467237, 560689, 672827, 807403, 968897, 1162687, 1395263,
					1674319, 2009191, 2411033, 2893249, 3471899, 4166287, 4999559, 5999471, 7199369 };
				size_t Helper::m_count = sizeof(Helper::m_primes) / sizeof(size_t);

				template<class TKey,class TValue>
				struct EntryPair
				{
					atomic_type Hash = -1;
					atomic_type Next = 0;
					TKey*Key = nullptr;
					TValue*Value = nullptr;
					EntryPair(EntryPair const&rhs)
					{
						this->Key = new TKey(*rhs.Key);
						this->Value = new TValue(*rhs.Value);
						this->Hash = rhs.Hash;
						this->Next = rhs.Next;
					}
					EntryPair(EntryPair &&lhs)
					{
						this->Key = lhs.Key;
						this->Value = lhs.Value;
						this->Hash = lhs.Hash;
						this->Next = lhs.Next;
						lhs.Key = nullptr;
						lhs.Value = nullptr;
					}
					EntryPair() {
						Hash = -1;
					}
					EntryPair&operator=(EntryPair const&rhs)
					{
						this->Clear();
						this->Key = new TKey(*rhs.Key);
						this->Value = new TValue(*rhs.Value);
						this->Hash = rhs.Hash;
						this->Next = rhs.Next;
						return *this;
					}
					EntryPair&operator=(EntryPair&&lhs)
					{
						this->Clear();
						this->Key = lhs.Key;
						this->Value = lhs.Value;
						this->Hash = lhs.Hash;
						this->Next = lhs.Next;
						lhs.Key = nullptr;
						lhs.Value = nullptr;
						return *this;
					}
					void Clear() {
						if (Key != nullptr)
						{
							delete Key;
							Key = nullptr;
						}
						if (Value != nullptr) {
							delete Value;
							Value = nullptr;
						}
						Hash = -1;
					}
					~EntryPair() {
						Clear();
					}
				};
			}
			template<class TKey, class TValue>
			class DictionaryEnumerator :public Iteration::IEnumerator<ObservePair<TKey,TValue>>
			{
				typedef Inner::EntryPair<TKey, TValue> Entry;
				typedef Iteration::IEnumerator<ObservePair<TKey, TValue>> Base;
				typedef ObservePair<TKey, TValue> T;
				T m_current;
				Entry * m_iter;
				Entry * m_end;
			public:
				DictionaryEnumerator(Entry * begin, Entry * end) :m_iter(begin), m_end(end), Base(&this->m_current) {
					this->MoveNext();
				}
				DictionaryEnumerator(DictionaryEnumerator const&rhs) :m_current(rhs.m_current), m_iter(rhs.m_iter), m_end(rhs.m_end), Base(&this->m_current) {}
				virtual Iteration::EnumerationResult MoveNext() final {
					while (m_iter != m_end)
					{	
						if (m_iter->Hash > 0)
						{
							m_current.Key(*m_iter->Key);
							m_current.Value(*m_iter->Value);
							m_iter++;
							return Iteration::EnumerationResult::Successful;
						}
						m_iter++;
					}
					return Iteration::EnumerationResult::EOE;
				}
				virtual DictionaryEnumerator& operator=(Iteration::IEnumerator<T>const&rhs)final {
					this->m_iter = static_cast<DictionaryEnumerator const&>(rhs).m_iter;
					this->m_end = static_cast<DictionaryEnumerator const&>(rhs).m_end;
					this->m_current = static_cast<DictionaryEnumerator const&>(rhs).m_current;
					return *this;
				}
				virtual void* ClonePtr()const final {
					return new DictionaryEnumerator(*this);
				}
			};

			//字典容器
			template<class TKey, class TValue>
			class Dictionary sealed :public IDictionary<TKey, TValue>, public System::Interface::ICloneable,public Linq::LinqBase<ObservePair<TKey, TValue>>
			{
				template<class AnyT>
				static atomic_type InnerGetHashCode(AnyT const&key) {
					if (Template::IsBaseOf<System::Hash::ISupportHash, AnyT>::R)
					{
						System::Hash::ISupportHash* ptr = (System::Hash::ISupportHash*)(const_cast<AnyT*>(&key));
						return ptr->GetHashCode();
					}
					else 
					{
						return Hash::Hash::HashSeq(&key);
					}
				}
				typedef Inner::EntryPair<TKey, TValue> Entry;
				System::Generic::array<Entry> m_entries;
				System::Generic::array<atomic_type> m_buckets;
				size_t m_count = 0;
				atomic_type m_freelist = 0;
				atomic_type m_freecount = 0;
				void Initialize(size_t capcity) {
					size_t size = Inner::Helper::GetPrime(capcity);
					m_buckets = array<atomic_type>(size);
					for (int i = 0; i < m_buckets.Count(); i++) m_buckets[i] = -1;
					m_entries = array<Entry>(size);
					m_freelist = -1;
				}

				void Resize(size_t newSize, bool forceNewHashCodes) {
					m_entries.ExpandTo(newSize);
					m_buckets.ExpandTo(newSize);

					for (int i = 0; i < m_buckets.Count(); i++) m_buckets[i] = -1;

					if (forceNewHashCodes) {
						for (index_t i = 0; i < m_count; i++) {
							if (m_entries[i].Hash != -1) {
								m_entries[i].Hash = (InnerGetHashCode(*m_entries[i].Key) & 0x7FFFFFFF);
							}
						}
					}
					for (int i = 0; i < m_count; i++) {
						if (m_entries[i].Hash >= 0) {
							atomic_type bucket = m_entries[i].Hash % newSize;
							m_entries[i].Next = m_buckets[bucket];
							m_buckets[bucket] = i;
						}
					}
				}

				void Resize() {
					Resize(Inner::Helper::GetPrime(2 * this->m_count), false);
				}

				void Insert(TKey const& key, TValue const&value, bool add) {
					if (m_buckets.Count() == 0) Initialize(0);
					atomic_type hashCode = InnerGetHashCode(key) & 0x7FFFFFFF;
					size_t targetBucket = hashCode % m_buckets.Count();
					size_t collisionCount = 0;
					for (atomic_type i = m_buckets[targetBucket]; i >= 0; i = m_entries[i].Next) {
						if (m_entries[i].Hash == hashCode) {
							if (add)
							{
								HL::Exception::Throw<HL::Exception::UnHandledException>();
							}
							m_entries[i].Value = new TValue(value);
							return;
						}
					}
					atomic_type index;
					if (m_freecount > 0) {
						index = m_freelist;
						m_freelist = m_entries[index].Next;
						m_freecount--;
					}
					else {
						if (m_count == m_entries.Count())
						{
							Resize();
							targetBucket = hashCode % m_buckets.Count();
						}
						index = m_count;
						m_count++;
					}
					m_entries[index].Hash = hashCode;
					m_entries[index].Next = m_buckets[targetBucket];
					m_entries[index].Key = new TKey(key);
					m_entries[index].Value = new TValue(value);
					m_buckets[targetBucket] = index;
				}

				index_t FindEntry(TKey const&key)const {
					if (m_buckets.Count() != 0) {
						atomic_type hashCode = InnerGetHashCode(key) & 0x7FFFFFFF;
						for (int i = m_buckets[hashCode % m_buckets.Count()]; i >= 0; i = m_entries[i].Next) {
							if (m_entries[i].Hash == hashCode) return i;
						}
					}
					return -1;
				}
			public:
				Dictionary() {
				}
				//初始化
				//[参数]Capticy:预留空间
				Dictionary(size_t Capticy) {
					Initialize(Capticy);
				}

				Dictionary(Dictionary const&rhs) {
					this->m_buckets = rhs.m_buckets;
					this->m_entries = rhs.m_entries;
					this->m_count = rhs.m_count;
					this->m_freelist = rhs.m_freelist;
					this->m_freecount = rhs.m_freecount;
				}

				Dictionary(Dictionary &&lhs)noexcept {
					this->m_buckets = Forward(lhs.m_buckets);
					this->m_entries = Forward(lhs.m_entries);
					this->m_count = lhs.m_count;
					this->m_freelist = lhs.m_freelist;
					this->m_freecount = lhs.m_freecount;
				}

				Dictionary&operator=(Dictionary const&rhs) {
					this->Clear();
					this->m_buckets = rhs.m_buckets;
					this->m_entries = rhs.m_entries;
					this->m_count = rhs.m_count;
					this->m_freelist = rhs.m_freelist;
					this->m_freecount = rhs.m_freecount;
					return *this;
				}

				Dictionary&operator=(Dictionary &&lhs)noexcept {
					this->Clear();
					this->m_buckets = Forward(lhs.m_buckets);
					this->m_entries = Forward(lhs.m_entries);
					this->m_count = lhs.m_count;
					this->m_freelist = lhs.m_freelist;
					this->m_freecount = lhs.m_freecount;
					return *this;
				}

				//获得当前插入元素个数
				inline size_t Count()const {
					return m_count - m_freecount;
				}
				//插入元素
				virtual void Add(TKey const&Key, TValue const&Value)final {
					Insert(Key, Value, true);
				}
				//移除元素
				virtual bool Remove(TKey const&Key)final {
					if (m_buckets.Count() != 0) {
						index_t hashCode = InnerGetHashCode(Key) & 0x7FFFFFFF;
						index_t bucket = hashCode % m_buckets.Count();
						index_t last = -1;
						for (index_t i = m_buckets[bucket]; i >= 0; last = i, i = m_entries[i].Next) {
							if (m_entries[i].Hash == hashCode) {
								if (last < 0) {
									m_buckets[bucket] = m_entries[i].Next;
								}
								else {
									m_entries[last].Next = m_entries[i].Next;
								}
								m_entries[i].Clear();
								m_entries[i].Hash = -1;
								m_entries[i].Next = m_freelist;
								m_freelist = i;
								m_freecount++;
								return true;
							}
						}
					}
					return false;
				}
				//是否包含元素
				virtual bool Contains(TKey const&Key)const final {
					index_t index = FindEntry(Key);
					if (index < 0)
						return false;
					return true;
				}
				//清空
				virtual void Clear() {
					if (m_count > 0) {
						m_buckets.Clear();
						m_entries.Clear();
						m_freelist = -1;
						m_count = 0;
						m_freecount = 0;
					}
				}
				//operator[]
				virtual TValue const&operator[](TKey const&Key)const final {
					index_t index = FindEntry(Key);
					if (index < 0)
						HL::Exception::Throw<HL::Exception::KeyNotFoundException>();
					return *m_entries[index].Value;
				}
				//operator[]
				virtual TValue &operator[](TKey const&Key)final {
					index_t index = FindEntry(Key);
					if (index < 0)
						HL::Exception::Throw<HL::Exception::KeyNotFoundException>();
					return *m_entries[index].Value;
				}
				virtual void* ClonePtr()const final{
					return new Dictionary(*this);
				}
				virtual uptr<Iteration::IEnumerator<ObservePair<TKey, TValue>>> GetEnumerator()const {
					return newptr<DictionaryEnumerator<TKey, TValue>>(
						const_cast<Entry*>(this->m_entries.GetData()), const_cast<Entry*>(this->m_entries.GetData() + this->m_entries.Count())
						);
				}
				//析构
				virtual ~Dictionary() {
					m_entries.Clear();
					m_buckets.Clear();
				}
			};
		}
		//配置uptr
		template<class TKey,class TValue>
		struct Interface::IndexerSupportInterface<Generic::Dictionary<TKey, TValue>>
		{
			typedef TKey IndexType;
			typedef TValue ReturnType;
		};
		template<class TKey, class TValue>
		struct Interface::EnumerableSupportInterface<Generic::Dictionary<TKey, TValue>>
		{
			typedef uptr<Iteration::Iterator<Generic::ObservePair<TKey, TValue>>> IteratorType;
			typedef uptr<Iteration::Iterator<Generic::ObservePair<TKey, TValue const>>> ConstIteratorType;
		};
	}
}