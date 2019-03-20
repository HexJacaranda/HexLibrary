#pragma once
namespace HL
{
	namespace System
	{
		namespace Collection
		{
			namespace Tree
			{
				template<class TKey>
				struct AVLNode
				{
					AVLNode* Left = nullptr;
					AVLNode* Right = nullptr;
					AVLNode* Parent = nullptr;
					Nullable::Nullable<TKey> Key;
					size_t Height = 1;

					template<class L>
					AVLNode(L&&key) :Key(std::forward<L>(key)) {}
					AVLNode(AVLNode const&) = default;
					AVLNode(AVLNode&&) = default;
					AVLNode& operator=(AVLNode const&) = default;
					AVLNode& operator=(AVLNode&&) = default;
					~AVLNode() = default;

					static inline size_t Max(size_t left, size_t right)noexcept {
						return left > right ? left : right;
					}
					static inline size_t GetHeight(AVLNode* node)noexcept {
						return node == nullptr ? 0U : node->Height;
					}
					inline void ReevaluateHeight()noexcept {
						Height = Max(GetHeight(Left), GetHeight(Right)) + 1;
					}
					inline intptr_t BalanceFactor() {
						return (intptr_t)GetHeight(Left) - (intptr_t)GetHeight(Right);
					}
				};

				template<class TKey,class Comparator = DefaultComparator<TKey>>
				class AVLTree
				{
					typedef AVLNode<TKey> NodeT;
					NodeT* m_root;
					Comparator m_comparator;
					static inline bool IsLeft(NodeT* node)noexcept {
						return node == node->Parent->Left;
					}
					void RotateLeft(NodeT* Target)
					{
						NodeT* adjacent = Target->Right;
						Target->Right = adjacent->Left;
						if (adjacent->Left != nullptr)
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

						adjacent->ReevaluateHeight();
						Target->ReevaluateHeight();
					}
					void RotateRight(NodeT* Target)
					{
						NodeT* adjacent = Target->Left;
						Target->Left = adjacent->Right;
						if (adjacent->Right != nullptr)
							adjacent->Right->Parent = Target;
						adjacent->Parent = Target->Parent;
						if (Target->Parent == nullptr)
							m_root = adjacent;
						else if (IsLeft(Target))
							Target->Parent->Left = adjacent;
						else
							Target->Parent->Right = adjacent;
						adjacent->Right = Target;
						Target->Parent = adjacent;

						adjacent->ReevaluateHeight();
						Target->ReevaluateHeight();
					}
					void Put(TKey const&key, NodeT* node)
					{
						NodeT* target = nullptr;
						NodeT* try_node = m_root;
						while (try_node != nullptr)
						{
							target = try_node;
							int val = m_comparator(key, try_node->Key.GetObject());
							if (val > 0)
								try_node = try_node->Right;
							else if (val < 0)
								try_node = try_node->Left;
							else
							{
								//Duplicate key
							}
						}
						node->Parent = target;
						if (target == nullptr)
							m_root = node;
						else
						{
							int val = m_comparator(key, target->Key.GetObject());
							if (val > 0)
								target->Right = node;
							else if (val < 0)
								target->Left = node;
						}
						FixUpPut(node);
					}
					NodeT* Get(TKey const&key)
					{
						NodeT* iterator = m_root;
						while (iterator != nullptr)
						{
							int val = m_comparator(key, iterator->Key.GetObject());
							if (val > 0)
								iterator = iterator->Right;
							else if (val < 0)
								iterator = iterator->Left;
							else
								return iterator;
						}
						return nullptr;
					}
					void FixUpPut(NodeT* node)
					{
						NodeT* iterator = node;
						while (iterator != nullptr)
						{
							iterator->ReevaluateHeight();
							intptr_t balance_factor = iterator->BalanceFactor();
							int left_val = iterator->Left == nullptr ? 0 : m_comparator(node->Key.GetObject(), iterator->Left->Key.GetObject());
							int right_val = iterator->Right == nullptr ? 0 : m_comparator(node->Key.GetObject(), iterator->Right->Key.GetObject());
							if (balance_factor > 1 && left_val < 0)
								RotateRight(iterator);
							else if (balance_factor < -1 && right_val > 0)
								RotateLeft(iterator);
							else if (balance_factor > 1 && left_val > 0)
							{
								RotateLeft(iterator->Left);
								RotateRight(iterator);
							}
							else if (balance_factor < -1 && right_val < 0)
							{
								RotateRight(iterator->Right);
								RotateLeft(iterator);
							}
							iterator = iterator->Parent;
						}
					}
					void Destruct(NodeT* node)
					{
						if (node->Left != nullptr)
							Destruct(node->Left);
						if (node->Right != nullptr)
							Destruct(node->Right);
						delete node;
					}
					void Delete(NodeT* node)
					{
						NodeT* to_fix = nullptr;
						if (node->Left == nullptr || node->Left == nullptr)
						{
							NodeT* child = node->Left == nullptr ? node->Right : node->Left;
							if (node->Parent == nullptr)
								m_root = child;
							else
							{
								if (IsLeft(node))
									node->Parent->Left = child;
								else
									node->Parent->Right = child;
							}
							to_fix = node->Parent;
							delete node;
						}
						else
						{
							NodeT* successor = Successor(node);
							node->Key = std::move(successor->Key);
							if (IsLeft(successor))
								successor->Parent->Left = nullptr;
							else
								successor->Parent->Right = nullptr;
							to_fix = successor->Parent;
							delete successor;
						}
						FixUpDelete(to_fix);
					}
					NodeT* Successor(NodeT* node)
					{
						NodeT* iterator = node->Left;
						while (iterator->Right != nullptr)
							iterator = iterator->Right;
						return iterator;
					}
					void FixUpDelete(NodeT* node)
					{
						NodeT* iterator = node;
						while (iterator != nullptr)
						{
							iterator->ReevaluateHeight();
							intptr_t balance = iterator->BalanceFactor();
							if (iterator->Left != nullptr)
							{
								if (balance > 1 && iterator->Left->BalanceFactor() >= 0)
									RotateRight(iterator);
								else if (balance > 1 && iterator->Left->BalanceFactor() < 0)
								{
									RotateLeft(iterator->Left);
									RotateRight(iterator);
								}
							}
							else if (iterator->Right != nullptr)
							{
								if (balance > 1 && iterator->Right->BalanceFactor() <= 0)
									RotateLeft(iterator);
								else if (balance > 1 && iterator->Right->BalanceFactor() > 0)
								{
									RotateRight(iterator->Right);
									RotateLeft(iterator);
								}
							}
							iterator = iterator->Parent;
						}
					}
				public:
					void Add(TKey const&key) {
						NodeT* node = new NodeT(key);
						Put(key, node);
					}
					void Remove(TKey const&key) {
						NodeT* node = Get(key);
						if (node == nullptr)
						{
							//
						}
						Delete(node);
					}
					~AVLTree() {
						if (m_root != nullptr)
						{
							Destruct(m_root);
							m_root = nullptr;
						}
					}
				};
			}
		}
	}
}