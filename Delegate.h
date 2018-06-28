#pragma once
namespace HL
{
	namespace System
	{
		namespace Functional
		{
			class InvokeFailedException;
			//自动推导标记
			class Auto {};
			namespace Internal
			{
				template<bool is_ref, class T>
				class ObjectConvertTo
				{
				public:
					static inline T const&Convert(UPointer::uobject const&obj)
					{
						return *const_cast<UPointer::uobject&>(obj).To<typename Template::RemoveLReference<T>::T>();
					}
				};
				template<class T>
				class ObjectConvertTo<true, T>
				{
				public:
					static inline T Convert(UPointer::uobject const&obj)
					{
						return static_cast<T&&>((T)obj);
					}
				};

				template<bool is_ref, class T>
				class ObjectConvertFrom
				{
				public:
					static inline UPointer::uobject Convert(T const&obj)
					{
						return newptr<T>(obj);
					}
				};
				template<class T>
				class ObjectConvertFrom<true, T>
				{
				public:
					static inline UPointer::uobject Convert(T const&obj)
					{
						return (UPointer::uobject)obj;
					}
				};

				template<class R, index_t num, class...Args>
				struct InvokeVaric {
					template<class Fx, class...args>
					static inline UPointer::uobject Invoke(Fx&f, UPointer::uptr<Generic::Array<UPointer::uobject>>const&arg, args const&...a)
					{
						return InvokeVaric<R, num - 1, Args...>::Invoke(f, arg, arg[num - 1], a...);
					}
					template<class TT, class Fx, class...args>
					static inline UPointer::uobject Invoke(TT obj, Fx&f, UPointer::uptr<Generic::Array<UPointer::uobject>>const&arg, args const&...a) {
						return InvokeVaric<R, num - 1, Args...>::Invoke(obj, f, arg, arg[num - 1], a...);
					}
				};
				template<class R, class...Args>
				struct InvokeVaric<R, 0, Args...> {
					template<class Fx, class...args>
					static inline UPointer::uobject Invoke(Fx&f, UPointer::uptr<Generic::Array<UPointer::uobject>>const&arg, args const&...a) {
						return ObjectConvertFrom<UPointer::IsUPtr<R>::R, R>::Convert(f(ObjectConvertTo<UPointer::IsUPtr<Args>::R, Args>::Convert(const_cast<UPointer::uobject&>((a)))...));
					}
					template<class TT, class Fx, class...args>
					static inline UPointer::uobject Invoke(TT obj, Fx&f, UPointer::uptr<Generic::Array<UPointer::uobject>>const&arg, args const&...a) {
						return ObjectConvertFrom<UPointer::IsUPtr<R>::R, R>::Convert((obj->*f)(ObjectConvertTo<UPointer::IsUPtr<Args>::R, Args>::Convert(const_cast<UPointer::uobject&>((a)))...));
					}
				};

				template<class R, class...Args>
				struct InvokeVaric<R&, 0, Args...> {
					template<class Fx, class...args>
					static inline UPointer::uobject Invoke(Fx&f, UPointer::uptr<Generic::Array<UPointer::uobject>>const&arg, args const&...a) {
						return Reference::ref(f(ObjectConvertTo<UPointer::IsUPtr<Args>::R, Args>::Convert(const_cast<UPointer::uobject&>((a)))...));
					}
					template<class TT, class Fx, class...args>
					static inline UPointer::uobject Invoke(TT obj, Fx&f, UPointer::uptr<Generic::Array<UPointer::uobject>>const&arg, args const&...a) {
						return Reference::ref((obj->*f)(ObjectConvertTo<UPointer::IsUPtr<Args>::R, Args>::Convert(const_cast<UPointer::uobject&>((a)))...));
					}
				};
				template<class...Args>
				struct InvokeVaric<void, 0, Args...>
				{
					template<class Fx, class...args>
					static inline UPointer::uobject Invoke(Fx& f, UPointer::uptr<Generic::Array<UPointer::uobject>>const&arg, args const&...a) {
						f(ObjectConvertTo<UPointer::IsUPtr<Args>::R, Args>::Convert(const_cast<UPointer::uobject&>((a)))...);
						return nullptr;
					}
					template<class TT, class Fx, class...args>
					static inline UPointer::uobject Invoke(TT obj, Fx&f, UPointer::uptr<Generic::Array<UPointer::uobject>>const&arg, args const&...a) {
						(obj->*f)(ObjectConvertTo<UPointer::IsUPtr<Args>::R, Args>::Convert(const_cast<UPointer::uobject&>(a))...);
						return nullptr;
					}
				};

				enum class InvokeType
				{
					Default,
					InvokeWithNativePtr,
					InvokeWithUPtr
				};

				template<class Literals, class RealFunctionType>class IDelegate;
				class DelegateBase
				{
				public:
					virtual inline DelegateBase*Clone()const = 0;
					virtual inline UPointer::uobject Invoke(UPointer::uptr<Generic::Array<UPointer::uobject>>params, UPointer::uobject object, InvokeType invoke_type)const = 0;
					virtual ~DelegateBase() {}
				};

				template<class Fx, class R, class...Args>
				class IDelegate<R(__stdcall)(Args...), Fx> :public DelegateBase
				{
					typedef InvokeVaric<R, sizeof...(Args), Args...> Invoker;
					Fx target_function;
				public:
					IDelegate(Fx function) :target_function(function) {}
					IDelegate(IDelegate const&rhs) :target_function(rhs.target_function) {}
					virtual inline DelegateBase*Clone()const {
						return new IDelegate(*this);
					}
					virtual inline UPointer::uobject Invoke(UPointer::uptr<Generic::Array<UPointer::uobject>>params, UPointer::uobject object, InvokeType invoke_type)const {
						if (invoke_type == InvokeType::Default)
							return Invoker::Invoke(target_function, params);
						else
							return nullptr;
					}
				};
				template<class Fx, class TT, class R, class...Args>
				class IDelegate<R(__stdcall TT::*)(Args...), Fx> :public DelegateBase
				{
					typedef InvokeVaric<R, sizeof...(Args), Args...> Invoker;
					Fx target_function;
					TT*object_ptr;
				public:
					IDelegate(Fx function, TT*object) :object_ptr(object), target_function(function) {}
					IDelegate(IDelegate const&rhs) :target_function(rhs.target_function), object_ptr(rhs.object_ptr) {}
					IDelegate() {}
					virtual inline DelegateBase*Clone()const {
						return new IDelegate(*this);
					}
					virtual inline UPointer::uobject Invoke(UPointer::uptr<Generic::Array<UPointer::uobject>>params, UPointer::uobject object, InvokeType invoke_type)const
					{
						if (invoke_type == InvokeType::Default)
							return Invoker::Invoke(object_ptr, target_function, params);
						else if (invoke_type == InvokeType::InvokeWithNativePtr)
						{
							uptr<Reference::IntPtr> ptr = object;
							return Invoker::Invoke((TT*)(*ptr), target_function, params);
						}
						else if (invoke_type == InvokeType::InvokeWithUPtr)
						{
							uptr<TT> ptr = object;
							return Invoker::Invoke(&(*ptr), target_function, params);
						}
						else
							return nullptr;
					}
				};

				//参数封装
				class ParameterPacker
				{
				public:
					template<class T, class...Args>
					static inline void Pack(UPointer::uptr<Generic::Array<UPointer::uobject>>&args, const T&Obj, const Args&...a) {
						args->Add(ObjectConvertFrom<UPointer::IsUPtr<Args>::R, T>::Convert(Obj));
						ParameterPacker::Pack<Args...>(args, a...);
					}
					template<class T>
					static inline void Pack(UPointer::uptr<Generic::Array<UPointer::uobject>>&args, const T& Obj) {
						args->Add(ObjectConvertFrom<UPointer::IsUPtr<T>::R, T>::Convert(Obj));
					}
				};
			}

			namespace Inner
			{
				template<class R, class Holder, class Functor>
				struct IDelegateConstruct 
				{
				
				};

				template<class R, class Functor, class...Args>
				struct IDelegateConstruct<R, Template::TypeList<Args...>, Functor>
				{
					inline static Delegate<R, Args...> Construct(Functor functor);
					template<class TT>
					inline static  Delegate<R, Args...>  Construct(Functor functor, TT* object);
				};
			}


			//参数打包
			template<class...Args>
			static inline UPointer::uptr<Generic::Array<UPointer::uobject>> params(Args const&...args) {
				UPointer::uptr<Generic::Array<UPointer::uobject>>ret = Reference::newptr<Generic::Array<UPointer::uobject>>(sizeof...(Args));
				Internal::ParameterPacker::Pack<Args...>(ret, args...);
				return ret;
			}
			//参数打包
			static inline UPointer::uptr<Generic::Array<UPointer::uobject>> params() {
				return nullptr;
			}

			//委托
			template<class R, class...Args>
			class Delegate
			{
				//友元
				template<class OR,class...Others>
				friend class Delegate;
				//签名
				typedef Template::TypeList<Args...> MySignature;

				//返回类型
				typedef typename Template::If<UPointer::IsUPtr<R>::R, typename UPointer::IsUPtr<R>::InnerType, R>::T RRet;

				typedef typename Template::If<Template::IsSame<R, void>::R, UPointer::uobject, RRet>::T Ret;
				//指针
				Internal::DelegateBase*method_ptr = nullptr;
			public:
				Delegate(){}
				Delegate(Delegate const&rhs) {
					if (rhs.method_ptr)
						this->method_ptr = rhs.method_ptr->Clone();
				}
				Delegate(Delegate &&lhs) {
					this->method_ptr = lhs.method_ptr;
					lhs.method_ptr = nullptr;
				}
				template<class OR, class...Others>
				Delegate(Delegate<OR,Others...> const&rhs) {
					bool Result = System::Convariance::ListCovariant<typename Delegate<OR, Others...>::MySignature, MySignature>::R;
					bool Result2 = System::Convariance::Contravariant<OR, R>::R;
					if (Result&&Result2)
					{
						if (rhs.method_ptr)
							this->method_ptr = rhs.method_ptr->Clone();
					}
				}
				template<class OR, class...Others>
				Delegate(Delegate<OR, Others...> &&lhs) {
					bool Result = System::Convariance::ListCovariant<typename Delegate<OR, Others...>::MySignature, MySignature>::R;
					bool Result2 = System::Convariance::Contravariant<OR, R>::R;
					if (Result&&Result2)
					{
						this->method_ptr = lhs.method_ptr;
						lhs.method_ptr = nullptr;
					}
				}
				Delegate(Internal::DelegateBase*target) {
					method_ptr = target;
				}
				template<class OR, class...Others>
				Delegate&operator=(Delegate<OR, Others...> const&rhs)
				{
					bool Result = System::Convariance::ListCovariant<typename Delegate<OR, Others...>::MySignature, MySignature>::R;
					bool Result2 = System::Convariance::Contravariant<OR, R>::R;
					if (Result&&Result2)
					{
						this->~Delegate();
						if (rhs.method_ptr)
							this->method_ptr = rhs.method_ptr->Clone();
					}
					return *this;
				}
				template<class OR, class...Others>
				Delegate&operator=(Delegate<OR, Others...> &&lhs)
				{
					bool Result = System::Convariance::ListCovariant<typename Delegate<OR, Others...>::MySignature, MySignature>::R;
					bool Result2 = System::Convariance::Contravariant<OR, R>::R;
					if (Result&&Result2)
					{
						this->~Delegate();
						this->method_ptr = lhs.method_ptr;
						lhs.method_ptr = nullptr;
					}
					return *this;
				}
				UPointer::uobject Invoke(UPointer::uptr<Generic::Array<UPointer::uobject>>const& params)const {
					return method_ptr->Invoke(params, nullptr, Internal::InvokeType::Default);
				}
				UPointer::uobject UInvoke(UPointer::uobject const& object, UPointer::uptr<Generic::Array<UPointer::uobject>> const&params)const {
					return method_ptr->Invoke(params, object, Internal::InvokeType::InvokeWithUPtr);
				}
				UPointer::uobject InvokeWith(UPointer::uptr<Reference::IntPtr> const& objptr, UPointer::uptr<Generic::Array<UPointer::uobject>>const&params)const {
					return method_ptr->Invoke(params, objptr, Internal::InvokeType::InvokeWithNativePtr);
				}
				Ret operator()(Args const&...args)const {
					return Invoke(params(args...));
				}
				~Delegate() {
					if (method_ptr) {
						delete method_ptr;
						method_ptr = nullptr;
					}
				}
			};
			//自动推导版本
			template<>
			class Delegate<Auto>
			{
				template<class OR, class...Others>
				friend class Delegate;
				typedef Template::TypeList<Auto> MySignature;
				//指针
				Internal::DelegateBase*method_ptr = nullptr;
			public:
				Delegate() {}
				Delegate(Delegate const&rhs) {
					if (rhs.method_ptr)
						this->method_ptr = rhs.method_ptr->Clone();
				}
				Delegate(Delegate &&lhs) {
					this->method_ptr = lhs.method_ptr;
					lhs.method_ptr = nullptr;
				}
				template<class OR, class...Others>
				Delegate(Delegate<OR, Others...> const&rhs) {
					if (rhs.method_ptr)
						this->method_ptr = rhs.method_ptr->Clone();
				}
				template<class OR, class...Others>
				Delegate(Delegate<OR, Others...> &&lhs) {
					this->method_ptr = lhs.method_ptr;
					lhs.method_ptr = nullptr;
				}
				template<class OR, class...Others>
				Delegate&operator=(Delegate<OR, Others...> const&rhs)
				{
					this->~Delegate();
					if (rhs.method_ptr)
						this->method_ptr = rhs.method_ptr->Clone();
					return *this;
				}
				template<class OR, class...Others>
				Delegate&operator=(Delegate<OR, Others...> &&lhs)
				{
					this->~Delegate();
					this->method_ptr = lhs.method_ptr;
					lhs.method_ptr = nullptr;
					return *this;
				}
				Delegate&operator=(Delegate const&rhs)
				{
					this->~Delegate();
					if (rhs.method_ptr)
						this->method_ptr = rhs.method_ptr->Clone();
					return *this;
				}
				Delegate&operator=(Delegate &&lhs)
				{
					this->~Delegate();
					this->method_ptr = lhs.method_ptr;
					lhs.method_ptr = nullptr;
					return *this;
				}
				UPointer::uobject Invoke(UPointer::uptr<Generic::Array<UPointer::uobject>>params)const {
					return method_ptr->Invoke(params, nullptr, Internal::InvokeType::Default);
				}
				UPointer::uobject UInvoke(UPointer::uobject object, UPointer::uptr<Generic::Array<UPointer::uobject>>params)const {
					return method_ptr->Invoke(params, object, Internal::InvokeType::InvokeWithUPtr);
				}
				UPointer::uobject InvokeWith(UPointer::uptr<Reference::IntPtr> objptr, UPointer::uptr<Generic::Array<UPointer::uobject>>params)const {
					return method_ptr->Invoke(params, objptr, Internal::InvokeType::InvokeWithNativePtr);
				}
				template<class...Args>
				UPointer::uobject operator()(Args const&...args)const {
					return Invoke(params(args...));
				}
				template<class R,class...Args>
				Delegate<R, Args...> Cast()const {
					if (this->method_ptr)
						return this->method_ptr->Clone();
					else
						return (Internal::DelegateBase*)nullptr;
				}
				~Delegate()
				{
					if (method_ptr) {
						delete method_ptr;
						method_ptr = nullptr;
					}
				}
			};

			namespace Inner
			{
				template<class R, class Functor, class ...Args>
				template<class TT>
				inline Delegate<R, Args...> IDelegateConstruct<R, Template::TypeList<Args...>, Functor>::Construct(Functor functor, TT * object)
				{
					return new Internal::IDelegate<R(__stdcall TT::*)(Args...), Functor>(functor, object);
				}

				template<class R, class Functor, class ...Args>
				inline Delegate<R, Args...> IDelegateConstruct<R, Template::TypeList<Args...>, Functor>::Construct(Functor functor)
				{
					return new Internal::IDelegate<R(__stdcall)(Args...), Functor>(functor);
				}
			}

			template<class Functor>
			auto Bind(Functor Target) {
				typedef typename Template::GetFunctionInfo<Functor>::Types Type;
				typedef typename Template::GetFunctionInfo<Functor>::R R;
				return Inner::IDelegateConstruct<R, Type, Functor>::Construct(Target);
			}
			template<class Functor,class TT>
			auto Bind(Functor Target,TT*ObjectPtr) {
				typedef typename Template::GetFunctionInfo<Functor>::Types Type;
				typedef typename Template::GetFunctionInfo<Functor>::R R;
				return Inner::IDelegateConstruct<R, Type, Functor>::Construct(Target, ObjectPtr);
			}

		}
		namespace Interface
		{
			template<class R, class...Args>
			struct FunctorSupportInterface<Functional::Delegate<R, Args...>>
			{
				typedef UPointer::uptr<R> ReturnType;
			};
		}
	}
}
