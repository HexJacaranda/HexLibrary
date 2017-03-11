#pragma once
namespace HL
{
	namespace System
	{
		namespace Convariance
		{
			//协变标记
			template<class T>class out{};

			template<class T>struct UnSign
			{
				typedef T T;
			};
			template<class T>struct UnSign<out<T>>
			{
				typedef T T;
			};

			//标记一个类型的协变(逆变)详细信息
			template<class T>
			struct IConvariance
			{
				//协变类型
				typedef typename Template::RemoveAllExtents<T>::T ConvarianceType;
			};

			//是否为可协变(逆变)类型
			template<class T>
			struct IsCovariant
			{
				enum
				{
					R = Template::Or<Template::IsReference<T>::R, Template::IsPointer<T>::R>::R
				};
			};

			//类型列表
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



			template<class From, class To>
			struct Covariant
			{
				enum { R = false };
			};

			template<class From, class To>
			struct Covariant<out<From>, To>
			{
				static constexpr bool Judge()
				{
					constexpr bool First = IsCovariant<From>::R&&IsCovariant<To>::R;
					static_assert(First, "illegal covariant type!");
					typedef typename IConvariance<From>::ConvarianceType FromT;
					typedef typename IConvariance<To>::ConvarianceType ToT;
					constexpr bool Second = Template::IsBaseOf<FromT, ToT>::R;
					static_assert(Second, "illegal convariance!");
				}
				static constexpr bool R = Judge();
			};

			template<class From, class To>
			struct Covariant<out<From>, out<To>>
			{
				static constexpr bool Judge()
				{
					constexpr bool First = IsCovariant<From>::R&&IsCovariant<To>::R;
					static_assert(First, "illegal covariant type!");
					typedef typename IConvariance<From>::ConvarianceType FromT;
					typedef typename IConvariance<To>::ConvarianceType ToT;
					constexpr bool Second = Template::IsBaseOf<FromT, ToT>::R;
					static_assert(Second, "illegal convariance!");
				}
				static constexpr bool R = Judge();
			};

			template<class From, class To>
			struct Covariant<From, out<To>>
			{
				static constexpr bool Judge()
				{
					constexpr bool First = IsCovariant<From>::R&&IsCovariant<To>::R;
					static_assert(First, "illegal covariant type!");
					typedef typename IConvariance<From>::ConvarianceType FromT;
					typedef typename IConvariance<To>::ConvarianceType ToT;

					constexpr bool Second = Template::IsBaseOf<FromT,ToT>::R;
					static_assert(Second, "no inherited!");
					return true;
				}
				static constexpr bool R = Judge();
			};

			template<class FromList, class ToList>
			struct ListCovariant
			{
				template<int Count>
				struct ForEach
				{
					enum {
						Judge = Covariant<typename TypeListGetAt<Count - 1, FromList>::T, typename TypeListGetAt<Count - 1, ToList>::T>::R,
						R = (Judge == false) ? false : ForEach<Count - 1>::R
					};
				};

				template<>
				struct ForEach<0>
				{
					enum
					{
						R = true
					};
				};

				enum {
					R = ForEach<FromList::Count>::R
				};
			};
		}
	}
}