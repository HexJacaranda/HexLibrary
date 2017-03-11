#pragma once
namespace HL
{
	namespace System
	{
		namespace Memory
		{
			template<class T, class...Args>
			inline T* PlacementNew(T*p, Args&&...args) {
				return new(p)T(args...);
			}
			template<class T>
			inline T* PlacementNew() { return new T(); }
			template<class TT>
			static inline void Deconstruct(TT*p) {
				p->TT::~TT();
			}
			template<class TT>
			static inline void Deconstruct(TT*p, size_t len) {
				while (len-- > 0) {
					(p++)->TT::~TT();
				}
			}

			template<class T1, class T2>
			static inline void Construct(T1* pdest, T2 const* psource, size_t len) {
				while (len-- > 0)
					PlacementNew(pdest++, *psource++);
			}
			template<class T1, class T2>
			static inline void ConstructWith(T1*pdest, T2 const&source, size_t len) {
				while (len-- > 0)
					PlacementNew(pdest++, source);
			}
			template<class T1, class T2>
			static inline void ConstructWith(T1*pdest, T2 &source, size_t len) {
				while (len-- > 0)
					PlacementNew(pdest++, source);
			}
			template<typename T>
			inline void Swap(T p1, T p2) {
				T t = p1;
				p1 = p2;
				p2 = t;
			}

			template<class TKey,class TValue>
			class KeyValuePair {
			public:
				TKey Key;
				TValue Value;
			};

			template<class T>
			class MemoryManager
			{
				T*m_data = nullptr;
				size_t m_used = 0;
				size_t m_max = 0;
				size_t m_rank = 2;
				void internal_ctor(size_t, size_t);
				void internal_ctor(const T*, size_t, size_t);
				void internal_resize(size_t);
			public:
				MemoryManager();
				MemoryManager(size_t);
				MemoryManager(size_t, size_t);
				MemoryManager(T const*, size_t, size_t);
				MemoryManager(MemoryManager const&);
				MemoryManager(MemoryManager&&);
				void Append(T const&);
				template<class AnyT>
				void Append(AnyT const&);
				void Append(T const*, size_t);
				template<class AnyT>
				void Append(AnyT const*, size_t);
				void Append(T&&);
				void Append(MemoryManager const&);
				void ReWrite(T const*, size_t);
				void SwapWith(MemoryManager&);
				void MoveBackward(index_t, size_t);
				void Remove(index_t, index_t);
				void CopyTo(T*, size_t);
				void CopyTo(T*, index_t, size_t);
				void Clean();
				void ManageFrom(T*, size_t, size_t);
				void ShrinkToFit();
				void Expand(size_t);
				T*GetMemoryBlock();
				T const*GetMemoryBlock()const;
				void SetRank(size_t);
				void SetUsed(size_t);
				size_t GetUsedSize()const;
				size_t GetMaxSize()const;
				size_t GetFreeSize()const;
				T&operator[](index_t);
				T const&operator[](index_t)const;
				MemoryManager&operator=(MemoryManager const&);
				MemoryManager&operator=(MemoryManager&&);
				~MemoryManager();
			};

			template<class T>
			inline void MemoryManager<T>::internal_ctor(size_t count, size_t rank)
			{
				if (count == 0)
					m_data = nullptr;
				else
					m_data = (T*)Allocator::AllocBlank(count * sizeof(T));
				m_used = 0;
				m_rank = rank;
				m_max = count;
			}
			template<class T>
			inline void MemoryManager<T>::internal_ctor(const T*source, size_t count, size_t rank)
			{
				m_data = (T*)Allocator::AllocBlank(sizeof(T)*(count + 1)*rank);
				if (!Template::IsValueType<T>::R)//如果不为值类型,需要PlacementNew构造
					for (size_t i = 0; i < count; i++)
						PlacementNew(m_data + i, source[i]);
				else
					Allocator::MemoryCopy(source, m_data, 0, sizeof(T)*count);
				m_used = count;
				m_max = (count + 1)*rank;
				m_rank = rank;
			}
			template<class T>
			inline void MemoryManager<T>::internal_resize(size_t count)
			{
				if (m_rank == 0)
					m_rank = 2;
				m_data = (T*)Allocator::ReAllocTransfer(m_data, (m_rank*(m_max + count)) * sizeof(T), sizeof(T)*m_used);
				if (m_max == 0)
					m_max = (m_rank*count);
				else
					m_max *= m_rank;
			}
			template<class T>
			inline MemoryManager<T>::MemoryManager()
			{
				internal_ctor(0, 2);
			}
			template<class T>
			inline MemoryManager<T>::MemoryManager(size_t count)
			{
				internal_ctor(count, 2);
			}
			template<class T>
			inline MemoryManager<T>::MemoryManager(size_t count, size_t rank)
			{
				internal_ctor(count, rank);
			}
			template<class T>
			inline MemoryManager<T>::MemoryManager(T const *source, size_t count, size_t rank = 2)
			{
				internal_ctor(source, count, rank);
			}
			template<class T>
			inline MemoryManager<T>::MemoryManager(MemoryManager const &rhs)
			{
				internal_ctor(rhs.m_data, rhs.m_used, rhs.m_rank);
			}
			template<class T>
			inline MemoryManager<T>::MemoryManager(MemoryManager &&lhs) :m_data(lhs.m_data), m_used(lhs.m_used), m_max(lhs.m_max), m_rank(lhs.m_rank)
			{
				lhs.m_data = nullptr;
			}

			template<class T>
			inline void MemoryManager<T>::Append(T const &rhs)
			{
				if (m_used == m_max)
					internal_resize(1);
				PlacementNew(m_data + m_used, rhs);
				m_used++;
			}

			template<class T>
			inline void MemoryManager<T>::Append(T const *source, size_t count)
			{
				if (m_used + count >= m_max)
					internal_resize(count);
				if (Template::IsValueType<T>::R)
					Allocator::MemoryCopy(source, m_data + m_used, 0, count * sizeof(T));
				else
					Construct(m_data + m_used, source, count);
				m_used += count;
			}

			template<class T>
			template<class AnyT>
			inline void MemoryManager<T>::Append(AnyT const &rhs)
			{
				if (m_used == m_max)
					internal_resize(1);
				PlacementNew(m_data + m_used, (T)rhs);
				m_used++;
			}

			template<class T>
			template<class AnyT>
			inline void MemoryManager<T>::Append(AnyT const *source, size_t count)
			{
				if (m_used + count >= m_max)
					internal_resize(count);
				for (size_t i = 0; i < count; i++)
					PlacementNew(m_data + m_used + i, (T)source[i]);

				m_used += count;
			}

			template<class T>
			inline void MemoryManager<T>::Append(T &&lhs)
			{
				if (m_used == m_max)
					internal_resize(1);
				PlacementNew(m_data + m_used, lhs);
				m_used++;
			}

			template<class T>
			inline void MemoryManager<T>::Append(MemoryManager const &rhs)
			{
				Append(rhs.m_data, rhs.m_used);
			}

			template<class T>
			inline void MemoryManager<T>::ReWrite(T const *source, size_t count)
			{
				if (count >= m_max)
					internal_resize(count);
				if (!Template::IsValueType<T>::R)
				{
					if (count <= m_max)
						Deconstruct(m_data + count, m_max - count);
					Construct(m_data, source, count);
				}
				else
					Allocator::MemoryCopy(source, m_data, 0, sizeof(T)*count);
				m_used = count;
			}

			template<class T>
			inline void MemoryManager<T>::SwapWith(MemoryManager &lhs)
			{
				Swap(lhs.m_max, this->m_max);
				Swap(lhs.m_rank, this->m_rank);
				Swap(lhs.m_used, this->m_used);
				Swap(lhs.m_data, this->m_data);
			}

			template<class T>
			inline void MemoryManager<T>::MoveBackward(index_t index, size_t jump)
			{
				if (jump == 0)
					return;
				if (jump + m_used >= m_max)
					internal_resize(jump);
				for (index_t i = (m_used + jump) * sizeof(T); i >= (index + jump) * sizeof(T); i--)
					*((unsigned char*)m_data + i) = *((unsigned char*)m_data + i - jump * sizeof(T));

				if (!Template::IsValueType<T>::R)
					for (index_t i = index * sizeof(T); i < (index + jump) * sizeof(T); i++)//置为0
						*((unsigned char*)m_data + i) = 0;

				m_used += jump;
			}

			template<class T>
			inline void MemoryManager<T>::Remove(index_t start, index_t end)
			{
				if (Template::IsValueType<T>::R)
					Allocator::MemoryCopy(m_data + end + 1, m_data + start, 0, sizeof(T)*(m_used - end));
				else
				{
					Deconstruct(m_data + start, end - start + 1);
					Allocator::MemoryCopy(m_data + end + 1, m_data + start, 0, sizeof(T)*(m_used - end));
					Allocator::SetBlank(m_data + m_used - (end - start), m_data + m_used);
				}
				m_used -= (end - start + 1);
			}

			template<class T>
			inline void MemoryManager<T>::CopyTo(T *dest, size_t count)
			{
				CopyTo(dest, 0, count);
			}

			template<class T>
			inline void MemoryManager<T>::CopyTo(T *dest, index_t index, size_t count)
			{
				if (index >= m_used || count == 0)
					return;
				if (Template::IsValueType<T>::R)
					Allocator::MemoryCopy(m_data + index, dest, 0, sizeof(T)*count);
				else
				{
					for (index_t i = index; i < m_used; i++)
						dest[i - index] = m_data[i];
				}
			}

			template<class T>
			inline void MemoryManager<T>::Clean()
			{
				if (m_data) {
					if (!Template::IsValueType<T>::R)
						Deconstruct(m_data, m_used);
					Allocator::Free(m_data);
				}
				m_data = nullptr;
				m_max = 0;
				m_used = 0;
				m_rank = 0;
			}

			template<class T>
			inline void MemoryManager<T>::ManageFrom(T *source, size_t count, size_t total)
			{
				this->Clean();
				this->m_data = source;
				this->m_used = count;
				this->m_max = total;
			}

			template<class T>
			inline void MemoryManager<T>::ShrinkToFit()
			{
				if (m_used != m_max)
				{
					if (m_used == 0)
						Clean();
					else
						internal_resize(m_used);
				}
			}

			template<class T>
			inline void MemoryManager<T>::Expand(size_t size)
			{
				if (this->m_used + size > this->m_max)
					this->internal_resize(size);
			}

			template<class T>
			inline T * MemoryManager<T>::GetMemoryBlock()
			{
				return this->m_data;
			}

			template<class T>
			inline T const * MemoryManager<T>::GetMemoryBlock() const
			{
				return this->m_data;
			}

			template<class T>
			inline void MemoryManager<T>::SetRank(size_t value)
			{
				m_used = value;
			}

			template<class T>
			inline void MemoryManager<T>::SetUsed(size_t value)
			{
				m_used = value;
			}

			template<class T>
			inline size_t MemoryManager<T>::GetUsedSize() const
			{
				return this->m_used;
			}

			template<class T>
			inline size_t MemoryManager<T>::GetMaxSize() const
			{
				return this->m_max;
			}

			template<class T>
			inline size_t MemoryManager<T>::GetFreeSize() const
			{
				return this->m_max - this->m_used;
			}

			template<class T>
			inline T & MemoryManager<T>::operator[](index_t index)
			{
				return this->m_data[index];
			}

			template<class T>
			inline T const & MemoryManager<T>::operator[](index_t index) const
			{
				return this->m_data[index];
			}

			template<class T>
			inline MemoryManager<T> & MemoryManager<T>::operator=(MemoryManager const &rhs)
			{
				if (rhs.m_used >= this->m_max)
					internal_resize(rhs.m_used);
				if (Template::IsValueType<T>::R)
					Allocator::MemoryCopy(rhs.m_data, this->m_data, 0, sizeof(T)*rhs.m_used);
				else
					Construct(m_data, rhs.m_data, rhs.m_used);
				m_used = rhs.m_used;
				m_rank = rhs.m_rank;
				return *this;
			}

			template<class T>
			inline MemoryManager<T> & MemoryManager<T>::operator=(MemoryManager &&lhs)
			{
				if (m_data) {
					if (!Template::IsValueType<T>::R)
						Deconstruct(m_data, m_used);
					Allocator::Free(m_data);
				}
				this->m_data = lhs.m_data;
				lhs.m_data = nullptr;
				this->m_used = lhs.m_used;
				this->m_max = lhs.m_max;
				this->m_rank = lhs.m_rank;
				return *this;
			}

			template<class T>
			inline MemoryManager<T>::~MemoryManager()
			{
				this->Clean();
			}
		}
	}
}