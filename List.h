#pragma once
namespace HL
{
	namespace System
	{
		namespace Collection
		{
			namespace Generic
			{
				template<class U>
				class List :public Linq::LinqBase<U*, List<U>>
				{
					LinearMemoryManager<U> m_memory;
				public:
					List(Memory::IAllocator* Allocator = Memory::DefaultAllocator::Instance()) :m_memory(Allocator) {
					}
					List(std::initializer_list<U> const&InitList) :m_memory(InitList.size(), Memory::DefaultAllocator::Instance()) {
						for (U const&var : InitList)
							m_memory.Append(var);
					}
					List(size_t Capactiy, Memory::IAllocator* Allocator = Memory::DefaultAllocator::Instance()) :m_memory(Capactiy, Allocator) {
					}
					List(List const&) = default;
					List(List&&)noexcept = default;
					List& operator=(List const&) = default;
					List& operator=(List&&) = default;
					inline void Add(U const&Target) {
						m_memory.Append(Target);
					}
					inline void Add(U && Target) {
						m_memory.Append(std::forward<U>(Target));
					}
					inline void Add(U const*Array, size_t Size) {
						m_memory.Append(Array, Size);
					}
					inline void Add(const List<U>& Array) {
						m_memory.Append(Array.m_memory.GetDataPtr(), Array.Count());
					}
					inline size_t IndexOf(U const& Key)const noexcept {
						return Algorithm::IndexOf(m_memory.GetDataPtr(), m_memory.UsedCount(), Key);
					}
					inline U* begin() {
						return this->m_memory.GetDataPtr();
					}
					inline U const* begin()const {
						return this->m_memory.GetDataPtr();
					}
					inline U* end() {
						return this->m_memory.GetDataPtr() + this->Count();
					}
					inline U const* end()const {
						return this->m_memory.GetDataPtr() + this->Count();
					}
					inline void RemoveAt(size_t Index) {
						m_memory.Remove(Index, Index + 1);
					}
					inline U&operator[](size_t Index) {
						return m_memory.At(Index);
					}
					inline U const&operator[](size_t Index)const {
						return m_memory.At(Index);
					}
					inline size_t Count()const noexcept {
						return this->m_memory.UsedCount();
					}
					inline size_t Capacity()const {
						return this->m_memory.MaxCount();
					}
					inline void Clear() {
						m_memory.Empty();
					}
					~List() {
						this->Clear();
					}
				};
			}
		}
		namespace Pointer
		{
			template<class U>
			struct IteratorInterface<Collection::Generic::List<U>>
			{
				typedef U* IteratorT;
				typedef const U* ConstIteratorT;
			};
		}
	}
}