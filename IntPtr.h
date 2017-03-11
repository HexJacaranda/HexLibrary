#pragma once
namespace HL
{
	namespace System
	{
		namespace Reference
		{
			template<class Out,class In>
			Out UnionCast(In object) {
				union
				{
					In in;
					Out out;
				}Caster;
				Caster.in = object;
				return Caster.out;
			}
			class IntPtr
			{
				void* m_ptr = nullptr;
			public:
#if defined(_WIN64)
				IntPtr(__int64 value) {

				}
#else
				IntPtr(int value) {
					m_ptr = reinterpret_cast<void*>(value);
				}
#endif 
				IntPtr();
				IntPtr(void*);
				IntPtr(IntPtr const&);
				template<class T>
				IntPtr(T*ptr) {
					m_ptr = UnionCast<void*>(ptr);
				}
				template<class T>
				operator T*() {
					return (T*)this->m_ptr;
				}
				template<class T>
				operator T*()const {
					return (T*)this->m_ptr;
				}
				bool operator==(void*)const;
				bool operator==(IntPtr const&)const;
				bool operator!=(void*)const;
				bool operator!=(IntPtr const&)const;
				~IntPtr();
			};

			inline IntPtr::IntPtr() {}
			inline IntPtr::IntPtr(void*ptr) :m_ptr(ptr) {}
			inline IntPtr::IntPtr(IntPtr const&rhs) : m_ptr(rhs.m_ptr) {}

			inline bool IntPtr::operator==(void*ptr)const {
				return this->m_ptr == ptr;
			}
			inline bool IntPtr::operator==(IntPtr const&rhs)const {
				return this->m_ptr == rhs.m_ptr;
			}
			inline bool IntPtr::operator!=(void*ptr)const {
				return this->m_ptr != ptr;
			}
			inline bool IntPtr::operator!=(IntPtr const&rhs)const {
				return this->m_ptr != rhs.m_ptr;
			}
			inline IntPtr::~IntPtr() {}
		}
	}
}