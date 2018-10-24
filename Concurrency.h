#pragma once
#include <atomic>
namespace HL
{
	namespace System
	{
		namespace Concurrency
		{
			template<class T>
			class ConcurrentNodeWrapper
			{
			public:
				int ExternalCount;//外部计数
				ConcurrentNode<T> *Ptr;//指针
			};
			template<class T>
			class ConcurrentNode
			{
			public:
				ConcurrentNode() :InternalCount(0) {}
				ConcurrentNode(T const&Target) :Data(Target), InternalCount(0) {}
				ConcurrentNodeWrapper<T> Next;
				T Data;
				std::atomic<int> InternalCount;
			};

			//并发栈
			template<class T>
			class ConcurrentStack
			{
				typedef ConcurrentNode<T> Node;
				typedef ConcurrentNodeWrapper<T> Wrapper;
				std::atomic<Wrapper> m_head;
				void IncHead(Wrapper &old)
				{
					Wrapper new_wrapper;
					do
					{
						new_wrapper = old;
						new_wrapper.ExternalCount++;
					} while (!m_head.compare_exchange_strong(old, new_wrapper, std::memory_order_acquire, std::memory_order_relaxed));
					old.ExternalCount = new_wrapper.ExternalCount;
				}
			public:
				ConcurrentStack()
				{
					m_head.store(Wrapper());
				}
				ConcurrentStack(ConcurrentStack const&) = delete;
				ConcurrentStack&operator=(ConcurrentStack const&) = delete;
				void Push(T const&Target)
				{
					Wrapper wrapper;
					wrapper.ExternalCount = 1;
					wrapper.Ptr = new Node(Target);
					wrapper.Ptr->Next = m_head.load(std::memory_order_relaxed);
					while (!m_head.compare_exchange_weak(wrapper.Ptr->Next, wrapper, std::memory_order_release, std::memory_order_relaxed));
				}
				bool Pop(T&Target)
				{
					Wrapper old_head = m_head.load(std::memory_order_relaxed);
					for (;;)
					{
						IncHead(old_head);
						Node*node = old_head.Ptr;
						if (!node)
							return false;
						if (m_head.compare_exchange_strong(old_head, node->Next,std::memory_order_relaxed))
						{
							Target = Forward(node->Data);
							int count_inc = old_head.ExternalCount - 2;
							if (node->InternalCount.fetch_add(count_inc, std::memory_order_release) == -count_inc)//计数add之后为0时,即之前为负的inc
							{
								delete node;
								return true;
							}
							else if (node->InternalCount.fetch_sub(1, std::memory_order_relaxed) == 1)
							{
								node->InternalCount.load(std::memory_order_acquire);
								delete node;
							}
						}
					}
				}
				bool Pop()
				{
					Wrapper old_head = m_head.load(std::memory_order_relaxed);
					for (;;)
					{
						IncHead(old_head);
						Node*node = old_head.Ptr;
						if (!node)
							return false;
						if (m_head.compare_exchange_strong(old_head, node->Next, std::memory_order_relaxed))
						{
							int count_inc = old_head.ExternalCount - 2;
							if (node->InternalCount.fetch_add(count_inc, std::memory_order_release) == -count_inc)//计数add之后为0时,即之前为负的inc
							{
								delete node;
								return true;
							}
							else if (node->InternalCount.fetch_sub(1, std::memory_order_relaxed) == 1)
							{
								node->InternalCount.load(std::memory_order_acquire);
								delete node;
							}
						}
					}
				}
				~ConcurrentStack() {
					while (Pop());
				}
			};
			//并发队列
			template<class T>
			class ConcurrentQueue
			{
				typedef ConcurrentNode<T> Node;
				typedef ConcurrentNodeWrapper<T> Wrapper;
				std::atomic<Wrapper> m_head;
				std::atomic<Wrapper> m_tail;
				static void Inc(std::atomic<Wrapper>&Target, Wrapper&Old)
				{
					Wrapper New;
					do
					{
						New = Old;
						++New.ExternalCount;
					} while (!Target.compare_exchange_strong(Old, New));
					Old.ExternalCount = New.ExternalCount;
				}
			public:
				ConcurrentQueue() {
					Wrapper null;
					m_head.store(null);
					m_tail.store(null);
				}
				ConcurrentQueue(ConcurrentQueue const&) = delete;
				void Enqueue(T const&Target) {
					Wrapper wrapper;
					wrapper.ExternalCount = 1;
					wrapper.Ptr = new Node(Target);
					wrapper.Ptr->Next = m_tail.load();
					while (!m_tail.compare_exchange_weak(wrapper.Ptr->Next, wrapper));
				}
				bool Dequeue(T&Target) {

				}
			};
		}
	}
}
