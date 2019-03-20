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
				class StackNode
				{
				public:
					Nullable::Nullable<U> Object;
					StackNode* Next = nullptr;
				};
				template<class U>
				class StackIterator
				{
					StackNode<U>* m_node;
				public:
					StackIterator(StackNode<U>*Node) :m_node(Node) {}
					StackIterator(StackIterator&&) = default;
					StackIterator(StackIterator const&) = default;
					StackIterator& operator=(StackIterator&&) = default;
					StackIterator& operator=(StackIterator const&) = default;
					StackIterator&operator++() {
						m_node = m_node->Next;
						return *this;
					}
					inline U& operator*()noexcept {
						return m_node->Object.GetObject();
					}
					inline U const& operator*()const noexcept {
						return m_node->Object.GetObject();
					}
					inline bool operator==(StackIterator const&other)noexcept {
						return m_node == other.m_node;
					}
					inline bool operator!=(StackIterator const&other)noexcept {
						return m_node != other.m_node;
					}
				};
				template<class U>
				class Stack
				{
					typedef StackNode<U> Node;
					Node* m_node = nullptr;
				public:
					Stack()noexcept {}
					~Stack() {
						if (m_node != nullptr)
						{
							while (!IsEmpty())
								Pop();
							m_node = nullptr;
						}
					}
					template<class O>
					void Push(O&&Object) {
						if (m_node == nullptr)
						{
							m_node = new Node();
							m_node->Object = std::forward<O>(Object);
						}
						else
						{
							Node* new_one = new Node();
							new_one->Object = std::forward<O>(Object);
							new_one->Next = m_node;
							m_node = new_one;
						}
					}
					inline U const& Top()const {
						return this->m_node->Object.GetObject();
					}
					inline U& Top() {
						return this->m_node->Object.GetObject();
					}
					void Pop() {
						if (this->m_node != nullptr)
						{
							Node* out = this->m_node;
							this->m_node = this->m_node->Next;
							delete out;
						}
					}
					inline StackIterator<U> begin()noexcept {
						return m_node;
					}
					inline StackIterator<U> end()noexcept {
						return nullptr;
					}
					inline StackIterator<U> begin()const noexcept {
						return (Node*)m_node;
					}
					inline StackIterator<U> end()const noexcept {
						return nullptr;
					}
					inline bool IsEmpty()const {
						return m_node == nullptr;
					}
				};
			}
		}
	}
}