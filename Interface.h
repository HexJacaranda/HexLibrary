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
		}
	}
}