#pragma once
namespace HL
{
	namespace System
	{
		namespace Interface
		{
			//为类型提供[智能指针<>]默认仿函数行为重载
			//接口类型定义规范:
			//typdef:
			//ReturnType:返回值类型
			template<class T>
			struct FunctorSupportInterface
			{
				typedef Void ReturnType;
			};

			//为类型提供[智能指针<>]获取枚举迭代器接口重载
			//接口类型定义规范:
			//typdef:
			//IteratorType:迭代器类型
			//ConstIteratorType:const迭代器类型
			template<class T>
			struct EnumerableSupportInterface
			{
				typedef Void* IteratorType;
				typedef Void* ConstIteratorType;
			};
			//为类型提供[智能指针<>]默认迭代器接口重载
			//接口类型定义规范:
			//typdef:
			//UnReferenceType:解引用类型
			template<class T>
			struct IteratorSupportInterface
			{
				typedef Void* UnReferenceType;
			};

			//为类型提供[智能指针<>]的逆/协变转化
			//接口类型定义规范:
			//enum:
			//Enable:指示是否为逆/协变类型
			template<class From,class To>
			struct CovariantSupportInterface
			{
				enum { Enable = false };
			};

			//可克隆接口
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
				//获得复制实例(由new分配在堆上的对象)
				template<class T>
				static T* GetClonePtr(T const&target) {
					return AidPtr<Template::IsBaseOf<ICloneable, T>::R, Template::IsPrimitiveType<T>::R, T>::GetClone(target);
				}
			};

		}
	}
}