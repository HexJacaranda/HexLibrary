#pragma once
namespace HL
{
	namespace System
	{
		namespace Convariance
		{
			//���һ�����͵�Э��(���)��ϸ��Ϣ
			template<class T>
			struct IConvariance
			{
				//Э������
				typedef typename Template::RemoveAllExtents<T>::T ConvarianceType;
			};

			//�Ƿ�Ϊ��Э��(���)����
			template<class T>
			struct IsCovariant
			{
				enum
				{
					R = Template::Or<Template::IsReference<T>::R, Template::IsPointer<T>::R>::R
				};
			};

			template<class From, class To>
			struct Covariant
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

			template<class From,class To>
			struct Contravariant 
			{
				static constexpr bool Judge()
				{
					constexpr bool First = IsCovariant<From>::R&&IsCovariant<To>::R;
					static_assert(First, "illegal covariant type!");
					typedef typename IConvariance<From>::ConvarianceType FromT;
					typedef typename IConvariance<To>::ConvarianceType ToT;

					constexpr bool Second = Template::IsBaseOf<ToT, FromT>::R;
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
						Judge = Covariant<typename Template::TypeListGetAt<Count - 1, FromList>::T, typename Template::TypeListGetAt<Count - 1, ToList>::T>::R,
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