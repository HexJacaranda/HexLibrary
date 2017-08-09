#pragma once
namespace HL
{
	namespace System
	{
		namespace Generic
		{
			//优先级指数
			typedef index_t PriorityIndex;

			namespace Inner
			{
				template<class T>
				class QueueNode
				{
				public:
					QueueNode*Next;
					QueueNode*Prev;
					T*Data;
					void Construct(T const&Target) {
						Next = nullptr;
						Prev = nullptr;
						Data = (T*)Memory::Allocator::Alloc(sizeof(T));
						Memory::PlacementNew(Data, Target);
					};
					void Clear() {
						if (this->Data != nullptr)
						{
							this->Data->~T();
							Memory::Allocator::Free(Data);
						}
					}
				};

				template<class T>
				class PriorityQueueItem
				{
				public:
					PriorityIndex Index;
					T*Data;
					void Construct(T const&Target) {
						Data = (T*)Memory::Allocator::Alloc(sizeof(T));
						Memory::PlacementNew(Data, Target);
					};
					void Empty() {
						if (this->Data != nullptr)
						{
							this->Data->~T();
							Index = -1;
						}
					}
					void Set(T const&Target) {
						if (Index != -1)
							this->Empty();
						if (Data == nullptr)
							Data = (T*)Memory::Allocator::Alloc(sizeof(T));
						Memory::PlacementNew(Data, Target);
					}
					void Clear() {
						if (this->Data != nullptr)
						{
							if (Index != -1)
								this->Data->~T();
							Memory::Allocator::Free(Data);
						}
					}
				};
			}
			//队列对象
			template<class T>
			class Queue
			{
			private:
				typedef Inner::QueueNode<T> Node;
				Node*m_head = nullptr;
				Node*m_tail = nullptr;
				size_t m_count = 0;
				void Construct(Queue const&rhs) {
					Node*iter = rhs.m_head;
					while (iter != nullptr)
					{
						Node*add = (Node*)Memory::Allocator::Alloc(sizeof(Node));
						add->Construct(*iter->Data);
						if (m_tail != nullptr)
							m_tail->Next = add;
						if (m_head == nullptr)
							m_head = add;
						m_tail = add;
					}
					m_count = rhs.m_count;
				}
			public:
				Queue(){}
				Queue(Queue const&rhs) {
					Construct(rhs);
				}
				Queue(Queue &&lhs) {
					this->m_head = lhs.m_head;
					this->m_tail = lhs.m_tail;
					lhs.m_head = nullptr;
					lhs.m_tail = nullptr;
					this->m_count = lhs.m_count;
				}
				size_t Count()const {
					return m_count;
				}
				void Enqueue(T const& Object) {
					Node*ptr = (Node*)Memory::Allocator::Alloc(sizeof(Node));
					ptr->Construct(Object);
					if (m_head == nullptr)
					{
						this->m_head = ptr;
						this->m_tail = ptr;
					}
					else
					{
						ptr->Next = this->m_head;
						this->m_head->Prev = ptr;
						this->m_head = ptr;
					}
					++m_count;
				}
				T Dequeue() {
					if (this->m_tail == nullptr)
						Exception::Throw<UnHandledException>();
					Node*last = this->m_tail;
					if (this->m_tail == this->m_head)
					{
						this->m_head = nullptr;
						this->m_tail = nullptr;
					}
					else
					{
						this->m_tail = this->m_tail->Prev;
					}
					T Object = *(last->Data);
					last->Clear();
					Memory::Allocator::Free(last);
					--m_count;
					return static_cast<T&&>(Object);
				}
				void Clear() {
					Node*iter = this->m_head;
					Node*next = nullptr;
					while (iter != nullptr)
					{
						iter->Clear();
						next = iter->Next;
						Memory::Allocator::Free(iter);
						iter = next;
					}
					this->m_count = 0;
					this->m_head = nullptr;
					this->m_tail = nullptr;
				}
				Queue&operator=(Queue const&rhs) {
					this->Clear();
					this->Construct(rhs);
					return *this;
				}
				Queue&operator=(Queue &&lhs) {
					this->Clear();
					this->m_head = lhs.m_head;
					this->m_tail = lhs.m_tail;
					lhs.m_head = nullptr;
					lhs.m_tail = nullptr;
					this->m_count = lhs.m_count;
					return *this;
				}
				//遍历
				template<class Functor>
				void Foreach(Functor Target) {
					Node*iter = this->m_head;
					while (iter != nullptr)
						Target(*iter->Data);
				}
				~Queue()
				{
					this->Clear();
				}
			};

			//优先级队列
			template<class T>
			class PriorityQueue
			{
				inline static void Parent(index_t Index) {
					return (Index - 1) / 2;
				}
				inline static void LeftChild(index_t Index) {
					return (2 * Index + 1);
				}
				inline static void RightFromLeft(index_t Index) {
					return ++Index;
				}
				typedef Inner::PriorityQueueItem<T> Item;
				Generic::array<Item*>m_items;
				size_t m_count = 0;
			public:
				PriorityQueue(size_t Capacity):m_items(Capacity){}
				PriorityQueue(){}
				//返回优先级最高的对象
				T Dequeue() {
					Item*ptr = m_items[0];
					if (ptr != nullptr)
					{
						if (ptr->Index != -1)
						{
							--m_count;
							ptr->Index = -1;
							T Object = *ptr->Data;
							ptr->Empty();
							return static_cast<T&&>(Object);
						}
					}
					Exception::Throw<Exception::UnHandledException>();
				}
				void Enqueue(T const&Object,PriorityIndex Priority) {
					if (this->m_count == this->m_items.Count())
						this->m_items.ExpandTo(this->m_count * 2);
					index_t index = this->m_count;
					//进行换序
					while (index > 0)
					{
						index_t parent_index = Parent(index);
						if (Priority < m_items[parent_index]->Index)
						{
							m_items[index] = m_items[parent_index];
							index = parent_index;
						}
						else
							break;
					}
					Item*ptr = m_items[index];
					if (ptr == nullptr)
						ptr = (Item*)Memory::Allocator::Alloc(sizeof(Item));
					ptr->Set(Object);
					ptr->Index = Priority;
					
				}
			};
		}
	}
}