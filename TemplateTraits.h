#pragma once
namespace HL
{
	namespace Template
	{
		template<class R>struct RemoveLReference { typedef R T; };
		template<class R>struct RemoveLReference<R&> { typedef R T; };
		template<class R>struct RemoveLReference<R const&> { typedef R const T; };
		template<class R>struct RemoveLReference<R volatile&> { typedef R volatile T; };
		template<class R>struct RemoveLReference<R const volatile&> { typedef R const volatile T; };

		template<class R>struct RemoveCLRLReference {
			typedef R T;
		};
		template<class R>struct RemoveCLRSign {
			typedef R F;
		};
#if _CLR

		template<class R>struct RemoveLReference<R^% > { typedef R^ T; };
		template<class R>struct RemoveLReference<R const^% > { typedef R^ T; };
		template<class R>struct RemoveLReference<R volatile^% > { typedef R^ T; };
		template<class R>struct RemoveLReference<R const volatile^% > { typedef R^ T; };

		template<class R>struct RemoveCLRSign<R^> {
			typedef R F;
		};

		template<class R>struct RemoveCLRLReference<R^> {
			typedef R^ T;
		};
#endif
		template<class R>struct RemoveRReference { typedef R T; };
		template<class R>struct RemoveRReference<R&&> { typedef R T; };

		template<class R>struct RemoveReference { 
			typedef typename RemoveLReference<typename RemoveRReference<R>::T>::T T;
		};


#ifdef _USE_RIGHT_VALUE_REFERENCE
		template<class R>struct RemoveRReference<R const&&> { typedef R const T; };
		template<class R>struct RemoveRReference<R volatile&&> { typedef R volatile T; };
		template<class R>struct RemoveRReference<R const volatile&&> { typedef R const volatile T; };
#endif
		template<class R>struct RemoveConst { typedef R  T; };
		template<class R>struct RemoveConst<R const> { typedef R  T; };
		template<class R>struct RemoveConst<R const&> { typedef R&  T; };
		template<class R>struct RemoveConst<R volatile const> { typedef R volatile  T; };
		template<class R>struct RemoveConst<R volatile const&> { typedef R volatile&  T; };
#ifdef _USE_RIGHT_VALUE_REFERENCE
		template<class R>struct RemoveConst<R const&&> { typedef R&& T; };
		template<class R>struct RemoveConst<R volatile const&&> { typedef R volatile&& T; };
#endif
		template<class R>struct RemoveVolatile { typedef R T; };
		template<class R>struct RemoveVolatile<R volatile> { typedef R T; };
		template<class R>struct RemoveVolatile<R volatile&> { typedef R& T; };
		template<class R>struct RemoveVolatile<R volatile const> { typedef R const T; };
		template<class R>struct RemoveVolatile<R volatile const&> { typedef R const& T; };
#ifdef _USE_RIGHT_VALUE_REFERENCE
		template<class R>struct RemoveVolatile<R volatile&&> { typedef R && T; };
		template<class R>struct RemoveVolatile<R volatile const&&> { typedef R const&& T; };
#endif
		template<class R>struct RemoveAllExtents { typedef R T; };
		template<class R>struct RemoveAllExtents<R const> { typedef R T; };
		template<class R>struct RemoveAllExtents<R volatile> { typedef R T; };
		template<class R>struct RemoveAllExtents<R volatile const> { typedef R T; };
		template<class R>struct RemoveAllExtents<R&> { typedef R T; };
		template<class R>struct RemoveAllExtents<R const&> { typedef R T; };
		template<class R>struct RemoveAllExtents<R volatile&> { typedef R T; };
		template<class R>struct RemoveAllExtents<R volatile const&> { typedef R T; };
		template<class R>struct RemoveAllExtents<R*> { typedef R T; };
		template<class R>struct RemoveAllExtents<R const*> { typedef R T; };
		template<class R>struct RemoveAllExtents<R volatile*> { typedef R T; };
		template<class R>struct RemoveAllExtents<R volatile const*> { typedef R T; };
#if _CLR
		template<class R>struct RemoveAllExtents<R^% > { typedef R^ T; };
		template<class R>struct RemoveAllExtents<R const^% > { typedef R^ T; };
		template<class R>struct RemoveAllExtents<R volatile^% > { typedef R^ T; };
		template<class R>struct RemoveAllExtents<R const volatile^% > { typedef R^ T; };
#endif
#ifdef _USE_RIGHT_VALUE_REFERENCE
		template<class R>struct RemoveAllExtents<R&&> { typedef R T; };
		template<class R>struct RemoveAllExtents<R const&&> { typedef R T; };
		template<class R>struct RemoveAllExtents<R volatile&&> { typedef R T; };
		template<class R>struct RemoveAllExtents<R volatile const&&> { typedef R T; };
#endif
		template<class R>struct RemovePointer { typedef R T; };
		template<class R>struct RemovePointer<R*> { typedef R T; };
		template<class R>struct RemovePointer<R* const> { typedef R T; };
		template<class R>struct RemovePointer<R* volatile> { typedef R T; };
		template<class R>struct RemovePointer<R* volatile const> { typedef R T; };
		template<class R>struct RemovePointer<R*&> { typedef R T; };
		template<class R>struct RemovePointer<R* const&> { typedef R T; };
		template<class R>struct RemovePointer<R* volatile&> { typedef R T; };
		template<class R>struct RemovePointer<R* volatile const&> { typedef R T; };
#ifdef _USE_RIGHT_VALUE_REFERENCE
		template<class R>struct RemovePointer<R*&&> { typedef R T; };
		template<class R>struct RemovePointer<R* const&&> { typedef R T; };
		template<class R>struct RemovePointer<R* volatile&&> { typedef R T; };
		template<class R>struct RemovePointer<R* volatile const&&> { typedef R T; };
#endif


		template<class R>struct AddLReference { typedef R& T; };
		template<class R>struct AddLReference<R&> { typedef R& T; };
		template<class R>struct AddLReference<R volatile&> { typedef R volatile& T; };
		template<class R>struct AddLReference<R const&> { typedef R const& T; };
		template<class R>struct AddLReference<R const> { typedef R const& T; };
		template<class R>struct AddLReference<R volatile> { typedef R volatile& T; };
		template<class R>struct AddLReference<R volatile const> { typedef R const volatile& T; };
		template<class R>struct AddLReference<R volatile const&> { typedef R const volatile& T; };
#if _CLR
		template<class R>struct AddLReference<R^> { typedef R^% T; };
		template<class R>struct AddLReference<R const^> { typedef R const^% T; };
		template<class R>struct AddLReference<R volatile^> { typedef R volatile^% T; };
		template<class R>struct AddLReference<R volatile const^> { typedef R const volatile^% T; };
#endif


		template<>struct AddLReference<void> { typedef void T; };
		template<>struct AddLReference<const void> { typedef const void T; };
#ifdef _USE_RIGHT_VALUE_REFERENCE
		template<class LT>struct AddLReference<LT &&> { typedef LT& T; };
		template<class LT>struct AddLReference<LT const&&> { typedef LT const& T; };
		template<class LT>struct AddLReference<LT volatile&&> { typedef LT volatile& T; };
		template<class LT>struct AddLReference<LT volatile const&&> { typedef LT volatile const& T; };
#endif
		template<class LT>struct AddConst { typedef LT const T; };
		template<class LT>struct AddConst<LT const> { typedef LT const T; };
		template<class LT>struct AddConst<LT &> { typedef LT const& T; };
		template<class LT>struct AddConst<LT const&> { typedef LT const& T; };
		template<class LT>struct AddConst<LT volatile> { typedef LT volatile const T; };
		template<class LT>struct AddConst<LT volatile&> { typedef LT const volatile& T; };
		template<class LT>struct AddConst<LT volatile const> { typedef LT volatile const T; };
		template<class LT>struct AddConst<LT volatile const&> { typedef LT const volatile& T; };
#ifdef _USE_RIGHT_VALUE_REFERENCE
		template<class LT>struct AddConst<LT &&> { typedef LT const&& T; };
		template<class LT>struct AddConst<LT const&&> { typedef LT const&& T; };
		template<class LT>struct AddConst<LT volatile&&> { typedef LT volatile const&& T; };
		template<class LT>struct AddConst<LT volatile const&&> { typedef LT volatile const&& T; };
#endif
		template<class LT>struct AddVolatile { typedef LT volatile T; };
		template<class LT>struct AddVolatile<LT volatile> { typedef LT volatile T; };
		template<class LT>struct AddVolatile<LT &> { typedef LT volatile& T; };
		template<class LT>struct AddVolatile<LT const&> { typedef LT const volatile& T; };
		template<class LT>struct AddVolatile<LT const> { typedef LT volatile const T; };
		template<class LT>struct AddVolatile<LT volatile&> { typedef LT volatile& T; };
		template<class LT>struct AddVolatile<LT volatile const> { typedef LT volatile const T; };
		template<class LT>struct AddVolatile<LT volatile const&> { typedef LT const volatile& T; };
#ifdef _USE_RIGHT_VALUE_REFERENCE
		template<class LT>struct AddVolatile<LT &&> { typedef LT volatile&& T; };
		template<class LT>struct AddVolatile<LT const&&> { typedef LT volatile const&& T; };
		template<class LT>struct AddVolatile<LT volatile&&> { typedef LT volatile&& T; };
		template<class LT>struct AddVolatile<LT volatile const&&> { typedef LT volatile const&& T; };
#endif
		template<class LT>struct AddPointer { typedef LT * T; };
		template<class LT>struct AddPointer<LT volatile> { typedef LT volatile* T; };
		template<class LT>struct AddPointer<LT &> { typedef LT * T; };
		template<class LT>struct AddPointer<LT const&> { typedef LT const* T; };
		template<class LT>struct AddPointer<LT const> { typedef LT const* T; };
		template<class LT>struct AddPointer<LT volatile&> { typedef LT volatile* T; };
		template<class LT>struct AddPointer<LT volatile const> { typedef LT volatile const* T; };
		template<class LT>struct AddPointer<LT volatile const&> { typedef LT const volatile* T; };
#ifdef _USE_RIGHT_VALUE_REFERENCE
		template<class LT>struct AddRReference { typedef LT&& T; };
		template<class LT>struct AddRReference<LT volatile> { typedef LT volatile&& T; };
		template<class LT>struct AddRReference<LT &> { typedef LT && T; };
		template<class LT>struct AddRReference<LT const&> { typedef LT const&& T; };
		template<class LT>struct AddRReference<LT const> { typedef LT const&& T; };
		template<class LT>struct AddRReference<LT volatile&> { typedef LT volatile&& T; };
		template<class LT>struct AddRReference<LT volatile const> { typedef LT volatile const&& T; };
		template<class LT>struct AddRReference<LT volatile const&> { typedef LT volatile const&& T; };
		template<class LT>struct AddRReference<LT &&> { typedef LT&& T; };
		template<class LT>struct AddRReference<LT const&&> { typedef LT const&& T; };
		template<class LT>struct AddRReference<LT volatile&&> { typedef LT volatile&& T; };
		template<class LT>struct AddRReference<LT volatile const&&> { typedef LT volatile const&& T; };
#endif
		template<class T>
		struct IsPointer {
			enum { R = false };
		};
		template<class T>
		struct IsPointer<T*> {
			enum { R = true };
		};
		template<class T>
		struct IsPointer<T const*> {
			enum { R = true };
		};
		template<class T>
		struct IsPointer<T volatile*> {
			enum { R = true };
		};
		template<class T>
		struct IsPointer<T const volatile*> {
			enum { R = true };
		};
		template<class T>
		struct IsReference {
			enum { R = 0 };
		};
		template<class T>
		struct IsReference<T&> {
			enum { R = true };
		};
		template<class T>
		struct IsReference<T const&> {
			enum { R = true };
		};
		template<class T>
		struct IsReference<T volatile&> {
			enum { R = true };
		};
		template<class T>
		struct IsReference<T const volatile&> {
			enum { R = true };
		};
		template<class T>
		struct IsReference<T&&> {
			enum { R = true };
		};
		template<class T>
		struct IsReference<T const&&> {
			enum { R = true };
		};
		template<class T>
		struct IsReference<T volatile&&> {
			enum { R = true };
		};
		template<class T>
		struct IsReference<T const volatile&&> {
			enum { R = true };
		};

		template<class T>
		struct IsConst {
			enum { R = false };
		};
		template<class T>
		struct IsConst<T const> {
			enum { R = 1 };
		};
		template<class T>
		struct IsConst<T const&> {
			enum { R = 1 };
		};
		template<class T>
		struct IsConst<T const volatile> {
			enum { R = 1 };
		};
		template<class T>
		struct IsConst<T const volatile&> {
			enum { R = 1 };
		};

		template<class T>
		struct IsGCHandle {
			enum { R = false };
		};
#if _CLR

		template<class T>
		struct IsGCHandle<T^> {
			enum { R = true };
		};
		template<class T>
		struct IsGCHandle<T const^> {
			enum { R = true };
		};
		template<class T>
		struct IsGCHandle<T const volatile^> {
			enum { R = true };
		};
		template<class T>
		struct IsGCHandle<T volatile^> {
			enum { R = true };
		};
#endif
		template<class T>
		struct IsArray {
			enum { R = false };
		};
		template<class T, size_t num>
		struct IsArray<T(&)[num]>
		{
			enum { R = true };
		};
		template<class T, size_t num>
		struct IsArray<T(*)[num]>
		{
			enum { R = true };
		};
		template<class T, size_t num>
		struct IsArray<T[num]>
		{
			enum { R = true };
		};
		//值类型判断及其特化
		template<class T>struct IsValueType{ enum { R = std::is_pod<T>::value }; };
		template<>struct IsValueType<int>{ enum { R = true }; };
		template<>struct IsValueType<unsigned>{ enum { R = true }; };
		template<>struct IsValueType<float>{ enum { R = true }; };
		template<>struct IsValueType<long>{ enum { R = true }; };
		template<>struct IsValueType<unsigned long>{ enum { R = true }; };
		template<>struct IsValueType<long long>{ enum { R = true }; };
		template<>struct IsValueType<unsigned long long>{ enum { R = true }; };
		template<>struct IsValueType<double>{ enum { R = true }; };
		template<>struct IsValueType<long double>{ enum { R = true }; };
		template<>struct IsValueType<char>{ enum { R = true }; };
		template<>struct IsValueType<unsigned char>{ enum { R = true }; };
		template<>struct IsValueType<wchar_t> { enum { R = true }; };
		//所有指针当作值类型
		template<class T>struct IsValueType<T*> { enum { R = true }; };


		//基元类型判断及其特化
		template<class T>struct IsPrimitiveType { enum { R = false }; };
		template<>struct IsPrimitiveType<int> { enum { R = true }; };
		template<>struct IsPrimitiveType<unsigned> { enum { R = true }; };
		template<>struct IsPrimitiveType<float> { enum { R = true }; };
		template<>struct IsPrimitiveType<long> { enum { R = true }; };
		template<>struct IsPrimitiveType<unsigned long> { enum { R = true }; };
		template<>struct IsPrimitiveType<long long> { enum { R = true }; };
		template<>struct IsPrimitiveType<unsigned long long> { enum { R = true }; };
		template<>struct IsPrimitiveType<double> { enum { R = true }; };
		template<>struct IsPrimitiveType<long double> { enum { R = true }; };
		template<>struct IsPrimitiveType<char> { enum { R = true }; };
		template<>struct IsPrimitiveType<unsigned char> { enum { R = true }; };
		template<>struct IsPrimitiveType<wchar_t> { enum { R = true }; };
		//所有指针当作基元类型
		template<class T>struct IsPrimitiveType<T*> { enum { R = true }; };


		template<class T1, class T2>struct IsSame { enum { R = false }; };
		template<class T>struct IsSame<T, T> { enum { R = true }; };

		//是否为纯类型
		template<class T>
		struct IsNakedType
		{
			typedef typename RemoveAllExtents<T>::T After;
			enum {
				R = IsSame<T, After>::R
			};
		};


		template<class U>inline long ___IsClassOrUnionTest(void(U::* p)()) { return 0; }
		template<class U>inline char ___IsClassOrUnionTest(...) { return 0; }
		template<class U>inline long ___IsBaseOfTest(U* p) { return 0; }
		template<class U>inline char ___IsBaseOfTest(...) { return 0; }
		template<class Base, class Derived>struct IsBaseOf
		{
		private:
			typedef typename RemoveVolatile<typename RemoveConst<typename RemoveLReference<typename RemoveRReference<Base>::T>::T>::T>::T B;
			typedef typename RemoveVolatile<typename RemoveConst<typename RemoveLReference<typename RemoveRReference<Derived>::T>::T>::T>::T D;
		public:
			static const bool R = sizeof(___IsClassOrUnionTest<Base>(0)) == sizeof(long) && sizeof(___IsBaseOfTest<B>((D*)0)) == sizeof(long);
		};

		template<bool LR, class True, class False>
		struct If {
			typedef True T;
		};
		template<class True, class False>
		struct If<false, True, False> {
			typedef False T;
		};
		template<bool LR, class TT>
		struct EnableIf {
			typedef TT T;
		};
		template<class TT>
		struct EnableIf<false, TT>
		{

		};

		template<bool...Expressions>struct And;

		template<bool First,bool...Rest>
		struct And<First, Rest...>
		{
			enum
			{
				R = (First == false) ? false : And<Rest...>::R
			};
		};
		template<>
		struct And<>
		{
			enum 
			{
				R = true
			};
		};

		template<bool...Expressions>struct Or;
		template<bool First,bool...Rest>
		struct Or<First,Rest...>
		{
			enum 
			{
				R = (First == true) ? true : Or<Rest...>::R
			};
		};
		template<>
		struct Or<>
		{
			enum 
			{
				R = false
			};
		};



#define TypeIf(Statement,TypeL,TypeR) typename If<Statement,TypeL,TypeR>::T


#define HasFunctionMember(Member)\
		template<class TT, class Ret, class...Args>\
		struct FunctionContained_##Member\
		{\
			template<class P, Ret(P::*ptr)(Args...)>struct TestStruct{};\
            template<class P, Ret(P::*ptr)(Args...)const>struct ConstTestStruct{};\
			template<class P>\
			static char Test(TestStruct<P,&P::##Member>*);\
            template<class P>\
			static char Test(ConstTestStruct<P,&P::##Member>*);\
			template<class P>\
			static long Test(...);\
		public:\
			static constexpr bool R = (sizeof(Test<TT>(nullptr)) == sizeof(char));\
		};


		template<class...Args>
		struct TypeList
		{
			static constexpr int Count = sizeof...(Args);
		};

		template<int index, class Sign>
		struct TypeListGetAt;
		template<int index, class T, class...Args>
		struct TypeListGetAt<index, TypeList<T, Args...>>
		{
			typedef typename TypeListGetAt<index - 1, TypeList<Args...>>::T T;
		};
		template<class T, class...Args>
		struct TypeListGetAt<0, TypeList<T, Args...>>
		{
			typedef T T;
		};


		template<class Functor>
		struct GetFunctionInfoImpl {
			enum { Type = -1 };
			typedef HL::Void R;
			typedef TypeList<Void> Types;
		};

		template<bool R, class FirstT, class Functor>
		struct RetSwitch
		{
			typedef FirstT R;
		};
		template<class FirstT, class Functor>
		struct RetSwitch<true, FirstT, Functor>
		{
			typedef typename GetFunctionInfoImpl<decltype(&Functor::operator())>::R R;
		};
		template<bool R, class FirstT, class Functor>
		struct TypesSwitch
		{
			typedef FirstT R;
		};
		template<class FirstT, class Functor>
		struct TypesSwitch<true, FirstT, Functor>
		{
			typedef typename GetFunctionInfoImpl<decltype(&Functor::operator())>::Types R;
		};


		template<class Ret, class...Args>
		struct GetFunctionInfoImpl<Ret(__stdcall*)(Args...)>
		{
			typedef Ret R;
			typedef TypeList<Args...> Types;
		};
		template<class Ret, class...Args>
		struct GetFunctionInfoImpl<Ret(__cdecl*)(Args...)>
		{
			typedef Ret R;
			typedef TypeList<Args...> Types;
		};
		template<class Ret, class...Args>
		struct GetFunctionInfoImpl<Ret(__fastcall*)(Args...)>
		{
			typedef Ret R;
			typedef TypeList<Args...> Types;
		};
		template<class Ret, class...Args>
		struct GetFunctionInfoImpl<Ret(__vectorcall*)(Args...)>
		{
			typedef Ret R;
			typedef TypeList<Args...> Types;
		};

		template<class TT, class Ret, class...Args>
		struct GetFunctionInfoImpl<Ret(TT::*)(Args...)>
		{
			typedef Ret R;
			typedef TypeList<Args...> Types;
		};

		template<class TT, class Ret, class...Args>
		struct GetFunctionInfoImpl<Ret(TT::*)(Args...)const>
		{
			typedef Ret R;
			typedef TypeList<Args...> Types;
		};

		template<class Functor>
		struct GetFunctionInfo {
		public:
			typedef typename GetFunctionInfoImpl<Functor>::R FirstT;//前置判断
			typedef typename GetFunctionInfoImpl<Functor>::Types FirstTypes;

			typedef typename RetSwitch<Template::IsSame<HL::Void, FirstT>::R, FirstT, Functor>::R R;
			typedef typename TypesSwitch<Template::IsSame<TypeList<HL::Void>, FirstTypes>::R, FirstTypes, Functor>::R Types;
		};
	}
	namespace System
	{
		template<class T>
		inline constexpr typename Template::RemoveLReference<T>::T&& Move(T&&Arg)noexcept
		{
			return static_cast<typename Template::RemoveLReference<T>::T&&>(Arg);
		}
		template<class T>
		inline constexpr void Swap(T&Left, T&Right)noexcept {
			T inter(Move(Left));
			Left = Move(Right);
			Right = Move(inter);
		}
		template<class T>
		inline constexpr T&& Forward(typename Template::RemoveReference<T>::T &&Arg)noexcept
		{
			return static_cast<T&&>(Arg);
		}
		template<class T>
		inline constexpr T&& Forward(typename Template::RemoveReference<T>::T &Arg)noexcept
		{
			return static_cast<T&&>(Arg);
		}
	}
}