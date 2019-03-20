#pragma once
namespace HL
{
	namespace System
	{
		namespace Nullable
		{
			template<class U>
			class Nullable
			{
				template<class O>friend class Nullable;
				U* m_object_ptr = nullptr;
				bool m_is_null = true;
				void SetNull() {
					if (m_object_ptr != nullptr && !m_is_null)
						Memory::Destruct(m_object_ptr);
					m_is_null = true;
				}
				template<class...Args>
				void ConstructValue(Args&&...values) {
					m_object_ptr = static_cast<U*>(Memory::DefaultAllocator::Instance()->Alloc(sizeof(U)));
					Memory::ZeroMemory(m_object_ptr);
					Memory::PlacementNew(m_object_ptr, std::forward<Args>(values)...);
					m_is_null = false;
				}
				template<class T>
				void Assign(T&&value) {
					if (m_object_ptr == nullptr || m_is_null)
					{
						m_object_ptr = static_cast<U*>(Memory::DefaultAllocator::Instance()->Alloc(sizeof(U)));
						Memory::ZeroMemory(m_object_ptr);
						DefaultConstruct(m_object_ptr);
					}
					*m_object_ptr = std::forward<T>(value);
					m_is_null = false;
				}
				template<typename=std::enable_if_t<std::is_default_constructible_v<U>,int>>
				static inline void DefaultConstruct(U*object) noexcept(std::is_nothrow_default_constructible_v<U>) {
					Memory::PlacementNew(object);
				}
				static inline void DefaultConstruct(U*object) noexcept {}
			public:
				template<typename = std::enable_if_t<std::is_move_constructible_v<U>,int>>
				Nullable(U&&object) noexcept(std::is_nothrow_move_constructible_v<U>) {
					ConstructValue(std::move(object));
				}
				template<typename = std::enable_if_t<std::is_copy_constructible_v<U>,int>>
				Nullable(U const& object) noexcept(std::is_nothrow_copy_constructible_v<U>) {
					ConstructValue(object);
				}
				template<class...Args, typename = std::enable_if_t<std::is_constructible_v<U,Args...>,int>>
				Nullable(Args&&...args) noexcept(std::is_nothrow_constructible_v<U, Args...>) {
					ConstructValue(std::forward<Args>(args)...);
				}
				template<class O, typename = std::enable_if_t<std::is_constructible_v<U,O>,int>>
				Nullable(Nullable<O> const&rhs) noexcept(std::is_nothrow_constructible_v<U, O>) {
					if (!rhs.m_is_null)
						ConstructValue(*rhs.m_object_ptr);
				}
				Nullable(Nullable const&rhs) {
					if (!rhs.m_is_null)
						ConstructValue(*rhs.m_object_ptr);
				}
				Nullable(Nullable &&lhs) {
					this->m_is_null = lhs.m_is_null;
					this->m_object_ptr = lhs.m_object_ptr;
					lhs.m_is_null = true;
					lhs.m_object_ptr = nullptr;
				}
				Nullable() {}
				Nullable(nullptr_t) {}
				inline U& GetObject()noexcept {
					return *this->m_object_ptr;
				}
				inline U const& GetObject()const noexcept {
					return *this->m_object_ptr;
				}
				inline bool IsNull()const noexcept {
					return m_is_null;
				}
				template<class O ,std::enable_if_t<std::is_assignable_v<U, O>>>
				Nullable& operator=(Nullable<O> const&other) noexcept(std::is_nothrow_assignable_v<U,O>) {
					if (other.IsNull())
					{
						SetNull();
						return *this;
					}
					Assign(other.GetObject());
					return *this;
				}
				Nullable& operator=(Nullable const&other) {
					if (other.IsNull())
					{
						SetNull();
						return *this;
					}
					Assign(other.GetObject());
					return *this;
				}
				Nullable& operator=(Nullable &&lhs)noexcept {
					SetNull();
					this->m_is_null = lhs.m_is_null;
					this->m_object_ptr = lhs.m_object_ptr;
					lhs.m_is_null = true;
					lhs.m_object_ptr = nullptr;
					return *this;
				}
				template<class O,typename = std::enable_if_t<std::is_assignable_v<U,O>>>
				Nullable& operator=(O&&object) noexcept(std::is_nothrow_assignable_v<U, O>) {
					Assign(std::forward<O>(object));
					return *this;
				}
				Nullable& operator=(nullptr_t) {
					SetNull();
					return *this;
				}
				~Nullable(){
					if (m_object_ptr != nullptr)
					{
						Memory::Destruct(m_object_ptr);
						Memory::DefaultAllocator::Instance()->Collect(m_object_ptr);
						m_object_ptr = nullptr;
					}
				}
			};
		}
	}
}