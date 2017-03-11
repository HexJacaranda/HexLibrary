#pragma once
namespace HL
{
	namespace System
	{
		namespace Reflection
		{
			//Ϊĳһ���Ͷ�̬�����������ṩ�ӿ�
			class IDynamicProvider {
			public:

			};
			namespace Inner
			{
				//�������ڵ�
				class ITypeTreeNode {
				public:
					//�����ռ�
					static const int Namespace = 0x00000000;
					//��ͨ����
					static const int NormalType = 0x00000001;
					//����
					static const int MethodType = 0x00000002;
					//�ֶ�
					static const int FieldType = 0x00000003;

					///////////////////////////////////////////////////////////////////////////

					//�ڵ�����,Ĭ��Ϊ�����ռ�
					int NodeType = Namespace;
					//�����Ϳ���,�����ռ�Ĭ��Ϊ-1;
					int NodeID = -1;
					//�ڵ�����
					System::UString NodeName;
					//�ӽڵ�
					System::Generic::UArray<UPointer::uptr<ITypeTreeNode>>Children;
					//���ڵ�(���е������ü���,��Ҫ��Ϊ�˺ϳ����Ʒ���)
					UPointer::weakuptr<ITypeTreeNode> Father;
					//�ڵ���
					UPointer::uptr<Threading::RWLock> Syncroot;

					ITypeTreeNode() {
						Syncroot = Reference::newptr<Threading::RWLock>();
					}
					virtual ~ITypeTreeNode() {
					}
				};

				//��ڵ�
				class ClassTypeNode :public ITypeTreeNode {
				public:
					//����ʵ����̬����������
					UPointer::uptr<IDynamicProvider> Provider;
					//����
					UPointer::weakuptr<ClassTypeNode> BaseType;
					//��С
					int SizeOf = 0;
					//�Ƿ�ΪSealed
					bool IsSealed = false;
					//�Ƿ�Ϊֵ����
					bool IsValueType = false;

					ClassTypeNode() {
						this->NodeType = this->NormalType;
					}
					ClassTypeNode(UPointer::uptr<IDynamicProvider> const&provider) :Provider(provider) {
						this->NodeType = this->NormalType;
					}
					~ClassTypeNode() {}
				};

				//�����ڵ�
				class MethodTypeNode :public ITypeTreeNode {
				public:
					System::Functional::UDelegate Method;
					bool IsPublic = true;
					bool IsVirtual = false;
					MethodTypeNode() {
						this->NodeType = MethodType;
						this->NodeID = -2;
					}
					MethodTypeNode(System::Functional::UDelegate const& method) :Method(method) {
						this->NodeType = MethodType;
						this->NodeID = -2;
					}
					~MethodTypeNode() {}
				};

				//�ֶνڵ�
				class FieldTypeNode :public ITypeTreeNode {
				public:
					//�ֶ�ƫ��
					int Offset;
					//�ֶ�����
					UPointer::uptr<ClassTypeNode>FieldType;

					FieldTypeNode() {
						this->NodeID = -3;
					}
					FieldTypeNode(int offset) :Offset(offset) {
						this->NodeID = -3;
					}
				};

				System::UString StandardizeFullName(const char * Name)
				{
					size_t size = Internal::BasicStringLength(Name);
					std::size_t convert = 0;
					wchar_t *str = (wchar_t*)Memory::Allocator::Alloc(++size);
					mbstowcs_s(&convert, str, size, Name, size);

					System::UString ret = newptr<System::String>(String::ManageFrom(str, size - 1, size));

					ret->Replace(L"::", L".");
					return ret;
				}
			}

		}
	}
}