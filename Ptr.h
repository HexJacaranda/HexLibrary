#pragma once
namespace HL
{
	namespace System
	{
		namespace Reference
		{
			namespace InternalAdopt
			{
				class reference_resource_base:public System::UPointer::uptr_resource
				{
				public:
					Threading::AtomicCounter reference_counter;
					virtual void release() = 0;
					virtual System::UPointer::uptr_resource* clone()const = 0;
				};

				template<class T>
				class reference_resource :public reference_resource_base
				{
				public:
					virtual void release()final {
						T*object_ptr = (T*)this->object;
						if (object_ptr)
						{
							delete object_ptr;
							object_ptr = nullptr;
						}
					}
					virtual System::UPointer::uptr_resource* clone()const final{
						T*object_ptr = (T*)this->object;
						reference_resource<T> *ret = new reference_resource<T>();
						ret->reference_counter = 1;
						ret->object = Interface::ICloneable::GetClonePtr<T>(*object_ptr);
						return ret;
					}
				};

				class reference_keeper:public System::UPointer::uptr_resource_keeper
				{
				public:
					reference_keeper() {
						this->keeper_type_id = System::UPointer::resource_keeper_type_id<reference_keeper>();
					}
					virtual void release_resource(System::UPointer::uptr_resource*ptr)final {
						if (ptr) {
							reference_resource_base*objptr = static_cast<reference_resource_base*>(ptr);
							if (--objptr->reference_counter == 0)
								if (ptr)
								{
									ptr->release();
									ptr->self_release = true;
								}
						}
					}
					virtual System::UPointer::uptr_resource*update(System::UPointer::uptr_resource* ptr,Reference::IntPtr)final {
						if (ptr)
						{
							reference_resource_base*objptr = static_cast<reference_resource_base*>(ptr);
							++objptr->reference_counter;
						}
						return ptr;
					}
					virtual bool is_invalid(System::UPointer::uptr_resource*ptr)const final {
						if (ptr)
						{
							reference_resource_base*objptr = static_cast<reference_resource_base*>(ptr);
							return objptr->reference_counter == 0;
						}
						return true;
					}
				};

				class native_reference_resource :public System::UPointer::uptr_resource
				{
					virtual void release() final{

					}
					virtual System::UPointer::uptr_resource* clone()const final
					{
						HL::Exception::Throw<HL::Exception::InterfaceNoImplementException>();
						return nullptr;
					}
				};

				class native_reference_keeper :public System::UPointer::uptr_resource_keeper
				{
				public:
					native_reference_keeper() {
						this->keeper_type_id = System::UPointer::resource_keeper_type_id<native_reference_keeper>();
					}
					virtual void release_resource(System::UPointer::uptr_resource*ptr)final {
					}
					virtual System::UPointer::uptr_resource*update(System::UPointer::uptr_resource* ptr, Reference::IntPtr)final {
						return ptr;
					}
					virtual bool is_invalid(System::UPointer::uptr_resource*ptr)const final {
						return true;
					}
				};

				template<class T>
				class unique_resource:public System::UPointer::uptr_resource
				{
				public:
					virtual void release() final {
						T*object_ptr = (T*)this->object;
						if (object_ptr)
						{
							delete object_ptr;
							object_ptr = nullptr;
						}
					}
					virtual System::UPointer::uptr_resource* clone()const final {
						T*object_ptr = (T*)this->object;
						unique_resource<T>*ret = new unique_resource<T>();
						ret->object = Interface::ICloneable::GetClonePtr(*object_ptr);
						return ret;
					}
				};
				
				class unique_keeper:public System::UPointer::uptr_resource_keeper
				{
				public:
					unique_keeper() {
						this->keeper_type_id = System::UPointer::resource_keeper_type_id<unique_keeper>();
					}
					virtual void release_resource(System::UPointer::uptr_resource*ptr)final {
						if (ptr)
						{
							ptr->release();
							ptr->self_release = true;
						}
					}
					virtual System::UPointer::uptr_resource*update(System::UPointer::uptr_resource* ptr, Reference::IntPtr ptr_to_ptr) final {
						if (ptr)
						{
							System::UPointer::uptr_resource**prev_ptr = ptr_to_ptr;
							*prev_ptr = nullptr;
						}
						return ptr;
					}
					virtual bool is_invalid(System::UPointer::uptr_resource*ptr)const final {
						return true;//unique不允许weakptr
					}
				};
			}

			template<class U,class...Args>
			System::UPointer::uptr<U> newptr(Args &&...args) {
				InternalAdopt::reference_keeper*ptr = System::UPointer::keeper_interface<InternalAdopt::reference_keeper>();
				InternalAdopt::reference_resource<U>*resource_ptr = new InternalAdopt::reference_resource<U>;
				U*object = new U(args...);
				resource_ptr->object = object;
				resource_ptr->reference_counter = 1;
				return System::UPointer::uptr<U>(object, resource_ptr, ptr);
			}

			template<class U, class...Args>
			System::UPointer::uptr<U> new_unique(Args &&...args) {
				InternalAdopt::unique_keeper*ptr = System::UPointer::keeper_interface<InternalAdopt::unique_keeper>();
				InternalAdopt::unique_resource<U>*resource_ptr = new InternalAdopt::unique_resource<U>;
				U*object = new U(args...);
				resource_ptr->object = object;
				return System::UPointer::uptr<U>(object, resource_ptr, ptr);
			}

			template<class U>
			System::UPointer::uptr<U> ref(U&object) {
				InternalAdopt::native_reference_keeper*ptr = System::UPointer::keeper_interface<InternalAdopt::native_reference_keeper>();
				InternalAdopt::native_reference_resource *resource_ptr = new InternalAdopt::native_reference_resource;
				return System::UPointer::uptr<U>(&object, resource_ptr, ptr);
			}
			//针对ref-uptr的类型
			inline static atomic_type ref_type_id() {
				return System::UPointer::resource_keeper_type_id<InternalAdopt::reference_keeper>();
			}
			template<class T>
			class Ref
			{
				T*m_object = nullptr;
			public:
				Ref(T&object) :m_object(&object) {}
				Ref(Ref const&rhs) :m_object(rhs.m_object) {}
				bool IsValid()const {
					return this->m_object != nullptr;
				}
				Ref& operator=(T&object) {
					this->m_object = &object;
					return *this;
				}
				Ref& operator=(Ref const&rhs) {
					this->m_object = rhs.m_object;
					return *this;
				}
				operator T&() {
					return *this->m_object;
				}
				operator T const&()const {
					return *this->m_object;
				}
			};
		}
	}
	namespace Exception
	{
		template<class T>
		class ExceptionHandle
		{
			System::UPointer::uptr<T> exception_ptr;	   
			template<class AnyT>
			friend class ExceptionHandle;
		public:
			ExceptionHandle(nullptr_t) {}
			ExceptionHandle() {}
			ExceptionHandle(ExceptionHandle const&) = default;
			template<class AnyT, class...Args>
			ExceptionHandle(AnyT*, Args&&...args)throw() {
				if (Template::IsBaseOf<IException, T>::R)
					exception_ptr = Reference::newptr<AnyT>(args...);
				else
					return;
			}
			System::UPointer::uptr<T>& operator->() {
				return exception_ptr;
			}
			template<class AnyT>
			operator ExceptionHandle<AnyT>()const {
				if (!Template::IsBaseOf<IException, AnyT>::R)
					return nullptr;
				ExceptionHandle<AnyT> ret;
				ret.exception_ptr = this->exception_ptr;
				return ret;
			}
			ExceptionHandle&operator=(ExceptionHandle const&) = default;
		};
		template<>
		class ExceptionHandle<void>
		{
			template<class AnyT>
			friend class ExceptionHandle;
			System::UPointer::uptr<HL::Exception::IException> exception_ptr;
		public:
			ExceptionHandle(nullptr_t){}
			ExceptionHandle() {}
			ExceptionHandle(ExceptionHandle const&) = default;
			template<class T, class...Args>
			ExceptionHandle(T*, Args&&...args)throw() {
				exception_ptr = System::Reference::newptr<T>(args...);
			}
			System::UPointer::uptr<HL::Exception::IException>& operator->() {
				return exception_ptr;
			}
			template<class AnyT>
			operator ExceptionHandle<AnyT>()const{
				if (!Template::IsBaseOf<IException, AnyT>::R)
					return nullptr;
				ExceptionHandle<AnyT> ret;
				ret.exception_ptr = const_cast<ExceptionHandle<void>*>(this)->exception_ptr.Cast<AnyT>([](HL::Exception::IException *ptr) {
					return dynamic_cast<AnyT*>(ptr);
				});
				return ret;
			}
			ExceptionHandle&operator=(ExceptionHandle const&) = default;
		};

		typedef ExceptionHandle<void> Exception;


		template<class T, class ...Args>
		void Throw(Args&&...args)
		{
			throw ExceptionHandle<void>((T*)(nullptr), args...);
		}
	}
}