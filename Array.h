#pragma once
namespace HL
{
	namespace System
	{
		namespace Generic
		{
			//����
			namespace Sort
			{
				template<class T>
				class SortCache
				{
					char*m_cache = nullptr;
				public:
					SortCache()
					{
						this->m_cache = (char*)Memory::Allocator::Alloc(sizeof(T));
					}
					void WriteCache(T const&Object) {
						if (m_cache)
							for (int i = 0; i < sizeof(T); ++i)
								m_cache[i] = ((const char*)&Object)[i];
					}
					void ReadCache(T&Target)const {
						if (m_cache)
							for (int i = 0; i < sizeof(T); ++i)
								((char*)&Target)[i] = this->m_cache[i];
					}
					operator T&() {
						return *(T*)m_cache;
					}
					operator T const&()const {
						return *(T*)m_cache;
					}
					~SortCache() {
						if (m_cache)
						{
							Memory::Allocator::Free(m_cache);
							m_cache = nullptr;
						}
					}
				};
				namespace Inner
				{
					template<class U>
					static void SortRaw(U*Array, index_t Low, index_t High, SortCache<U>&Cache) {
						if (Low >= High)
							return;
						atomic_type first = Low;
						atomic_type last = High;
						Cache.WriteCache(Array[first]);
						while (first < last)
						{
							while (first < last&&Array[last] >= Cache)
								--last;
							Array[first] = Array[last];
							while (first < last&&Array[first] <= Cache)
								++first;
							Array[last] = Array[first];
						}
						Cache.ReadCache(Array[first]);
						SortRaw(Array, Low, first - 1, Cache);
						SortRaw(Array, first + 1, High, Cache);
					}
					template<class U,class Functor>
					static void SortRaw(U*Array, index_t Low, index_t High, SortCache<U>&Cache,Functor Comparator) {
						if (Low >= High)
							return;
						atomic_type first = Low;
						atomic_type last = High;
						Cache.WriteCache(Array[first]);
						while (first < last)
						{
							while (first < last&&Comparator(Array[last], Cache) >= 0)
								--last;
							Array[first] = Array[last];
							while (first < last&&Comparator(Array[last], Cache) <= 0)
								++first;
							Array[last] = Array[first];
						}
						Cache.ReadCache(Array[first]);
						SortRaw(Array, Low, first - 1, Cache);
						SortRaw(Array, first + 1, High, Cache);
					}
				}
				template<class U>
				static void Sort(U*Array, index_t Low, index_t High) {
					SortCache<U> cache;
					Inner::SortRaw(Array, Low, High, cache);
				}
				template<class U, class Functor>
				static void Sort(U*Array, index_t Low, index_t High, Functor Comparator) {
					SortCache<U> cache;
					Inner::SortRaw(Array, Low, High, cache, Comparator);
				}
			}

			namespace Internal
			{
				template<class AnyT, class Compera>
				static index_t BinarySearchInternal_Generic(const AnyT*Arr, size_t n, const AnyT&key, Compera comparator)
				{
					index_t mid, front = 0, back = n - 1;
					while (front <= back)
					{
						mid = (front + back) / 2;
						if (comparator(Arr[mid], key) == 0)
							return mid;
						if (comparator(Arr[mid], key) <= 0)
							front = mid + 1;
						else back = mid - 1;
					}
					return -1;
				}
				template<class AnyT>
				static index_t BinarySearchInternal(const AnyT*Arr, size_t len, const AnyT&key)
				{
					index_t mid, front = 0, back = n - 1;
					while (front <= back)
					{
						mid = (front + back) / 2;
						if (Arr[mid] == key)
							return mid;
						if (Arr[mid] < key)
							front = mid + 1;
						else back = mid - 1;
					}
					return -1;
				}

				template<class AnyT>
				static index_t IndexOf(AnyT const*Arr,size_t length,index_t start_index, index_t end_index, AnyT const&key) {
					if (!Arr)
						return -1;
					if (end_index < start_index)
						return -1;
					if (start_index < 0 || end_index >= length)
						return -1;
					for (index_t i = start_index; i <= end_index; ++i) {
						if (Arr[i] == key)
							return i;
					}
					return -1;
				}
				template<class AnyT,class BinaryCompera>
				static index_t IndexOf(AnyT const*Arr, size_t length, index_t start_index, index_t end_index, AnyT const&key, BinaryCompera comparator) {
					if (!Arr)
						return -1;
					if (end_index < start_index)
						return -1;
					if (start_index < 0 || end_index >= length)
						return -1;
					for (index_t i = start_index; i <= end_index; ++i) {
						if (comparator(Arr[i],key)==true)
							return i;
					}
					return -1;
				}
				template<class AnyT>
				static index_t LastIndexOf(AnyT const*Arr, size_t length, index_t start_index, index_t end_index, AnyT const&key) {
					if (!Arr)
						return -1;
					if (end_index < start_index)
						return -1;
					if (start_index < 0 || end_index >= length)
						return -1;
					for (index_t i = end_index; i >= start_index; --i) {
						if (Arr[i] == key)
							return i;
					}
					return -1;
				}
				template<class AnyT, class BinaryCompera>
				static index_t LastIndexOf(AnyT const*Arr, size_t length, index_t start_index, index_t end_index, AnyT const&key, BinaryCompera comparator) {
					if (!Arr)
						return -1;
					if (end_index < start_index)
						return -1;
					if (start_index < 0 || end_index >= length)
						return -1;
					for (index_t i = end_index; i >= start_index; --i) {
						if (comparator(Arr[i], key) == true)
							return i;
					}
					return -1;
				}
				template<class AnyT,class Fx>
				static void IndexOfAll(AnyT const*Arr, size_t length, index_t start_index, index_t end_index, AnyT const&key,Fx func) {
					if (!Arr)
						return;
					if (start_index < 0 || end_index >= length || end_index < start_index)
						return;
					index_t index = start_index - 1;
					for (;;) {
						index = Internal::IndexOf(Arr, length, index + 1, end_index, key);
						if (index == -1)
							break;
						else
							func(Arr[index]);
					}
				}
				template<class AnyT, class Fx, class BinaryCompera>
				static void IndexOfAll(AnyT const*Arr, size_t length, index_t start_index, index_t end_index, AnyT const&key, BinaryCompera comparator, Fx func) {
					if (!Arr)
						return;
					if (start_index < 0 || end_index >= length || end_index < start_index)
						return;
					index_t index = start_index - 1;
					for (;;) {
						index = Internal::IndexOf(Arr, length, index + 1, end_index, key, comparator);
						if (index == -1)
							break;
						else
							func(Arr[index]);
					}
				}

				template<class AnyT, class Fx>
				static void LastIndexOfAll(AnyT const*Arr, size_t length, index_t start_index, index_t end_index, AnyT const&key, Fx func) {
					if (!Arr)
						return;
					if (start_index < 0 || end_index >= length || end_index < start_index)
						return;
					index_t index = end_index + 1;
					for (;;) {
						index = Internal::LastIndexOf(Arr, length, 0, index - 1, key);
						if (index == -1)
							break;
						else
							func(Arr[index]);
					}
				}
				template<class AnyT, class Fx ,class BinaryCompera>
				static void LastIndexOfAll(AnyT const*Arr, size_t length, index_t start_index, index_t end_index, AnyT const&key, BinaryCompera comparator, Fx func) {
					if (!Arr)
						return;
					if (start_index < 0 || end_index >= length || end_index < start_index)
						return;
					index_t index = end_index + 1;
					for (;;) {
						index = Internal::LastIndexOf(Arr, length, 0, index - 1, key, comparator);
						if (index == -1)
							break;
						else
							func(Arr[index]);
					}
				}
				template<class IterT, class Fx>
				void Foreach(IterT begin, IterT end, Fx func) {
					for (IterT iter = begin; iter != end; ++iter) {
						func(*iter);
					}
				}
			}

			//����������ӿ�ʵ��
			template<typename T, typename TT>
			class Iterator<Array<TT>, T>
			{
				T*pObj;
			public:
				inline explicit Iterator(T*p) :pObj(p) {}
				inline Iterator&operator++() {
					++pObj;
					return *this;
				}
				inline Iterator&operator--() {
					--pObj;
					return *this;
				}
				inline Iterator&operator++(int t) {
					pObj += t;
					return *this;
				}
				inline Iterator&operator--(int t) {
					pObj -= t;
					return *this;
				}
				inline T*operator->() {
					return pObj;
				}
				inline T const*operator->()const {
					return pObj;
				}
				friend inline bool operator==(Iterator const& it1, Iterator const& it2) {
					return it1.pObj == it2.pObj;
				}
				friend inline bool operator!=(Iterator const& it1, Iterator const& it2) {
					return it1.pObj != it2.pObj;
				}
				friend inline Iterator operator+(Iterator const& it, int t) {
					return Iterator(it.pObj + t);
				}
				friend inline Iterator operator-(Iterator const&it, int t) {
					return Iterator(it.pObj - t);
				}
				friend inline Iterator operator+(int t, Iterator const& it) {
					return Iterator(t + it.pObj);
				}
				inline T& CurrentReference() {
					return *pObj;
				}
				inline T const& CurrentReference()const {
					return *pObj;
				}
				inline operator T*() {
					return pObj;
				}
				inline operator T const*()const {
					return pObj;
				}
			};

			//����
			template<typename T>
			class Array sealed
			{
			private:
				void InsertInternal(T const*source, index_t index, size_t count) {
					if (source)
					{
						data.Expand(count);
						data.MoveBackward(index, count);
						for (index_t i = 0; i < count; ++i) {
							Memory::PlacementNew(this->data.GetMemoryBlock() + i + index, source[i]);
						}
					}
				}
			public:
				typedef Memory::MemoryManager<T> ContainerT;
			private:
				ContainerT data;
			public:
				Array():data(0){}
				Array(Array const&rhs) :data(rhs.data) {}
				Array(Array &&lhs) :data(static_cast<ContainerT&&>(lhs.data)) {}
				Array(ContainerT const&rhs) :data(rhs) {}
				Array(ContainerT &&lhs) :data(static_cast<ContainerT&&>(lhs)) {}
				Array(std::initializer_list<T>const&init_list) {
					Internal::Foreach(init_list.begin(),init_list.end(), [this](T const&object) {
						this->Add(object);
					});
				}
				Array(size_t size) :data(size) {}
				//�����������Ԫ��
				void Add(T const&object) {
					data.Append(object);
				}
				//�����������Ԫ��
				void Add(T &&object) {
					data.Append(object);
				}
				//�������������һ����ȫ��Ԫ��
				void Add(Array const&rhs) {
					this->data.Append(rhs.data);
				}
				//�������������һ�����index��ʼcount��Ԫ��
				void Add(Array const&rhs, index_t index, size_t count) {
					if (index >= 0 && index + count < rhs.GetLength())
						this->data.Append(rhs.data.GetMemoryBlock() + index, count);
				}
				//�������������һ����ȫ��Ԫ��
				void Add(ContainerT const&rhs) {
					this->data.Append(rhs);
				}
				//�������������һ�����index��ʼcount��Ԫ��
				void Add(ContainerT const&rhs, index_t index, size_t count) {
					if (index >= 0 && index + count < rhs.GetUsedSize())
						this->data.Append(rhs.GetMemoryBlock() + index, count);
				}
				//�������������һ����ȫ��Ԫ��
				void Add(T const*array_ptr, size_t length) {
					if(array_ptr)
						this->data.Append(array_ptr, length);
				}
				//�������������һ�����index��ʼcount��Ԫ��
				void Add(T const*array_ptr, index_t index, size_t count) {
					if (array_ptr)
						this->data.Append(array_ptr + index, count);
				}
				//�Ƴ�ָ����Ԫ��
				void RemoveAt(index_t index) {
					if (index < this->GetLength())
						data.Remove(index, index);
				}
				//�Ƴ�[start_index,end_index]������Ԫ��
				void Remove(index_t start_index, index_t end_index) {
					if (start_index >= 0 && start_index <= end_index&&end_index < this->GetLength())
						data.Remove(start_index, end_index);
				}
				//��index������һ��Ԫ��
				void Insert(T const&object, index_t index) {
					if (index >= 0 && index < this->GetLength())
						this->InsertInternal(&object, index, 1);
				}
				//��index������count��Ԫ��
				void Insert(Array const&rhs, index_t index, size_t count) {
					if (index >= 0 && index < this->GetLength())
						this->InsertInternal(rhs.data.GetMemoryBlock(), index, count);
				}
				//��index������count��Ԫ��
				void Insert(ContainerT const&rhs, index_t index, size_t count) {
					if (index >= 0 && index < this->GetLength())
						this->InsertInternal(rhs.GetMemoryBlock(), index, count);
				}
				//��index������count��Ԫ��
				void Insert(T const*array_ptr, index_t index, size_t count) {
					if (index >= 0 && index < this->GetLength())
						this->InsertInternal(array_ptr, index, count);
				}
				//��ȡ[start_index,end_index]������Ԫ��
				Array SubArray(index_t start_index, index_t end_index)const {
					if (start_index <= end_index&&end_index < this->GetLength() && start_index >= 0)
					{
						Array ret(end_index - start_index);
						ret.Add(this->data.GetMemoryBlock(), start_index, end_index - start_index + 1);
						return ret;
					}
					return Array(0);
				}
				//�Ƴ����һ��Ԫ��
				void Pop() {
					if (this->GetLength() != 0)
						data.Remove(this->GetLength() - 1, this->GetLength() - 1);
				}
				//��������ռ�
				void ShrinkToFit() {
					data.ShrinkToFit();
				}
				//�������
				void Clean() {
					data.Clean();
				}
				//���������ݿ�����ָ���ڴ��
				void CopyTo(T*memory_block, index_t index, size_t count) {
					if (memory_block)
						if (index >= 0 && index + count < this->GetLength())
							this->data.CopyTo(memory_block, index, count);
				}
				//��������
				void Sort() {
					Sort::Sort(this->data.GetMemoryBlock(), 0, this->GetLength() - 1);
				}
				//��������
				template<class Functor>
				void Sort(Functor Comparator) {
					Sort::Sort(this->data.GetMemoryBlock(), 0, this->GetLength() - 1, Comparator);
				}
				//����������
				index_t BinarySearch(T const&key) {
					return Internal::BinarySearchInternal(this->data.GetMemoryBlock(), this->GetLength(), key);
				}
				//����������
				template<class Functor>
				index_t BinarySearch(T const&key, Functor comparator) {
					return Internal::BinarySearchInternal_Generic(this->data.GetMemoryBlock(), this->GetLength(), key, comparator);
				}
				//ƥ�����
				index_t IndexOf(T const&key) {
					return Internal::IndexOf(this->data.GetMemoryBlock(), this->GetLength(), 0, this->GetLength() - 1, key);
				}
				//ƥ�����
				index_t IndexOf(T const&key, index_t start_index) {
					return Internal::IndexOf(this->data.GetMemoryBlock(), this->GetLength(), start_index, this->GetLength() - 1, key);
				}
				//ƥ�����
				index_t IndexOf(T const&key, index_t start_index, index_t end_index) {
					return Internal::IndexOf(this->data.GetMemoryBlock(), this->GetLength(), start_index, end_index, key);
				}
				//ƥ�����(ʹ�ñȽ���)
				template<class BinaryCompera>
				index_t IndexOfWithComparator(T const&key, BinaryCompera comparator) {
					return Internal::IndexOf(this->data.GetMemoryBlock(), this->GetLength(), 0, this->GetLength() - 1, key, comparator);
				}
				//ƥ�����(ʹ�ñȽ���)
				template<class BinaryCompera>
				index_t IndexOfWithComparator(T const&key, index_t start_index, BinaryCompera comparator) {
					return Internal::IndexOf(this->data.GetMemoryBlock(), this->GetLength(), start_index, this->GetLength() - 1, key, comparator);
				}
				//ƥ�����(ʹ�ñȽ���)
				template<class BinaryCompera>
				index_t IndexOfWithComparator(T const&key, index_t start_index, index_t end_index,BinaryCompera comparator) {
					return Internal::IndexOf(this->data.GetMemoryBlock(), this->GetLength(), start_index, end_index, key, comparator);
				}
				//ƥ�����ж���
				template<class Fx>
				void IndexOfAll(T const&key,Fx func) {
					Internal::IndexOfAll(this->data.GetMemoryBlock(), this->GetLength(), 0, this->GetLength() - 1, key, func);
				}
				//ƥ�����ж���
				template<class Fx>
				void IndexOfAll(T const&key, index_t start_index, Fx func) {
					Internal::IndexOfAll(this->data.GetMemoryBlock(), this->GetLength(), start_index, this->GetLength() - 1, key, func);
				}
				//ƥ�����ж���
				template<class Fx>
				void IndexOfAll(T const&key, index_t start_index, index_t end_index, Fx func) {
					Internal::IndexOfAll(this->data.GetMemoryBlock(), this->GetLength(), start_index, end_index, key, func);
				}
				//ƥ�����ж���(ʹ�ñȽ���)
				template<class BinaryCompera,class Fx>
				void IndexOfAllWithComparator(T const&key, BinaryCompera comparator, Fx func) {
					Internal::IndexOfAll(this->data.GetMemoryBlock(), this->GetLength(), 0, this->GetLength() - 1, key, comparator, func);
				}
				//ƥ�����ж���(ʹ�ñȽ���)
				template<class BinaryCompera, class Fx>
				void IndexOfAllWithComparator(T const&key, index_t start_index, BinaryCompera comparator, Fx func) {
					Internal::IndexOfAll(this->data.GetMemoryBlock(), this->GetLength(), start_index, this->GetLength() - 1, key, comparator, func);
				}
				//ƥ�����ж���(ʹ�ñȽ���)
				template<class BinaryCompera, class Fx>
				void IndexOfAllWithComparator(T const&key, index_t start_index, index_t end_index, BinaryCompera comparator, Fx func) {
					Internal::IndexOfAll(this->data.GetMemoryBlock(), this->GetLength(), start_index, end_index, key, comparator, func);
				}
				//�����ʼƥ�����
				index_t LastIndexOf(T const&key) {
					return Internal::LastIndexOf(this->data.GetMemoryBlock(), this->GetLength(), 0, this->GetLength() - 1, key);
				}
				//�����ʼƥ�����
				index_t LastIndexOf(T const&key, index_t start_index) {
					return Internal::LastIndexOf(this->data.GetMemoryBlock(), this->GetLength(), start_index, this->GetLength() - 1, key);
				}
				//�����ʼƥ�����
				index_t LastIndexOf(T const&key, index_t start_index, index_t end_index) {
					return Internal::LastIndexOf(this->data.GetMemoryBlock(), this->GetLength(), start_index, end_index, key);
				}
				//�����ʼƥ�����(ʹ�ñȽ���)
				template<class BinaryCompera>
				index_t LastIndexOfWithComparator(T const&key, BinaryCompera comparator) {
					return Internal::LastIndexOf(this->data.GetMemoryBlock(), this->GetLength(), 0, this->GetLength() - 1, key, comparator);
				}
				//�����ʼƥ�����(ʹ�ñȽ���)
				template<class BinaryCompera>
				index_t LastIndexOfWithComparator(T const&key, index_t start_index, BinaryCompera comparator) {
					return Internal::LastIndexOf(this->data.GetMemoryBlock(), this->GetLength(), start_index, this->GetLength() - 1, key, comparator);
				}
				//�����ʼƥ�����(ʹ�ñȽ���)
				template<class BinaryCompera>
				index_t LastIndexOfWithComparator(T const&key, index_t start_index, index_t end_index, BinaryCompera comparator) {
					return Internal::LastIndexOf(this->data.GetMemoryBlock(), this->GetLength(), start_index, end_index, key, comparator);
				}
				//�����ʼƥ�����ж���
				template<class Fx>
				void LastIndexOfAll(T const&key, Fx func) {
					Internal::LastIndexOfAll(this->data.GetMemoryBlock(), this->GetLength(), 0, this->GetLength() - 1, key, func);
				}
				//�����ʼƥ�����ж���
				template<class Fx>
				void LastIndexOfAll(T const&key,index_t start_index, Fx func) {
					Internal::LastIndexOfAll(this->data.GetMemoryBlock(), this->GetLength(), start_index, this->GetLength() - 1, key, func);
				}
				//�����ʼƥ�����ж���
				template<class Fx>
				void LastIndexOfAll(T const&key, index_t start_index, index_t end_index, Fx func) {
					Internal::LastIndexOfAll(this->data.GetMemoryBlock(), this->GetLength(), start_index, end_index, key, func);
				}
				//�����ʼƥ�����ж���(ʹ�ñȽ���)
				template<class BinaryCompera, class Fx>
				void LastIndexOfAllWithComparator(T const&key, BinaryCompera comparator, Fx func) {
					Internal::LastIndexOfAll(this->data.GetMemoryBlock(), this->GetLength(), 0, this->GetLength() - 1, key, func);
				}
				//�����ʼƥ�����ж���(ʹ�ñȽ���)
				template<class BinaryCompera, class Fx>
				void LastIndexOfAllWithComparator(T const&key, index_t start_index, BinaryCompera comparator, Fx func) {
					Internal::LastIndexOfAll(this->data.GetMemoryBlock(), this->GetLength(), start_index, this->GetLength() - 1, key, func);
				}
				//�����ʼƥ�����ж���(ʹ�ñȽ���)
				template<class BinaryCompera, class Fx>
				void LastIndexOfAllWithComparator(T const&key, index_t start_index, index_t end_index, BinaryCompera comparator, Fx func) {
					Internal::LastIndexOfAll(this->data.GetMemoryBlock(), this->GetLength(), start_index, end_index, key, func);
				}
				//�����Ƿ�Ϊ��
				bool IsEmpty()const {
					return this->data.GetUsedSizeI() == 0;
				}
				//������鳤��
				size_t GetLength()const {
					return this->data.GetUsedSize();
				}
				//����������
				size_t Capacity()const {
					return this->data.GetMaxSize();
				}
				//�����������ݵ���������
				void SetRank(size_t rank) {
					if (rank >= 2)
						this->data.SetRank(rank);
				}
				//Ҫ��������������ڴ�
				void Expand(size_t append_size) {
					this->data.Expand(append_size);
				}
				//����ڴ��
				T const*GetData()const {
					return this->data.GetMemoryBlock();
				}
				//������
				T const&operator[](index_t index)const {
					if (index < 0 || index >= this->GetLength())
						Exception::Throw<Exception::IndexOutOfException>(index, this->GetLength() - 1);
					return this->data[index];
				}
				//������
				T &operator[](index_t index) {
					if (index < 0 || index >= this->GetLength())
						Exception::Throw<Exception::IndexOutOfException>(index, this->GetLength() - 1);
					return this->data[index];
				}
				//������(��Խ���쳣)
				T const&GetAt(index_t index)const {
					return this->data[index];
				}
				//������(��Խ���쳣)
				T&GetAt(index_t index) {
					return this->data[index];
				}

				Array& operator=(Array const&rhs) {
					this->Clean();
					this->Add(rhs);
					return *this;
				}

				Array& operator=(Array &&lhs) {
					this->Clean();
					this->data = static_cast<ContainerT&&>(lhs.data);
					return *this;
				}

				~Array() {
					this->Clean();
				}

				public://��������
#if	_FOR_LOOP_SYNTACTIC_SUGAR //for�﷨��֧��
					inline Iterator<Array<T>, T> begin() {
						return Iterator<Array<T>, T>(this->data.GetMemoryBlock());
					}
					inline Iterator<Array<T>, const T>begin()const {
						return Iterator<Array<T>, const T>(this->data.GetMemoryBlock());
					}
					inline Iterator<Array<T>, T> end() {
						return Iterator<Array<T>, T>(this->data.GetMemoryBlock() + this->GetLength());
					}
					inline Iterator<Array<T>, const T>end()const {
						return Iterator<Array<T>, const T>(this->data.GetMemoryBlock() + this->GetLength());
					}
#endif
				//ѭ������
				template<class Fx>
				void Foreach(Fx func) {
					Internal::Foreach(this->data.GetMemoryBlock(), this->data.GetMemoryBlock() + this->GetLength(), func);
				}
				//��Allocator������ڴ��йܳ�Ϊ����
				static Array ManageFrom(T*array_ptr, size_t count, size_t total) {
					Array ret;
					ret.data.ManageFrom(array_ptr, count, total);
					return ret;
				}
			};


			template<class T>
			struct Interface::IndexerSupportInterface<Array<T>>
			{
				typedef size_t IndexType;
				typedef T ReturnType;
			};

			template<class T>
			struct Interface::IteratorSupportInterface<Array<T>>
			{
				typedef Generic::Iterator<Generic::Array<T>, T> IteratorType;
				typedef Generic::Iterator<Generic::Array<T>, const T> ConstIteratorType;
			};
		}
	}
}