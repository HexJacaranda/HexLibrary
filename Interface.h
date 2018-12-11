#pragma once
namespace HL
{
	namespace System
	{
		namespace Interface
		{
			//Ϊ�����ṩ[����ָ��<>]Ĭ�Ϸº�����Ϊ����
			//�ӿ����Ͷ���淶:
			//typdef:
			//ReturnType:����ֵ����
			template<class T>
			struct FunctorSupportInterface
			{
				typedef Void ReturnType;
			};

			//Ϊ�����ṩ[����ָ��<>]��ȡö�ٵ������ӿ�����
			//�ӿ����Ͷ���淶:
			//typdef:
			//IteratorType:����������
			//ConstIteratorType:const����������
			template<class T>
			struct EnumerableSupportInterface
			{
				typedef Void* IteratorType;
				typedef Void* ConstIteratorType;
			};
			//Ϊ�����ṩ[����ָ��<>]Ĭ�ϵ������ӿ�����
			//�ӿ����Ͷ���淶:
			//typdef:
			//UnReferenceType:����������
			template<class T>
			struct IteratorSupportInterface
			{
				typedef Void* UnReferenceType;
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
			class ICloneable
			{
				template<bool Based, bool PriType, class AnyT>
				struct AidPtr
				{
					static AnyT* GetClone(AnyT const&target) {
						return (AnyT*)static_cast<ICloneable const&>(target).ClonePtr();
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
				virtual void* ClonePtr()const = 0;
				virtual ~ICloneable() {}
				//��ø���ʵ��(��new�����ڶ��ϵĶ���)
				template<class T>
				static T* GetClonePtr(T const&target) {
					return AidPtr<Template::IsBaseOf<ICloneable, T>::R, Template::IsPrimitiveType<T>::R, T>::GetClone(target);
				}
			};

		}
	}
}