#pragma once
namespace HL
{
	namespace System
	{
		namespace UPointer
		{
			namespace Internal
			{
				static atomic_type InternalKeeperTypeID() {
					static Threading::AtomicCounter id = 1;
					return ++id;
				}
				template<class T, bool based>
				struct set_uptr_this
				{
					static void set_this(T*, uptr<T> const&);
				};
				template<class T>
				struct set_uptr_this<T, true>
				{
					static void set_this(T*, uptr<T> const&);
				};



				template<class U,bool IsUPtr>
				struct object_converter
				{
					static U Convert(uobject const&);
				};
				template<class U>
				struct object_converter<U, true>
				{
					static U Convert(uobject const&);
				};

			}
			
			//resource_keeper标识符
			template<class T>
			static atomic_type resource_keeper_type_id() {
				static atomic_type id = Internal::InternalKeeperTypeID();
				return id;
			}

			template<class T>
			static T* keeper_interface() {
				static T keeper;
				return &keeper;
			}

			//智能指针资源接口
			class uptr_resource
			{	
			public:
				//释放资源接口
				//[注意]uptr不会使用该接口,仅仅只会将该指针传入keeper->release_resource内部处理
				virtual void release() = 0;

				//返回对象clone
				virtual uptr_resource* clone()const = 0;

				bool self_release = false; //通知uptr该资源指针是否需要被释放

				void* object = nullptr;

				Threading::AtomicCounter weak_reference_count;//弱引用计数
			};

			//智能指针资源管理接口
			class uptr_resource_keeper
			{
				template<class U>friend class uptr;
				template<class U>friend class weakuptr;
				friend class uobject;
			public:
				enum {
					unknown_type = 0x01
				};
				//keeper类型(0x01为Unknown)
				atomic_type keeper_type_id = unknown_type;
				//释放资源接口
				virtual void release_resource(uptr_resource*) = 0;
				//更新
				virtual uptr_resource* update(uptr_resource*, Reference::IntPtr) = 0;
				//查询资源是否有效
				virtual bool is_invalid(uptr_resource*)const = 0;

				virtual ~uptr_resource_keeper() {}
			};

			//智能指针统一化类型
			template<class T>
			class uptr:public System::Hash::ISupportHash
			{
				template<class U>friend class uptr;
				template<class U>friend class weakuptr;
				friend class uobject;
			private:
				uptr_resource_keeper*resource_keeper = nullptr;
				uptr_resource*resource = nullptr;
				T*m_ptr = nullptr;
				//限制类型,-1表不限定
				atomic_type limited_type_id = -1;
			public:
				uptr();
				uptr(nullptr_t);
				uptr(T*, uptr_resource*, uptr_resource_keeper*);
				uptr(uptr const&);
				uptr(uptr &&)noexcept;
				uptr&operator=(uptr const&);
				uptr&operator=(uptr &&);
				uptr&operator=(nullptr_t);
				T*operator->();
				T const*operator->()const;
				T const* GetObjectPtr()const;
				T* GetObjectPtr();
				//将当前指针置空
				void SetToNull();
				//检查当前指针是否有效
				bool IsNull()const;
				//设置当前智能指针允许的托管资源类型(传入-1表不限定)
				void SetLimitativeType(atomic_type);
				//获得当前智能指针的限制类型
				atomic_type GetLimitativeType()const;
				//获得当前智能指针托管资源的类型
				atomic_type GetResourceType()const;
				//获得对象Hash码(如果支持,不支持则为地址Hash)
				virtual size_t GetHashCode()const;
				//获得对象Clone
				uptr<T> Clone()const;
				template<class U, class Converter>
				uptr<U> Cast(Converter);
				template<class U>
				void SetValue(uptr<U> const&);
				~uptr();
			private: //特性行为拓展
				typedef typename Interface::IndexerSupportInterface<T>::IndexType IndexType;
				typedef typename Interface::IndexerSupportInterface<T>::ReturnType IndexRetType;
				typedef typename Interface::FunctorSupportInterface<T>::ReturnType FunctorRetType;
				typedef typename Interface::EnumerableSupportInterface<T>::IteratorType IterType;
				typedef typename Interface::EnumerableSupportInterface<T>::ConstIteratorType ConstIterType;
				typedef typename Interface::IteratorSupportInterface<T>::UnReferenceType UnRefType;
			public:
				inline IndexRetType& operator[](IndexType const&index) {
					return this->m_ptr->operator[](index);
				}
				inline IndexRetType const& operator[](IndexType const&index)const {
					return this->m_ptr->operator[](index);
				}
				template<class...Args>
				inline FunctorRetType operator()(Args const&...args)const {
					return this->m_ptr->operator()(const_cast<Args&>(args)...);
				}
				inline ConstIterType begin()const {
					return this->m_ptr->begin();
				}
				inline IterType begin() {
					return this->m_ptr->begin();
				}
				inline ConstIterType end()const {
					return this->m_ptr->end();
				}
				inline IterType end() {
					return this->m_ptr->end();
				}
				inline uptr& operator++() {
					(*(this->m_ptr))++;
					return *this;
				}
				inline uptr& operator--() {
					(*(this->m_ptr))--;
					return *this;
				}
				inline uptr& operator--(int) {
					(*(this->m_ptr))--;
					return *this;
				}
				inline uptr& operator++(int) {
					(*(this->m_ptr))++;
					return *this;
				}
				inline UnRefType& operator*() {
					return *(*this->m_ptr);
				}
				inline UnRefType const& operator*()const {
					return *(*this->m_ptr);
				}
				inline bool operator==(uptr const&rhs)const {
					return *this->m_ptr == *rhs.m_ptr;
				}
				inline bool operator!=(uptr const&rhs)const {
					return *this->m_ptr != *rhs.m_ptr;
				}
			public:
				operator uobject()const;
				template<class AnyT>
				operator uptr<AnyT>()const;

				operator T const&()const;
				operator T&();
			};

			//智能指针封箱统一化类型
			class uobject:System::Hash::ISupportHash
			{
				template<class U>friend class uptr;
				template<class U>friend class weakuptr;
				template<class U, bool T>friend struct Internal::object_converter;
			private:
				uptr_resource_keeper*resource_keeper = nullptr;
				uptr_resource*resource = nullptr;
				void*m_ptr = nullptr;
				//限制类型,-1表不限定
				atomic_type limited_type_id = -1;
			public:
				uobject();
				uobject(nullptr_t);
				uobject(uobject const&);
				uobject(uobject &&)noexcept;
				template<class U>
				uobject(uptr<U> const&);
				template<class U>
				uobject(uptr<U> &&);
				template<class T>
				uobject(T*, uptr_resource*, uptr_resource_keeper*);
				uobject&operator=(uobject const&);
				uobject&operator=(uobject &&);
				template<class U>
				uobject&operator=(uptr<U> const&);
				template<class U>
				uobject&operator=(uptr<U> &&);
				uobject&operator=(nullptr_t);
				void SetToNull();
				bool IsNull()const;
				//设置当前智能指针允许的托管资源类型(传入-1表不限定)
				void SetLimitativeType(atomic_type);
				//获得当前智能指针的限制类型
				atomic_type GetLimitativeType()const;
				//获得当前智能指针托管资源的类型
				atomic_type GetResourceType()const;
				//获得对象Hash码(总是以地址算)
				virtual size_t GetHashCode() const;
				//获得对象副本
				uobject Clone()const;
				template<class T>
				T * To();
				template<class T>
				T const * To() const;
				~uobject();
			public:
				template<class U>
				operator U()const;
			private:
				template<class U>
				uptr<U> ToUPtr()const;
			};

			//弱智能指针
			template<class T>
			class weakuptr
			{
				uptr_resource_keeper*resource_keeper = nullptr;
				uptr_resource*resource = nullptr;
				T*m_ptr = nullptr;
				//限制类型,-1表不限定
				atomic_type limited_type_id = -1;
			public:
				weakuptr();
				weakuptr(nullptr_t);
				weakuptr(uptr<T> const&);
				weakuptr(weakuptr const&);
				weakuptr(weakuptr&&)noexcept;
				weakuptr&operator=(nullptr_t);
				weakuptr&operator=(uptr<T> const&);
				weakuptr&operator=(weakuptr const&);
				weakuptr&operator=(weakuptr&&);
				T* GetObjectPtr();
				T const* GetObjectPtr()const;
				//检查指针持有的资源是否无效
				bool IsInvalid()const;
				void SetToNull();
				//设置当前智能指针允许的托管资源类型(传入-1表不限定)
				void SetLimitativeType(atomic_type);
				//获得当前智能指针的限制类型
				atomic_type GetLimitativeType()const;
				//获得当前智能指针托管资源的类型
				atomic_type GetResourceType()const;
				size_t GetHashCode()const;
				//转换为标准强引用智能指针
				uptr<T> ToUPtr()const;
				~weakuptr();
			};

			//从内部获取uptr
			template<class T>
			class UPtrThis
			{
				template<class U,bool base>
				friend struct Internal::set_uptr_this;
				weakuptr<T> uptr_this;
			public:
				uptr<T> GetThisUPtr()const;
			};

			//uptr实现

			template<class T>
			inline uptr<T>::uptr() {}

			template<class T>
			inline uptr<T>::uptr(nullptr_t) {}

			template<class T>
			inline uptr<T>::uptr(T *object_ptr, uptr_resource* resource_ptr, uptr_resource_keeper *keeper)
			{
				if (object_ptr&&keeper)
				{
					m_ptr = object_ptr;
					resource_keeper = keeper;
					resource = resource_ptr;
					Internal::set_uptr_this<T, Template::IsBaseOf<UPtrThis<T>, T>::R>::set_this(object_ptr, *this);
				}
			}

			template<class T>
			inline uptr<T>::uptr(uptr const &rhs)
			{
				if (rhs.resource_keeper&&rhs.resource)
				{
					this->resource_keeper = rhs.resource_keeper;
					this->m_ptr = rhs.m_ptr;
					this->resource = rhs.resource_keeper->update(rhs.resource, &rhs.resource);
				}
			}

			template<class T>
			inline uptr<T>::uptr(uptr && lhs)noexcept
			{
				this->m_ptr = lhs.m_ptr;
				this->resource_keeper = lhs.resource_keeper;
				this->resource = lhs.resource;
				lhs.m_ptr = nullptr;
				lhs.resource_keeper = nullptr;
				lhs.resource = nullptr;
			}

			template<class T>
			inline uptr<T> & uptr<T>::operator=(uptr const &rhs)
			{
				if (rhs.m_ptr == this->m_ptr)
					return *this;
				if (this->limited_type_id != -1)//如果是限定类型
					if (this->limited_type_id != rhs.limited_type_id)//如果限定的类型对不上
						return *this;//返回自己
				this->SetToNull();
				if (rhs.resource_keeper&&rhs.resource)
				{
					this->resource_keeper = rhs.resource_keeper;
					this->m_ptr = rhs.m_ptr;
					this->resource = rhs.resource_keeper->update(rhs.resource, &rhs.resource);
				}
				return *this;
			}

			template<class T>
			inline uptr<T> & uptr<T>::operator=(uptr &&lhs)
			{
				if (this->limited_type_id != -1)//如果是限定类型
					if (this->limited_type_id != lhs.limited_type_id)//如果限定的类型对不上
					{
						lhs.SetToNull();//将move对象置空
						return *this;
					}
				if (lhs.m_ptr != this->m_ptr)
				{
					this->SetToNull();
					this->m_ptr = lhs.m_ptr;
					this->resource_keeper = lhs.resource_keeper;
					this->resource = lhs.resource;
				}
				lhs.m_ptr = nullptr;
				lhs.resource_keeper = nullptr;
				lhs.resource = nullptr;
				return *this;
			}

			template<class T>
			inline uptr<T> & uptr<T>::operator=(nullptr_t)
			{
				this->SetToNull();
				return *this;
			}

			template<class T>
			inline T * uptr<T>::operator->()
			{
				return this->m_ptr;
			}

			template<class T>
			inline T const * uptr<T>::operator->() const
			{
				return this->m_ptr;
			}

			template<class T>
			inline T const * uptr<T>::GetObjectPtr() const
			{
				return this->m_ptr;
			}

			template<class T>
			inline T * uptr<T>::GetObjectPtr()
			{
				return this->m_ptr;
			}

			template<class T>
			inline void uptr<T>::SetToNull()
			{
				if (this->resource_keeper&&this->resource)
				{
					resource_keeper->release_resource(this->resource);
					/*判断弱引用*/
					if (resource->self_release&&resource->weak_reference_count == 0)
						delete resource;
					resource_keeper = nullptr;
					resource = nullptr;
				}
				m_ptr = nullptr;
			}

			template<class T>
			inline bool uptr<T>::IsNull() const
			{
				return (this->resource == nullptr || this->resource_keeper == nullptr);
			}

			template<class T>
			inline void uptr<T>::SetLimitativeType(atomic_type type)
			{
				if (type >= -1)
					this->limited_type_id = type;
			}

			template<class T>
			inline atomic_type uptr<T>::GetLimitativeType() const
			{
				return this->limited_type_id;
			}

			template<class T>
			inline atomic_type uptr<T>::GetResourceType() const
			{
				if (this->resource_keeper)
					return this->resource_keeper->keeper_type_id;
				else
					return uptr_resource_keeper::unknown_type;
			}

			template<class T>
			inline size_t uptr<T>::GetHashCode() const
			{
				if (Template::IsBaseOf<System::Hash::ISupportHash, T>::R)
					return ((System::Hash::ISupportHash*)this->m_ptr)->GetHashCode();
				atomic_type ptr = (atomic_type)this->m_ptr;
				return Hash::Hash::HashSeq(&ptr);
			}

			template<class T>
			inline uptr<T> uptr<T>::Clone() const
			{
				uptr_resource*clone = this->resource->clone();
				return uptr<T>((T*)clone->object, clone, this->resource_keeper);
			}

			template<class T>
			inline uptr<T>::~uptr()
			{
				this->SetToNull();
			}

			template<class T>
			inline uptr<T>::operator uobject() const
			{
				uobject ret = nullptr;
				if (!this->IsNull())
				{
					ret.resource_keeper = this->resource_keeper;
					ret.m_ptr = this->m_ptr;
					ret.resource = this->resource_keeper->update(this->resource, &this->resource);
				}
				return ret;
			}

			template<class T>
			inline uptr<T>::operator T const&() const
			{
				return *(this->m_ptr);
			}

			template<class T>
			inline uptr<T>::operator T&()
			{
				return *(this->m_ptr);
			}

			//封箱类型

			inline uobject::uobject()
			{
			}

			inline uobject::uobject(nullptr_t)
			{
			}

			inline uobject::uobject(uobject const &rhs)
			{
				if (rhs.resource_keeper&&rhs.resource)
				{
					this->resource_keeper = rhs.resource_keeper;
					this->m_ptr = rhs.m_ptr;
					this->resource = rhs.resource_keeper->update(rhs.resource, &rhs.resource);
				}
			}

			inline uobject::uobject(uobject &&lhs)noexcept
			{
				this->resource = lhs.resource;
				this->m_ptr = lhs.m_ptr;
				this->resource_keeper = lhs.resource_keeper;
				lhs.m_ptr = nullptr;
				lhs.resource_keeper = nullptr;
				lhs.resource = nullptr;
			}

			inline uobject & uobject::operator=(uobject const &rhs)
			{
				if (rhs.m_ptr == this->m_ptr)
					return *this;
				if (this->limited_type_id != -1)//如果是限定类型
					if (this->limited_type_id != rhs.limited_type_id)//如果限定的类型对不上
						return *this;//返回自己
				this->SetToNull();
				if (rhs.resource_keeper&&rhs.resource)
				{
					this->resource_keeper = rhs.resource_keeper;
					this->m_ptr = rhs.m_ptr;
					this->resource = rhs.resource_keeper->update(rhs.resource, &rhs.resource);
				}
				return *this;
			}

			inline uobject & uobject::operator=(uobject && lhs)
			{
				if (this->limited_type_id != -1)//如果是限定类型
					if (this->limited_type_id != lhs.limited_type_id)//如果限定的类型对不上
					{
						lhs.SetToNull();//将move对象置空
						return *this;
					}
				if (lhs.m_ptr != this->m_ptr)
				{
					this->SetToNull();
					this->m_ptr = lhs.m_ptr;
					this->resource_keeper = lhs.resource_keeper;
					this->resource = lhs.resource;
				}
				lhs.m_ptr = nullptr;
				lhs.resource_keeper = nullptr;
				lhs.resource = nullptr;
				return *this;
			}

			inline uobject & uobject::operator=(nullptr_t)
			{
				this->SetToNull();
				return *this;
			}

			inline void uobject::SetToNull()
			{
				if (this->resource_keeper&&this->resource)
				{
					resource_keeper->release_resource(this->resource);
					/*判断弱引用*/
					if (resource->self_release&&resource->weak_reference_count == 0)
						delete resource;
					resource_keeper = nullptr;
					resource = nullptr;
				}
				m_ptr = nullptr;
			}

			inline bool uobject::IsNull() const
			{
				return (this->resource == nullptr || this->resource_keeper == nullptr);
			}

			inline void uobject::SetLimitativeType(atomic_type type)
			{
				if (type >= -1)
					this->limited_type_id = type;
			}

			inline atomic_type uobject::GetLimitativeType() const
			{
				return this->limited_type_id;
			}

			inline atomic_type uobject::GetResourceType() const
			{
				if (this->resource_keeper)
					return this->resource_keeper->keeper_type_id;
				else
					return uptr_resource_keeper::unknown_type;
			}

			inline size_t uobject::GetHashCode() const
			{
				atomic_type ptr = (atomic_type)this->m_ptr;
				return Hash::Hash::HashSeq(&ptr);
			}

			inline uobject uobject::Clone() const
			{
				uptr_resource*clone = nullptr;
				if (this->resource)
					clone = this->resource->clone();
				else
					return nullptr;
				uobject ret;
				ret.resource = clone;
				ret.resource_keeper = this->resource_keeper;
				ret.m_ptr = clone->object;
				return ret;
			}

			inline uobject::~uobject()
			{
				this->SetToNull();
			}

			template<class U>
			inline uobject::uobject(uptr<U> const & rhs)
			{
				if (rhs.resource_keeper&&rhs.resource)
				{
					this->resource_keeper = rhs.resource_keeper;
					this->m_ptr = rhs.m_ptr;
					this->resource = rhs.resource_keeper->update(rhs.resource, &rhs.resource);
				}
			}

			template<class U>
			inline uobject::uobject(uptr<U>&& lhs)
			{
				this->resource = lhs.resource;
				this->m_ptr = lhs.m_ptr;
				this->resource_keeper = lhs.resource_keeper;
				lhs.m_ptr = nullptr;
				lhs.resource_keeper = nullptr;
				lhs.resource = nullptr;
			}

			template<class T>
			inline uobject::uobject(T *object_ptr, uptr_resource* resource_ptr, uptr_resource_keeper *keeper)
			{
				if (object_ptr&&keeper)
				{
					resource = resource_ptr;
					m_ptr = object_ptr;
					resource_keeper = keeper;
					Internal::set_uptr_this<T, Template::IsBaseOf<UPtrThis<T>, T>::R>::set_this(object_ptr, *this);
				}
			}

			template<class U>
			inline uobject & uobject::operator=(uptr<U> const &rhs)
			{
				if (rhs.m_ptr == this->m_ptr)
					return *this;
				if (this->limited_type_id != -1)//如果是限定类型
					if (this->limited_type_id != rhs.limited_type_id)//如果限定的类型对不上
						return *this;//返回自己
				this->SetToNull();
				if (rhs.resource_keeper&&rhs.resource)
				{
					this->resource_keeper = rhs.resource_keeper;
					this->m_ptr = rhs.m_ptr;
					this->resource = rhs.resource_keeper->update(rhs.resource, &rhs.resource);
				}
				return *this;
			}

			template<class U>
			inline uobject & uobject::operator=(uptr<U>&&lhs)
			{
				if (this->limited_type_id != -1)//如果是限定类型
					if (this->limited_type_id != lhs.limited_type_id)//如果限定的类型对不上
					{
						lhs.SetToNull();//将move对象置空
						return *this;
					}
				if (lhs.m_ptr != this->m_ptr)
				{
					this->SetToNull();
					this->m_ptr = lhs.m_ptr;
					this->resource_keeper = lhs.resource_keeper;
					this->resource = lhs.resource;
				}
				lhs.m_ptr = nullptr;
				lhs.resource_keeper = nullptr;
				lhs.resource = nullptr;
				return *this;
			}

			template<class T>
			inline T * uobject::To()
			{
				return static_cast<T*>(m_ptr);
			}

			template<class T>
			inline T const * uobject::To() const
			{
				return static_cast<T const*>(m_ptr);
				
			}

			template<class U>
			inline uobject::operator U() const
			{
				return Internal::object_converter<U, IsUPtr<U>::R>::Convert(*this);
			}

			template<class U>
			inline uptr<U> uobject::ToUPtr() const
			{
				uptr<U> ret = nullptr;
				if (!this->IsNull())
				{
					ret.resource_keeper = this->resource_keeper;
					ret.resource = this->resource_keeper->update(this->resource, &this->resource);
					ret.m_ptr = static_cast<U*>(this->m_ptr);
				}
				return ret;
			}

			template<class T>
			template<class U, class Converter>
			inline uptr<U> uptr<T>::Cast(Converter converter)
			{
				uptr<U> ret = nullptr;
				if (!this->IsNull())
				{
					U*ptr = converter(this->m_ptr);
					if (ptr)
					{
						ret.resource_keeper = this->resource_keeper;
						ret.m_ptr = ptr;
						ret.resource = this->resource_keeper->update(this->resource, &this->resource);
					}
				}
				return ret;
			}

			template<class T>
			template<class U>
			inline void uptr<T>::SetValue(uptr<U> const &rhs)
			{
				*this->m_ptr = *rhs.m_ptr;
			}

			template<class T>
			template<class AnyT>
			inline uptr<T>::operator uptr<AnyT>() const
			{
				uptr<AnyT> ret = nullptr;
				if (!this->IsNull())
				{
					AnyT * ptr = static_cast<AnyT*>(const_cast<T*>(this->m_ptr));
					if (ptr)
					{
						ret.resource_keeper = this->resource_keeper;
						ret.m_ptr = ptr;
						ret.resource = this->resource_keeper->update(this->resource, &this->resource);
					}
				}
				return ret;
			}

			//弱引用指针

			template<class T>
			inline bool weakuptr<T>::IsInvalid() const
			{
				if (this->resource&&this->resource_keeper)
					if (this->resource_keeper->is_invalid(this->resource))
						return true;
					else
						return false;
				else
					return true;
			}
			template<class T>
			inline void weakuptr<T>::SetToNull()
			{
				if (this->resource_keeper&&this->resource)
				{
					--this->resource->weak_reference_count;
					//判定弱引用是否为0,再释放
					if (this->resource->weak_reference_count == 0 && this->resource_keeper->is_invalid(this->resource))
						delete this->resource;
					this->resource = nullptr;
					this->resource_keeper = nullptr;
					this->m_ptr = nullptr;
				}
			}
			template<class T>
			inline void weakuptr<T>::SetLimitativeType(atomic_type type)
			{
				if (type >= -1)
					this->limited_type_id = type;
			}
			template<class T>
			inline atomic_type weakuptr<T>::GetLimitativeType() const
			{
				return this->limited_type_id;
			}
			template<class T>
			inline atomic_type weakuptr<T>::GetResourceType() const
			{
				if (this->resource_keeper)
					return this->resource_keeper->keeper_type_id;
				else
					return uptr_resource_keeper::unknown_type;
			}
			template<class T>
			inline size_t weakuptr<T>::GetHashCode() const
			{
				atomic_type ptr = (atomic_type)this->m_ptr;
				return Hash::Hash::HashSeq(&ptr);
			}
			template<class T>
			inline uptr<T> weakuptr<T>::ToUPtr() const
			{
				if (this->IsInvalid())
					return nullptr;
				uptr<T> ret = nullptr;
				ret.resource_keeper = this->resource_keeper;
				ret.m_ptr = this->m_ptr;
				ret.resource = this->resource_keeper->update(this->resource, &this->resource);
				return ret;
			}
			template<class T>
			inline weakuptr<T>::~weakuptr()
			{
				this->SetToNull();
			}
			template<class T>
			inline weakuptr<T>::weakuptr()
			{
			}
			template<class T>
			inline weakuptr<T>::weakuptr(nullptr_t)
			{
			}

			template<class T>
			inline weakuptr<T>::weakuptr(uptr<T> const &rhs)
			{
				if (rhs.IsNull())
					return;
				if (rhs.resource_keeper->is_invalid(rhs.resource))//如果已经失效,不会进行引用
					return;
				this->m_ptr = rhs.m_ptr;
				this->resource = rhs.resource;
				this->resource_keeper = rhs.resource_keeper;
				++this->resource->weak_reference_count;
			}
			template<class T>
			inline weakuptr<T>::weakuptr(weakuptr const &rhs)
			{
				if (rhs.IsInvalid())
					return;
				this->m_ptr = rhs.m_ptr;
				this->resource = rhs.resource;
				this->resource_keeper = rhs.resource_keeper;
				++this->resource->weak_reference_count;
			}
			template<class T>
			inline weakuptr<T>::weakuptr(weakuptr &&lhs)noexcept
			{
				this->m_ptr = lhs.m_ptr;
				this->resource = lhs.resource;
				this->resource_keeper = lhs.resource_keeper;
				lhs.m_ptr = nullptr;
				lhs.resource = nullptr;
				lhs.resource_keeper = nullptr;
			}

			template<class T>
			inline weakuptr<T>& weakuptr<T>::operator=(nullptr_t)
			{
				this->SetToNull();
				return *this;
			}

			template<class T>
			inline weakuptr<T> & weakuptr<T>::operator=(uptr<T> const &rhs)
			{
				if (rhs.m_ptr == this->m_ptr)
					return *this;
				if (this->limited_type_id != -1)//如果是限定类型
					if (this->limited_type_id != rhs.limited_type_id)//如果限定的类型对不上
						return *this;//返回自己
				this->SetToNull();
				if (!rhs.IsNull())
				{
					if (rhs.resource_keeper->is_invalid(rhs.resource))
						return *this;
					this->resource = rhs.resource;
					this->m_ptr = rhs.m_ptr;
					this->resource_keeper = rhs.resource_keeper;
					++this->resource->weak_reference_count;//弱引用自加
				}
				return *this;
			}

			template<class T>
			inline weakuptr<T> & weakuptr<T>::operator=(weakuptr const & rhs)
			{
				if (rhs.m_ptr == this->m_ptr)
					return *this;
				if (this->limited_type_id != -1)//如果是限定类型
					if (this->limited_type_id != rhs.limited_type_id)//如果限定的类型对不上
						return *this;//返回自己
				this->SetToNull();
				if (!rhs.IsInvalid())
				{
					this->resource = rhs.resource;
					this->m_ptr = rhs.m_ptr;
					this->resource_keeper = rhs.resource_keeper;
					++this->resource->weak_reference_count;//弱引用自加
				}
				return *this;
			}

			template<class T>
			inline weakuptr<T> & weakuptr<T>::operator=(weakuptr &&lhs)
			{
				if (this->limited_type_id != -1)//如果是限定类型
					if (this->limited_type_id != lhs.limited_type_id)//如果限定的类型对不上
					{
						lhs.SetToNull();//将move对象置空
						return *this;
					}
				if (lhs.m_ptr != this->m_ptr)
				{
					this->SetToNull();
					this->m_ptr = lhs.m_ptr;
					this->resource_keeper = lhs.resource_keeper;
					this->resource = lhs.resource;
				}
				lhs.m_ptr = nullptr;
				lhs.resource_keeper = nullptr;
				lhs.resource = nullptr;
				return *this;
			}

			template<class T>
			inline T * weakuptr<T>::GetObjectPtr()
			{
				return this->m_ptr;
			}

			template<class T>
			inline T const * weakuptr<T>::GetObjectPtr() const
			{
				return this->m_ptr;
			}



			template<class U>
			struct IsUPtr {
				typedef U InnerType;
				enum { R = false };
			};
			template<class U>
			struct IsUPtr<uptr<U>> {
				typedef U InnerType;
				enum { R = true };
			};
			template<>
			struct IsUPtr<uobject> {
				typedef uobject InnerType;
				enum { R = true };
			};

			template<class T>
			inline uptr<T> UPtrThis<T>::GetThisUPtr() const
			{
				return this->uptr_this.ToUPtr();
			}
			template<class T, bool based>
			inline void Internal::set_uptr_this<T, based>::set_this(T *, uptr<T> const &)
			{
			}
			template<class T>
			inline void Internal::set_uptr_this<T, true>::set_this(T *object_ptr, uptr<T> const &ptr)
			{
				((UPtrThis<T>*)object_ptr)->uptr_this = ptr;
			}

			template<class U>
			inline U Internal::object_converter<U, true>::Convert(uobject const&target)
			{
				return target.ToUPtr<typename IsUPtr<U>::InnerType>();
			}
			template<class U,bool is_uptr>
			inline U Internal::object_converter<U, is_uptr>::Convert(uobject const&target) {
				return *target.To<U>();
			}
		}
		//配置协变
		namespace Convariance
		{
			template<class T>
			struct IConvariance<System::UPointer::uptr<T>>
			{
				typedef T ConvarianceType;
			};
			template<class T>
			struct IsCovariant<System::UPointer::uptr<T>>
			{
				//使用附加& 欺骗模板这是一个协变类型
				static constexpr bool Judge = Template::IsNakedType<T>::R;
				enum {
					R = IsCovariant<typename Template::If<Judge, T&, T>::T>::R
				};
			};
		}
	}
}