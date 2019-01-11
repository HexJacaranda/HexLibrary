#pragma once
namespace HL
{
	namespace System
	{
		namespace Generic
		{
			//内部实现
			namespace Internal
			{
				//数组迭代器接口实现
				template<typename T>
				class ArrayEnumerator :public Iteration::IEnumerator<T>
				{
					typedef Iteration::IEnumerator<T> Base;
					T*m_end = nullptr;
				public:
					ArrayEnumerator(ArrayEnumerator const&rhs) :m_end(rhs.m_end), Base(rhs.CurrentObject) {}
					ArrayEnumerator(T*begin, T*end) : m_end(end), Base(begin) {}
					virtual Iteration::EnumerationResult MoveNext() final {
						this->CurrentObject++;
						if (this->CurrentObject == m_end)
							return Iteration::EnumerationResult::EOE;
						return Iteration::EnumerationResult::Successful;
					}
					virtual ArrayEnumerator& operator=(Iteration::IEnumerator<T>const&rhs)final {
						this->m_end = static_cast<ArrayEnumerator const&>(rhs).m_end;
						this->CurrentObject = static_cast<ArrayEnumerator const&>(rhs).CurrentObject;
						return *this;
					}
					virtual void* ClonePtr()const final {
						return new ArrayEnumerator(*this);
					}
					virtual ~ArrayEnumerator() {}
				};
			}

			//定长数组
			template<class T>
			class array sealed 
			{
				T*m_data = nullptr;
				size_t m_count = 0;
			public:
				array() {}
				array(array&&lhs) {
					m_data = lhs.m_data;
					m_count = lhs.m_count;
					lhs.m_data = nullptr;
				}
				array(array const&rhs) {
					m_data = (T*)Memory::Allocator::AllocBlank(rhs.m_count * sizeof(T));
					m_count = rhs.m_count;
					if (!Template::IsValueType<T>::R)
						Memory::Construct(this->m_data, rhs.m_data, m_count);
					else
						Memory::Allocator::MemoryCopy(rhs.m_data, this->m_data, 0, sizeof(T)*m_count);
				}
				array(size_t capcity) : m_count(capcity) {
					m_data = (T*)Memory::Allocator::AllocBlank(capcity * sizeof(T));
				}
				array(T const*ptr, index_t index, size_t count) {
					m_data = (T*)Memory::Allocator::AllocBlank(count * sizeof(T));
					Memory::Construct(m_data, ptr + index, count);
				}
				array&operator=(array const&rhs) {
					this->Clear();
					m_data = (T*)Memory::Allocator::AllocBlank(rhs.m_count * sizeof(T));
					m_count = rhs.m_count;
					if (!Template::IsValueType<T>::R)
						Memory::Construct(this->m_data, rhs.m_data, m_count);
					else
						Memory::Allocator::MemoryCopy(rhs.m_data, this->m_data, 0, sizeof(T)*m_count);
					return *this;
				}
				array&operator=(array&&lhs) {
					this->Clear();
					m_data = lhs.m_data;
					m_count = lhs.m_count;
					lhs.m_data = nullptr;
					return *this;
				}
				array SubArray(index_t index, size_t count)const {
					if (index >= 0 && count > 0) {
						if (index + count < m_count) {
							return array(this->m_data, index, count);
						}
					}
					return array();
				}
				void ExpandTo(size_t count) {
					if (count > m_count) {
						T*ptr = (T*)Memory::Allocator::AllocBlank(count * sizeof(T));
						Memory::Allocator::MemoryCopy(this->m_data, ptr, sizeof(T)*this->m_count, sizeof(T)*this->m_count);
						Memory::Allocator::Free(this->m_data);
						this->m_data = ptr;
						this->m_count = count;
					}
				}
				inline size_t Count()const {
					return m_count;
				}
				inline T const&operator[](index_t index)const {
					return m_data[index];
				}
				inline T&operator[](index_t index) {
					return m_data[index];
				}
				inline T const& GetAt(index_t index)const {
					return m_data[index];
				}
				inline T & GetAt(index_t index) {
					return m_data[index];
				}
				inline T* GetData() {
					return this->m_data;
				}
				inline T const* GetData()const {
					return this->m_data;
				}
				void Clear() {
					if (m_data != nullptr) {
						if (!Template::IsValueType<T>::R)
							Memory::Deconstruct(m_data, m_count);
						Memory::Allocator::Free(m_data);
						m_data = nullptr;
					}
				}
				~array() {
					Clear();
				}
			};

			//数组
			template<typename T>
			class Array sealed :public Interface::ICloneable, public Linq::LinqBase<T>
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
				typedef Internal::ArrayEnumerator<T> IterT;
			private:
				ContainerT data;
			public:
				Array() :data(0) {}
				Array(Array const&rhs) :data(rhs.data) {}
				Array(Array &&lhs)noexcept :data(static_cast<ContainerT&&>(lhs.data)) {}
				Array(ContainerT const&rhs) :data(rhs) {}
				Array(ContainerT &&lhs) :data(static_cast<ContainerT&&>(lhs)) {}
				Array(std::initializer_list<T>const&init_list) {
					Algorithm::Foreach(init_list.begin(), init_list.end(), [this](T const&object) {
						this->Add(object);
					});
				}
				Array(size_t size) :data(size) {}
				//向数组中添加元素
				void Add(T const&object) {
					data.Append(object);
				}
				//向数组中添加元素
				void Add(T &&object) {
					data.Append(Move(object));
				}
				//向数组中添加另一数组全部元素
				void Add(Array const&rhs) {
					this->data.Append(rhs.data);
				}
				//向数组中添加另一数组从index开始count个元素
				void Add(Array const&rhs, index_t index, size_t count) {
					if (index >= 0 && index + count < rhs.Count())
						this->data.Append(rhs.data.GetMemoryBlock() + index, count);
				}
				//向数组中添加另一数组全部元素
				void Add(ContainerT const&rhs) {
					this->data.Append(rhs);
				}
				//向数组中添加另一数组从index开始count个元素
				void Add(ContainerT const&rhs, index_t index, size_t count) {
					if (index >= 0 && index + count < rhs.GetUsedSize())
						this->data.Append(rhs.GetMemoryBlock() + index, count);
				}
				//向数组中添加另一数组全部元素
				void Add(T const*array_ptr, size_t length) {
					if (array_ptr)
						this->data.Append(array_ptr, length);
				}
				//向数组中添加另一数组从index开始count个元素
				void Add(T const*array_ptr, index_t index, size_t count) {
					if (array_ptr)
						this->data.Append(array_ptr + index, count);
				}
				//移除指定处元素
				void RemoveAt(index_t index) {
					if (index < this->Count())
						data.Remove(index, index);
				}
				//移除[start_index,end_index]区间内元素
				void Remove(index_t start_index, index_t end_index) {
					if (start_index >= 0 && start_index <= end_index&&end_index < this->Count())
						data.Remove(start_index, end_index);
				}
				//在index处插入一个元素
				void Insert(T const&object, index_t index) {
					if (index >= 0 && index < this->Count())
						this->InsertInternal(&object, index, 1);
				}
				//在index处插入count个元素
				void Insert(Array const&rhs, index_t index, size_t count) {
					if (index >= 0 && index < this->Count())
						this->InsertInternal(rhs.data.GetMemoryBlock(), index, count);
				}
				//在index处插入count个元素
				void Insert(ContainerT const&rhs, index_t index, size_t count) {
					if (index >= 0 && index < this->Count())
						this->InsertInternal(rhs.GetMemoryBlock(), index, count);
				}
				//在index处插入count个元素
				void Insert(T const*array_ptr, index_t index, size_t count) {
					if (index >= 0 && index < this->Count())
						this->InsertInternal(array_ptr, index, count);
				}
				//截取[start_index,end_index]区间内元素
				Array SubArray(index_t start_index, index_t end_index)const {
					if (start_index <= end_index&&end_index < this->Count() && start_index >= 0)
					{
						Array ret(end_index - start_index);
						ret.Add(this->data.GetMemoryBlock(), start_index, end_index - start_index + 1);
						return ret;
					}
					return Array(0);
				}
				//排序
				template<class Functor>
				void Sort(Functor const&Compare)
				{
					Algorithm::QuickSort(this->data.GetMemoryBlock(), 0, this->Count() - 1, Compare);
				}
				//排序
				void Sort(Linq::SortOrder Order = Linq::SortOrder::Ascending)
				{
					if (Order == Linq::SortOrder::Ascending)
						Algorithm::QuickSort(this->data.GetMemoryBlock(), 0, this->Count() - 1, [](T&l, T&r) {
						return l >= r ? 1 : -1;
					});
					else
						Algorithm::QuickSort(this->data.GetMemoryBlock(), 0, this->Count() - 1, [](T&l, T&r) {
						return l >= r ? -1 : 1;
					});
				}
				//移除最后一个元素
				void Pop() {
					if (this->Count() != 0)
						data.Remove(this->Count() - 1, this->Count() - 1);
				}
				//收缩数组空间
				void ShrinkToFit() {
					data.ShrinkToFit();
				}
				//清空数组
				virtual void Clear() {
					data.Clear();
				}
				//将数组内容拷贝到指定内存块
				void CopyTo(T*memory_block, index_t index, size_t count) {
					if (memory_block)
						if (index >= 0 && index + count < this->Count())
							this->data.CopyTo(memory_block, index, count);
				}
				//二分搜索法
				index_t BinarySearch(T const&key) {
					return Algorithm::BinarySearch(this->data.GetMemoryBlock(), this->Count(), key);
				}
				//二分搜索法
				template<class Functor>
				index_t BinarySearch(T const&key, Functor comparator) {
					return Algorithm::BinarySearchInternal_Generic(this->data.GetMemoryBlock(), this->Count(), key, comparator);
				}
				//匹配对象
				index_t IndexOf(T const&key) {
					return Algorithm::IndexOf(this->data.GetMemoryBlock(), this->Count(), 0, this->Count() - 1, key);
				}
				//匹配对象
				index_t IndexOf(T const&key, index_t start_index) {
					return Algorithm::IndexOf(this->data.GetMemoryBlock(), this->Count(), start_index, this->Count() - 1, key);
				}
				//匹配对象
				index_t IndexOf(T const&key, index_t start_index, index_t end_index) {
					return Algorithm::IndexOf(this->data.GetMemoryBlock(), this->Count(), start_index, end_index, key);
				}
				//匹配对象(使用比较器)
				template<class BinaryCompera>
				index_t IndexOfWithComparator(T const&key, BinaryCompera comparator) {
					return Algorithm::IndexOf(this->data.GetMemoryBlock(), this->Count(), 0, this->Count() - 1, key, comparator);
				}
				//匹配对象(使用比较器)
				template<class BinaryCompera>
				index_t IndexOfWithComparator(T const&key, index_t start_index, BinaryCompera comparator) {
					return Algorithm::IndexOf(this->data.GetMemoryBlock(), this->Count(), start_index, this->Count() - 1, key, comparator);
				}
				//匹配对象(使用比较器)
				template<class BinaryCompera>
				index_t IndexOfWithComparator(T const&key, index_t start_index, index_t end_index, BinaryCompera comparator) {
					return Algorithm::IndexOf(this->data.GetMemoryBlock(), this->Count(), start_index, end_index, key, comparator);
				}
				//匹配所有对象
				template<class Fx>
				void IndexOfAll(T const&key, Fx func) {
					Algorithm::IndexOfAll(this->data.GetMemoryBlock(), this->Count(), 0, this->Count() - 1, key, func);
				}
				//匹配所有对象
				template<class Fx>
				void IndexOfAll(T const&key, index_t start_index, Fx func) {
					Algorithm::IndexOfAll(this->data.GetMemoryBlock(), this->Count(), start_index, this->Count() - 1, key, func);
				}
				//匹配所有对象
				template<class Fx>
				void IndexOfAll(T const&key, index_t start_index, index_t end_index, Fx func) {
					Algorithm::IndexOfAll(this->data.GetMemoryBlock(), this->Count(), start_index, end_index, key, func);
				}
				//匹配所有对象(使用比较器)
				template<class BinaryCompera, class Fx>
				void IndexOfAllWithComparator(T const&key, BinaryCompera comparator, Fx func) {
					Algorithm::IndexOfAll(this->data.GetMemoryBlock(), this->Count(), 0, this->Count() - 1, key, comparator, func);
				}
				//匹配所有对象(使用比较器)
				template<class BinaryCompera, class Fx>
				void IndexOfAllWithComparator(T const&key, index_t start_index, BinaryCompera comparator, Fx func) {
					Algorithm::IndexOfAll(this->data.GetMemoryBlock(), this->Count(), start_index, this->Count() - 1, key, comparator, func);
				}
				//匹配所有对象(使用比较器)
				template<class BinaryCompera, class Fx>
				void IndexOfAllWithComparator(T const&key, index_t start_index, index_t end_index, BinaryCompera comparator, Fx func) {
					Algorithm::IndexOfAll(this->data.GetMemoryBlock(), this->Count(), start_index, end_index, key, comparator, func);
				}
				//从最后开始匹配对象
				index_t LastIndexOf(T const&key) {
					return Algorithm::LastIndexOf(this->data.GetMemoryBlock(), this->Count(), 0, this->Count() - 1, key);
				}
				//从最后开始匹配对象
				index_t LastIndexOf(T const&key, index_t start_index) {
					return Algorithm::LastIndexOf(this->data.GetMemoryBlock(), this->Count(), start_index, this->Count() - 1, key);
				}
				//从最后开始匹配对象
				index_t LastIndexOf(T const&key, index_t start_index, index_t end_index) {
					return Algorithm::LastIndexOf(this->data.GetMemoryBlock(), this->Count(), start_index, end_index, key);
				}
				//从最后开始匹配对象(使用比较器)
				template<class BinaryCompera>
				index_t LastIndexOfWithComparator(T const&key, BinaryCompera comparator) {
					return Algorithm::LastIndexOf(this->data.GetMemoryBlock(), this->Count(), 0, this->Count() - 1, key, comparator);
				}
				//从最后开始匹配对象(使用比较器)
				template<class BinaryCompera>
				index_t LastIndexOfWithComparator(T const&key, index_t start_index, BinaryCompera comparator) {
					return Algorithm::LastIndexOf(this->data.GetMemoryBlock(), this->Count(), start_index, this->Count() - 1, key, comparator);
				}
				//从最后开始匹配对象(使用比较器)
				template<class BinaryCompera>
				index_t LastIndexOfWithComparator(T const&key, index_t start_index, index_t end_index, BinaryCompera comparator) {
					return Internal::LastIndexOf(this->data.GetMemoryBlock(), this->Count(), start_index, end_index, key, comparator);
				}
				//从最后开始匹配所有对象
				template<class Fx>
				void LastIndexOfAll(T const&key, Fx func) {
					Algorithm::LastIndexOfAll(this->data.GetMemoryBlock(), this->Count(), 0, this->Count() - 1, key, func);
				}
				//从最后开始匹配所有对象
				template<class Fx>
				void LastIndexOfAll(T const&key, index_t start_index, Fx func) {
					Algorithm::LastIndexOfAll(this->data.GetMemoryBlock(), this->Count(), start_index, this->Count() - 1, key, func);
				}
				//从最后开始匹配所有对象
				template<class Fx>
				void LastIndexOfAll(T const&key, index_t start_index, index_t end_index, Fx func) {
					Algorithm::LastIndexOfAll(this->data.GetMemoryBlock(), this->Count(), start_index, end_index, key, func);
				}
				//从最后开始匹配所有对象(使用比较器)
				template<class BinaryCompera, class Fx>
				void LastIndexOfAllWithComparator(T const&key, BinaryCompera comparator, Fx func) {
					Algorithm::LastIndexOfAll(this->data.GetMemoryBlock(), this->Count(), 0, this->Count() - 1, key, func);
				}
				//从最后开始匹配所有对象(使用比较器)
				template<class BinaryCompera, class Fx>
				void LastIndexOfAllWithComparator(T const&key, index_t start_index, BinaryCompera comparator, Fx func) {
					Algorithm::LastIndexOfAll(this->data.GetMemoryBlock(), this->Count(), start_index, this->Count() - 1, key, func);
				}
				//从最后开始匹配所有对象(使用比较器)
				template<class BinaryCompera, class Fx>
				void LastIndexOfAllWithComparator(T const&key, index_t start_index, index_t end_index, BinaryCompera comparator, Fx func) {
					Algorithm::LastIndexOfAll(this->data.GetMemoryBlock(), this->Count(), start_index, end_index, key, func);
				}
				//数组是否为空
				bool IsEmpty()const {
					return this->data.GetUsedSize() == 0;
				}
				//获得数组长度
				size_t Count()const {
					return this->data.GetUsedSize();
				}
				//获得最大容量
				size_t Capacity()const {
					return this->data.GetMaxSize();
				}
				//设置数组扩容的增长倍数
				void SetRank(size_t rank) {
					if (rank >= 2)
						this->data.SetRank(rank);
				}
				//要求数组分配更多的内存
				void Expand(size_t append_size) {
					this->data.Expand(append_size);
				}
				//获得内存块
				T const*GetData()const {
					return this->data.GetMemoryBlock();
				}
				//索引器
				T const&operator[](index_t index)const {
					if (index < 0 || index >= this->Count())
						HL::Exception::Throw<HL::Exception::IndexOutOfException>(index, this->Count() - 1);
					return this->data[index];
				}
				//索引器
				T &operator[](index_t index) {
					if (index < 0 || index >= this->Count())
						HL::Exception::Throw<HL::Exception::IndexOutOfException>(index, this->Count() - 1);
					return this->data[index];
				}
				//索引器(无越界异常)
				T const&GetAt(index_t index)const {
					return this->data[index];
				}
				//索引器(无越界异常)
				T&GetAt(index_t index) {
					return this->data[index];
				}

				Array& operator=(Array const&rhs) {
					this->Clear();
					this->Add(rhs);
					return *this;
				}

				Array& operator=(Array &&lhs) {
					this->Clear();
					this->data = static_cast<ContainerT&&>(lhs.data);
					return *this;
				}

				virtual ~Array() {
					this->Clear();
				}

			public:
				//其他特性
				virtual uptr<Iteration::IEnumerator<T>> GetEnumerator()const final
				{
					return Reference::newptr<IterT>(const_cast<T*>(this->data.GetMemoryBlock()), const_cast<T*>(this->data.GetMemoryBlock() + this->data.GetUsedSize()));
				}
				//循环迭代
				template<class Fx>
				void Foreach(Fx func) {
					Internal::Foreach(this->data.GetMemoryBlock(), this->data.GetMemoryBlock() + this->Count(), func);
				}
				//从Allocator分配的内存托管成为数组
				static Array ManageFrom(T*array_ptr, size_t count, size_t total) {
					Array ret;
					ret.data.ManageFrom(array_ptr, count, total);
					return ret;
				}
				virtual void* ClonePtr()const final {
					return new Array(*this);
				}
			};


			template<class T>
			struct Interface::EnumerableSupportInterface<Array<T>>
			{
				typedef uptr<Iteration::Iterator<T>> IteratorType;
				typedef uptr<Iteration::Iterator<T const>> ConstIteratorType;
			};
		}
	}
}