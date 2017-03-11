#pragma once
namespace HL
{
	namespace System
	{
		namespace Interface
		{
			//为类型提供[智能指针<>]默认索引器重载
			//接口类型定义规范:
			//typdef:
			//IndexType:索引值类型
			//ReturnType:返回值类型
			template<class T>
			struct IndexerSupportInterface
			{
				typedef Void ReturnType;
				typedef Void IndexType;
			};

			//为类型提供[智能指针<>]默认仿函数行为重载
			//接口类型定义规范:
			//typdef:
			//ReturnType:返回值类型
			template<class T>
			struct FunctorSupportInterface
			{
				typedef Void ReturnType;
			};

			//为类型提供[智能指针<>]默认迭代器接口重载
			//接口类型定义规范:
			//typdef:
			//IteratorType:迭代器类型
			//ConstIteratorType:const迭代器类型
			template<class T>
			struct IteratorSupportInterface
			{
				typedef Void* IteratorType;
				typedef Void* ConstIteratorType;
			};
		}
	}
}