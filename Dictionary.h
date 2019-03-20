#pragma once
namespace HL
{
	namespace System
	{
		namespace Collection
		{
			namespace Generic
			{
				class DictionaryHelper
				{
					static size_t m_count;
					static size_t m_primes[];
				public:
					static constexpr size_t Null = ((size_t)1 << sizeof(size_t) * 4) - 1;
					static bool IsPrime(size_t candidate)
					{
						if ((candidate & 1) != 0)
						{
							size_t limit = (size_t)sqrt(candidate);
							for (size_t divisor = 3; divisor <= limit; divisor += 2)
								if ((candidate % divisor) == 0)
									return false;
							return true;
						}
						return (candidate == 2);
					}
					static size_t GetPrime(size_t min) {
						for (size_t i = 0; i < m_count; i++)
							if (m_primes[i] >= min) return m_primes[i];
						for (size_t i = (min | 1); i < 2147483647; i += 2)
							if (IsPrime(i) && ((i - 1) % 101 != 0))
								return i;
						return min;
					}
				};
				size_t DictionaryHelper::m_primes[] = {
					3, 7, 11, 17, 23, 29, 37, 47, 59, 71, 89, 107, 131, 163, 197, 239, 293, 353, 431, 521, 631, 761, 919,
					1103, 1327, 1597, 1931, 2333, 2801, 3371, 4049, 4861, 5839, 7013, 8419, 10103, 12143, 14591,
					17519, 21023, 25229, 30293, 36353, 43627, 52361, 62851, 75431, 90523, 108631, 130363, 156437,
					187751, 225307, 270371, 324449, 389357, 467237, 560689, 672827, 807403, 968897, 1162687, 1395263,
					1674319, 2009191, 2411033, 2893249, 3471899, 4166287, 4999559, 5999471, 7199369 };
				size_t DictionaryHelper::m_count = sizeof(DictionaryHelper::m_primes) / sizeof(size_t);

				template<class TKey,class TValue>
				class KeyValuePair
				{
					Nullable::Nullable<TKey> m_key;
					Nullable::Nullable<TValue> m_value;
				public:
					template<class Left,class Right>
					KeyValuePair(Left&&key, Right&&value)
						: m_key(std::forward<Left>(key)), m_value(std::forward<Right>(value)) {}
					KeyValuePair(KeyValuePair const&) = default;
					KeyValuePair(KeyValuePair&&) = default;
					KeyValuePair& operator=(KeyValuePair const&) = default;
					KeyValuePair& operator=(KeyValuePair&&)noexcept = default;
					void SetNull() {
						m_key = nullptr;
						m_value = nullptr;
					}
					inline TKey& Key()noexcept {
						return this->m_key.GetObject();
					}
					inline TKey const& Key()const noexcept {
						return this->m_key.GetObject();
					}
					inline TValue& Value()noexcept {
						return this->m_value.GetObject();
					}
					inline TValue const& Value()const noexcept {
						return this->m_value.GetObject();
					}
					~KeyValuePair() {}
				};
				
				template<class TKey,class TValue>
				class Entry
				{
				public:
					size_t HashCode = 0;
					size_t NextIndex = DictionaryHelper::Null;
					KeyValuePair<TKey, TValue> Object;
					Entry(Entry const&) = default;
					Entry(Entry&&) = default;
					Entry& operator=(Entry const&) = default;
					Entry& operator=(Entry&&) = default;
					~Entry() = default;
				};

				template<class TKey,class TValue>
				class DictionaryIterator
				{
					typedef Entry<TKey, TValue> EntryT;
					EntryT*m_iterator = nullptr;
					EntryT*m_end = nullptr;
					KeyValuePair<TKey, TValue>*m_object = nullptr;
				public:
					DictionaryIterator(EntryT*Iterator, EntryT*End)
						:m_iterator(Iterator), m_end(End) {
						if (m_iterator != m_end)
							m_object = &m_iterator->Object;
					}
					DictionaryIterator& operator++() {
						while (m_iterator != m_end)
						{
							m_iterator++;
							if (m_iterator->HashCode > 0)
							{
								m_object = &m_iterator->Object;
								break;
							}
						}
						return *this;
					}
					KeyValuePair<TKey, TValue>& operator*() {
						return *this->m_object;
					}
					KeyValuePair<TKey, TValue> const& operator*()const {
						return *this->m_object;
					}
					inline bool operator==(DictionaryIterator const&other)const noexcept {
						return this->m_iterator == other.m_iterator;
					}
					inline bool operator!=(DictionaryIterator const&other)const noexcept {
						return this->m_iterator != other.m_iterator;
					}
					~DictionaryIterator() {
					}
				};

				template<class TKey,class TValue>
				class Dictionary :public Linq::LinqBase<DictionaryIterator<TKey, TValue>, Dictionary<TKey, TValue>>
				{
					typedef Entry<TKey, TValue> EntryT;
					Collection::Generic::Array<EntryT> m_entries;
					Collection::Generic::Array<size_t> m_buckets;
					size_t m_free_list_index = DictionaryHelper::Null;
					size_t m_free_count = 0;
					size_t m_count = 0;
				private:
					void Initialize(size_t capcity) {
						size_t size = DictionaryHelper::GetPrime(capcity);
						m_buckets.AdjustTo(size);
						for (size_t i = 0; i < m_buckets.Count(); i++) 
							m_buckets[i] = DictionaryHelper::Null;
						m_entries.AdjustTo(size);
						m_free_list_index = DictionaryHelper::Null;
					}
					void Resize(size_t new_size, bool forced_new_hash) {
						m_entries.AdjustTo(new_size);
						m_buckets.AdjustTo(new_size);
						for (size_t i = 0; i < m_buckets.Count(); i++) m_buckets[i] = DictionaryHelper::Null;
						//强制重新计算Hash值
						if (forced_new_hash) {
							for (size_t i = 0; i < m_count; i++)
								if (m_entries[i].HashCode > 0)
									m_entries[i].HashCode = Hash::IHash::GetHashCode(m_entries[i].Object.Key()) & 0x7FFFFFFF;
						}
						for (size_t i = 0; i < m_count; i++) {
							if (m_entries[i].HashCode > 0) {
								size_t bucket = m_entries[i].HashCode % new_size;
								m_entries[i].NextIndex = m_buckets[bucket];
								m_buckets[bucket] = i;
							}
						}
					}
					inline void Resize() {
						Resize(DictionaryHelper::GetPrime(2 * this->m_count), false);
					}
					size_t FindInsertIndex(TKey const&key, bool add) {
						if (m_buckets.Count() == 0) 
							Initialize(0);
						size_t hash_code = Hash::IHash::GetHashCode(key) & 0x7FFFFFFF;
						size_t target_bucket = hash_code % m_buckets.Count();
						size_t collision_count = 0;

						for (size_t i = m_buckets[target_bucket]; i != DictionaryHelper::Null; i = m_entries[i].NextIndex)
						{
							if (m_entries[i].HashCode == hash_code)
								return add ? DictionaryHelper::Null : i;
							collision_count++;
						}
						size_t index;
						if (m_free_count >0) {
							index = m_free_list_index;
							m_free_list_index = m_entries[index].NextIndex;
							m_free_count--;
						}
						else
						{
							if (m_count == m_entries.Count())
							{
								Resize();
								target_bucket = hash_code % m_buckets.Count();
							}
							index = m_count;
							m_count++;
						}
						m_entries[index].HashCode = hash_code;
						m_entries[index].NextIndex = m_buckets[target_bucket];
						m_buckets[target_bucket] = index;
						return index;
					}
					size_t FindEntry(TKey const&key)const {
						if (m_buckets.Count() != 0) {
							size_t hash_code = Hash::IHash::GetHashCode(key) & 0x7FFFFFFF;
							for (size_t i = m_buckets[hash_code % m_buckets.Count()]; i != DictionaryHelper::Null; i = m_entries[i].NextIndex)
								if (m_entries[i].HashCode == hash_code) return i;
						}
						return DictionaryHelper::Null;
					}
					size_t FindEntryByHash(size_t hash)const {
						if (m_buckets.Count() != 0) {
							size_t hash_code = hash & 0x7FFFFFFF;
							for (size_t i = m_buckets[hash_code % m_buckets.Count()]; i != DictionaryHelper::Null; i = m_entries[i].NextIndex)
								if (m_entries[i].HashCode == hash_code) return i;
						}
						return DictionaryHelper::Null;
					}
					bool RemoveBy(TKey const&Key) {
						if (m_buckets.Count() != 0) {
							size_t hash_code = Hash::IHash::GetHashCode(Key) & 0x7FFFFFFF;
							size_t bucket = hash_code % m_buckets.Count();
							size_t last = DictionaryHelper::Null;
							for (size_t i = m_buckets[bucket]; i != DictionaryHelper::Null; last = i, i = m_entries[i].NextIndex) {
								if (m_entries[i].HashCode == hash_code) {
									if (last == DictionaryHelper::Null)
										m_buckets[bucket] = m_entries[i].NextIndex;
									else
										m_entries[last].NextIndex = m_entries[i].NextIndex;
									m_entries[i].Object.SetNull();
									m_entries[i].HashCode = 0;
									m_entries[i].NextIndex = m_free_list_index;
									m_free_list_index = i;
									m_free_count++;
									return true;
								}
							}
						}
						return false;
					}
				public:
					Dictionary() {}
					Dictionary(size_t Capacity) {
						Initialize(Capacity);
					}
					Dictionary(Dictionary const&) = default;
					Dictionary(Dictionary&&)noexcept = default;
					Dictionary&operator=(Dictionary const&) = default;
					Dictionary&operator=(Dictionary&&) = default;
					~Dictionary() {}

					void Add(TKey const&Key, TValue const&Value) {
						Add(KeyValuePair<TKey, TValue>(Key, Value));
					}
					void Add(KeyValuePair<TKey, TValue> const&Pair) {
						size_t index = FindInsertIndex(Pair.Key(), true);
						if (index == DictionaryHelper::Null) {

						}
						m_entries[index].Object = Pair;
					}
					void Add(KeyValuePair<TKey, TValue>&&Pair) {
						size_t index = FindInsertIndex(Pair.Key(), true);
						if (index == DictionaryHelper::Null) {

						}
						m_entries[index].Object = std::move(Pair);
					}
					inline bool Contains(TKey const&Key)const {
						return FindEntry(Key) != DictionaryHelper::Null;
					}
					void Remove(TKey const&Key) {
						RemoveBy(Key);
					}
					inline size_t Count()const noexcept {
						return this->m_count;
					}
					void Clear() {
						m_entries.Clear();
						m_buckets.Clear();
						m_free_count = 0;
						m_count = 0;
						m_free_list_index = DictionaryHelper::Null;
					}
					inline TValue& ByHash(size_t Hash) {
						return m_entries[FindEntryByHash(Hash)].Object.Value();
					}
					inline TValue const& ByHash(size_t Hash)const {
						return m_entries[FindEntryByHash(Hash)].Object.Value();
					}
					inline TValue& operator[](TKey const&Key) {
						return m_entries[FindEntry(Key)].Object.Value();
					}
					inline TValue const& operator[](TKey const&Key)const {
						return m_entries[FindEntry(Key)].Object.Value();
					}
					typedef DictionaryIterator<TKey, TValue> IteratorT;
					inline IteratorT begin() {
						return IteratorT(this->m_entries.begin(), this->m_entries.end());
					}
					inline IteratorT end() {
						return IteratorT(this->m_entries.end(), this->m_entries.end());
					}
					inline IteratorT begin()const {
						return IteratorT((EntryT*)this->m_entries.begin(), (EntryT*)this->m_entries.end());
					}
					inline IteratorT end()const {
						return IteratorT((EntryT*)this->m_entries.end(), (EntryT*)this->m_entries.end());
					}
				};
			}
		}
	}
}