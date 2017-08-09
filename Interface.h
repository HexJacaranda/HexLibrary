#pragma once
namespace HL
{
	namespace System
	{
		namespace Interface
		{
			//Ϊ�����ṩ[����ָ��<>]Ĭ������������
			//�ӿ����Ͷ���淶:
			//typdef:
			//IndexType:����ֵ����
			//ReturnType:����ֵ����
			template<class T>
			struct IndexerSupportInterface
			{
				typedef Void ReturnType;
				typedef Void IndexType;
			};

			//Ϊ�����ṩ[����ָ��<>]Ĭ�Ϸº�����Ϊ����
			//�ӿ����Ͷ���淶:
			//typdef:
			//ReturnType:����ֵ����
			template<class T>
			struct FunctorSupportInterface
			{
				typedef Void ReturnType;
			};

			//Ϊ�����ṩ[����ָ��<>]Ĭ�ϵ������ӿ�����
			//�ӿ����Ͷ���淶:
			//typdef:
			//IteratorType:����������
			//ConstIteratorType:const����������
			template<class T>
			struct IteratorSupportInterface
			{
				typedef Void* IteratorType;
				typedef Void* ConstIteratorType;
			};
			//Ϊ�����ṩ[����ָ��<>]����/Э��ת��
			//�ӿ����Ͷ���淶:
			//enum:
			//Enable:ָʾ�Ƿ�Ϊ��/Э������
			template<class From,class To>
			struct CovariantSupportInterface
			{
				enum { Enable = false };
			};

			//�ɿ�¡�ӿ�
			template<class T>
			class ICloneable
			{
				template<bool Based,bool PriType,class AnyT>
				struct Aid
				{
					static AnyT GetClone(AnyT const&target) {
						return ((ICloneable<AnyT>*)&(target))->Clone();
					}
				};

				template<bool Based,class AnyT>
				struct Aid<Based, true, AnyT>
				{
					static AnyT GetClone(AnyT const&target) {
						return AnyT(target);
					}
				};

				template<bool PriType, class AnyT>
				struct Aid<false, PriType, AnyT>
				{
					static AnyT GetClone(AnyT const&target) {
						Exception::Throw<Exception::InterfaceNoImplementException>();
						return *((AnyT*)nullptr);
					}
				};

				template<bool Based, bool PriType, class AnyT>
				struct AidPtr
				{
					static AnyT* GetClone(AnyT const&target) {
						return new AnyT(((ICloneable<AnyT>*)&(target))->Clone());
					}
				};

				template<bool Based, class AnyT>
				struct AidPtr<Based, true, AnyT>
				{
					static AnyT* GetClone(AnyT const&target) {
						return new AnyT(target);
					}
				};

				template<bool PriType, class AnyT>
				struct AidPtr<false, PriType, AnyT>
				{
					static AnyT* GetClone(AnyT const&target) {
						HL::Exception::Throw<HL::Exception::InterfaceNoImplementException>();
						return nullptr;
					}
				};

				template<class AnyT>
				struct AidPtr<false, true, AnyT>
				{
					static AnyT* GetClone(AnyT const&target) {
						return new AnyT(target);
					}
				};
			public:
				virtual T Clone()const = 0;
				//��ø���ʵ��
				static T GetClone(T const&target) {
					return Aid<Template::IsBaseOf<ICloneable<T>, T>::R, Template::IsPrimitiveType<T>::R, T>::GetClone(target);
				}
				//��ø���ʵ��(��new�����ڶ��ϵĶ���)
				static T* GetClonePtr(T const&target) {
					return AidPtr<Template::IsBaseOf<ICloneable<T>, T>::R, Template::IsPrimitiveType<T>::R, T>::GetClone(target);
				}
			};
		}
	}
}