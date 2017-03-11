#pragma once
namespace HL
{
	namespace System
	{
		namespace Reflection
		{
			//为某一类型动态创建或销毁提供接口
			class IDynamicProvider {
			public:

			};
			namespace Inner
			{
				//类型树节点
				class ITypeTreeNode {
				public:
					//命名空间
					static const int Namespace = 0x00000000;
					//普通类型
					static const int NormalType = 0x00000001;
					//方法
					static const int MethodType = 0x00000002;
					//字段
					static const int FieldType = 0x00000003;

					///////////////////////////////////////////////////////////////////////////

					//节点类型,默认为命名空间
					int NodeType = Namespace;
					//仅类型可用,命名空间默认为-1;
					int NodeID = -1;
					//节点名称
					System::UString NodeName;
					//子节点
					System::Generic::UArray<UPointer::uptr<ITypeTreeNode>>Children;
					//父节点(进行单纯引用即可,主要是为了合成名称方便)
					UPointer::weakuptr<ITypeTreeNode> Father;
					//节点锁
					UPointer::uptr<Threading::RWLock> Syncroot;

					ITypeTreeNode() {
						Syncroot = Reference::newptr<Threading::RWLock>();
					}
					virtual ~ITypeTreeNode() {
					}
				};

				//类节点
				class ClassTypeNode :public ITypeTreeNode {
				public:
					//类型实例动态创建和销毁
					UPointer::uptr<IDynamicProvider> Provider;
					//基类
					UPointer::weakuptr<ClassTypeNode> BaseType;
					//大小
					int SizeOf = 0;
					//是否为Sealed
					bool IsSealed = false;
					//是否为值类型
					bool IsValueType = false;

					ClassTypeNode() {
						this->NodeType = this->NormalType;
					}
					ClassTypeNode(UPointer::uptr<IDynamicProvider> const&provider) :Provider(provider) {
						this->NodeType = this->NormalType;
					}
					~ClassTypeNode() {}
				};

				//函数节点
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

				//字段节点
				class FieldTypeNode :public ITypeTreeNode {
				public:
					//字段偏移
					int Offset;
					//字段类型
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