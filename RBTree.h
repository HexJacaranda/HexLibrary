#pragma once
namespace HL
{
	namespace System
	{
		namespace Collection
		{
			namespace Tree
			{
				enum class NodeColor :bool
				{
					Red,
					Black
				};
				template<class TKey>
				struct Node
				{
					Node* Left = nullptr;
					Node* Right = nullptr;
					Node* Parent = nullptr;
					NodeColor Color = NodeColor::Black;
					Nullable::Nullable<TKey> Key;
					template<class L>
					Node(L&&key) :Key(std::forward<L>(key)) {}
					Node(NodeColor color) :Color(color) {}
					Node(Node const&) = default;
					Node(Node&&) = default;
					Node& operator=(Node const&) = default;
					Node& operator=(Node&&) = default;
					~Node() = default;
				};

				template<class T>
				struct DefaultComparator
				{
					inline int operator()(T const&left, T const& right)noexcept
					{
						if (left > right)
							return 1;
						else if (left < right)
							return -1;
						else
							return 0;
					}
				};

				template<class TKey, class ComparatorT = DefaultComparator<TKey>>
				class RBTree
				{
					typedef Node<TKey> NodeT;
					NodeT* m_root = nullptr;
					ComparatorT m_comparator;
					static NodeT NilNode;
				private:
					inline static bool IsNil(NodeT* target)noexcept {
						return target == &NilNode;
					}
					inline static NodeT* Nil()noexcept {
						return &NilNode;
					}
					void RotateLeft(NodeT* Target) {
						NodeT* adjacent = Target->Right;
						Target->Right = adjacent->Left;
						if (adjacent->Left != Nil())
							adjacent->Left->Parent = Target;
						adjacent->Parent = Target->Parent;
						if (Target->Parent == nullptr)
							m_root = adjacent;
						else if (IsLeft(Target))
							Target->Parent->Left = adjacent;
						else
							Target->Parent->Right = adjacent;
						adjacent->Left = Target;
						Target->Parent = adjacent;
					}
					void RotateRight(NodeT* Target) {
						NodeT* adjacent = Target->Left;
						Target->Left = adjacent->Right;
						if (adjacent->Right != Nil())
							adjacent->Right->Parent = Target;
						adjacent->Parent = Target->Parent;
						if (Target->Parent == nullptr)
							m_root = adjacent;
						else if (IsRight(Target))
							Target->Parent->Right = adjacent;
						else
							Target->Parent->Left = adjacent;
						adjacent->Right = Target;
						Target->Parent = adjacent;
					}
					template<class O>
					NodeT* Get(O const& key)const {
						NodeT* iterator = m_root;
						while (iterator != nullptr)
						{
							int val = const_cast<ComparatorT&>(m_comparator)(key, iterator->Key.GetObject());
							if (val < 0)
								iterator = iterator->Left;
							else if (val > 0)
								iterator = iterator->Right;
							else
								return iterator;
						}
						return nullptr;
					}
					NodeT* Put(TKey const& key, NodeT* node) {
						NodeT* target = nullptr;
						NodeT* try_node = m_root;
						node->Color = NodeColor::Red;
						node->Left = Nil();
						node->Right = Nil();
						while (try_node != Nil() && try_node != nullptr)
						{
							target = try_node;
							int val = m_comparator(key, try_node->Key.GetObject());
							if (val < 0)
								try_node = try_node->Left;
							else if (val > 0)
								try_node = try_node->Right;
							else
								return nullptr;
						}
						node->Parent = target;
						if (target == nullptr)
							m_root = node;
						else
						{
							int val = m_comparator(key, target->Key.GetObject());
							if (val < 0)
								target->Left = node;
							else if (val > 0)
								target->Right = node;
							else
								return nullptr;
						}
						FixUpInsert(node);
						return node;
					}
					static inline bool IsLeft(NodeT* target)noexcept {
						return target->Parent->Left == target;
					}
					void FixUpInsert(NodeT* node) {
						while (node->Parent != nullptr&&node->Parent->Color == NodeColor::Red)
						{
							if (IsLeft(node->Parent))
							{
								NodeT* uncle = node->Parent->Parent->Right;
								if (uncle != Nil() &&uncle->Color == NodeColor::Red)
								{
									node->Parent->Color = NodeColor::Black;
									uncle->Color = NodeColor::Black;
									node->Parent->Parent->Color = NodeColor::Red;
									node = node->Parent->Parent;
								}
								else
								{
									if (IsRight(node))
									{
										node = node->Parent;
										RotateLeft(node);
									}
									node->Parent->Color = NodeColor::Black;
									node->Parent->Parent->Color = NodeColor::Red;
									RotateRight(node->Parent->Parent);
								}
							}
							else
							{
								NodeT* uncle = node->Parent->Parent->Left;
								if (uncle != Nil() &&uncle->Color == NodeColor::Red)
								{
									node->Parent->Color = NodeColor::Black;
									uncle->Color = NodeColor::Black;
									node->Parent->Parent->Color = NodeColor::Red;
									node = node->Parent->Parent;
								}
								else
								{
									if (IsLeft(node))
									{
										node = node->Parent;
										RotateRight(node);
									}
									node->Parent->Color = NodeColor::Black;
									node->Parent->Parent->Color = NodeColor::Red;
									RotateLeft(node->Parent->Parent);
								}
							}
						}
						m_root->Color = NodeColor::Black;
					}
					void FixUpDeletion(NodeT* node) {
						while (node != m_root && node->Color == NodeColor::Black)
						{
							NodeT* aux = nullptr;
							if (IsLeft(node))
							{
								aux = node->Parent->Right;
								if (aux->Color == NodeColor::Red)
								{
									aux->Color = NodeColor::Black;
									node->Parent->Color = NodeColor::Red;
									RotateLeft(node->Parent);
									aux = node->Parent->Right;
								}
								if (aux->Left->Color == NodeColor::Black&&aux->Right->Color == NodeColor::Black)
								{
									aux->Color = NodeColor::Red;
									node = node->Parent;
								}
								else if (aux->Right->Color == NodeColor::Black)
								{
									aux->Left->Color = NodeColor::Black;
									aux->Color = NodeColor::Red;
									RotateRight(aux);
									aux = node->Parent->Right;
									aux->Color = node->Parent->Color;
									node->Parent->Color = NodeColor::Black;
									aux->Right->Color = NodeColor::Black;
									RotateLeft(node->Parent);
									node = m_root;
								}
							}
							else
							{
								aux = node->Parent->Left;
								if (aux->Color == NodeColor::Red)
								{
									aux->Color = NodeColor::Black;
									node->Parent->Color = NodeColor::Red;
									RotateRight(node->Parent);
									aux = node->Parent->Left;
								}
								if (aux->Right->Color == NodeColor::Black&&aux->Left->Color == NodeColor::Black)
								{
									aux->Color = NodeColor::Red;
									node = node->Parent;
								}
								else if (aux->Left->Color == NodeColor::Black)
								{
									aux->Right->Color = NodeColor::Black;
									aux->Color = NodeColor::Red;
									RotateLeft(aux);
									aux = node->Parent->Left;
									aux->Color = node->Parent->Color;
									node->Parent->Color = NodeColor::Black;
									aux->Left->Color = NodeColor::Black;
									RotateRight(node->Parent);
									node = m_root;
								}
							}
						}
						node->Color = NodeColor::Black;
					}
					NodeT* Successor(NodeT* node) {
						if (node == nullptr)
							return node;
						else if (node->Right != Nil())
						{
							node = node->Right;
							while (node->Left != Nil())
								node = node->Left;
							return node;
						}
						else
						{
							NodeT* parent = node->Parent;
							while (parent != nullptr &&node == parent->Right)
							{
								node = parent;
								parent = parent->Parent;
							}
							return parent;
						}
						return nullptr;
					}
					NodeT* Delete(NodeT* node) {
						NodeT* qnode = nullptr;
						NodeT* tnode = nullptr;
						if (node->Left == Nil() || node->Right == Nil())
							qnode = node;
						else
							qnode = Successor(node);
						if (qnode->Left != Nil())
							tnode = qnode->Left;
						else
							tnode = qnode->Right;
						tnode->Parent = qnode->Parent;
						if (qnode->Parent == nullptr)
							m_root = tnode;
						else if (IsLeft(qnode))
							qnode->Parent->Left = tnode;
						else
							qnode->Parent->Right = tnode;
						if (qnode != node)
							node->Key = std::move(qnode->Key);
						if (qnode->Color == NodeColor::Black)
							FixUpDeletion(tnode);
						return qnode;
					}
					void Destruct(NodeT*& node)
					{
						if (node == Nil())
							return;
						if (node->Left != Nil())
							Destruct(node->Left);
						if (node->Right != Nil())
							Destruct(node->Right);
						delete node;
						node = nullptr;
					}
				public:
					void Remove(TKey const&key) {
						NodeT* target = Get(key);
						if (target == nullptr)
							return;
						delete Delete(target);
					}
					template<class O>
					TKey const& GetValue(O const& other)const {
						NodeT* target = Get(other);
						if (target == nullptr) {
							//ex
						}
						return target->Key.GetObject();
					}
					template<class O>
					TKey & GetValue(O const& other) {
						NodeT* target = Get(other);
						if (target == nullptr) {
							//ex
						}
						return target->Key.GetObject();
					}
					void Add(TKey const&key) {
						NodeT* node = new NodeT(key);
						NodeT* target = Put(key, node);
						if (node == nullptr)
							delete node;
					}
					void Add(TKey &&key) {
						NodeT* node = new NodeT(std::move(key));
						NodeT* target = Put(key, node);
						if (node == nullptr)
							delete node;
					}
					inline bool Contains(TKey const&key)const noexcept {
						return Get(key) != nullptr;
					}
					TKey const& Max()const {
						NodeT* iterator = nullptr;
						NodeT* try_node = m_root;
						while (try_node != Nil())
						{
							iterator = try_node;
							try_node = try_node->Right;
						}
						if (iterator == nullptr)
						{
							//ex
						}
						return iterator->Key;
					}
					TKey const& Min()const {
						NodeT* iterator = nullptr;
						NodeT* try_node = m_root;
						while (try_node != Nil())
						{
							iterator = try_node;
							try_node = try_node->Left;
						}
						if (iterator == nullptr)
						{
							//ex
						}
						return iterator->Key;
					}
					~RBTree() {
						if (m_root != nullptr)
						{
							Destruct(m_root);
							m_root = nullptr;
						}
					}
				};

				template<class TKey, class ComparatorT>
				Node<TKey> RBTree<TKey, ComparatorT>::NilNode = Node<TKey>(NodeColor::Black);

			}
		}
	}
}