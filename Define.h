#pragma once
namespace HL
{
#if defined(_MSC_VER)//是否为VC编译器
#define VC 1
#endif

#if (_MANAGED == 1) || (_M_CEE == 1) //托管支持
	//表示有公共语言运行时支持
#define _CLR 1
#endif

#if _MSC_VER>=1600
#define _USE_RIGHT_VALUE_REFERENCE 1//右值引用
#endif
#if _MSC_VER>=1800
#define _FOR_LOOP_SYNTACTIC_SUGAR 1//for语法糖，C++11特性
#endif

#ifdef _WIN64
	typedef long long atomic_type;
#else
	typedef long atomic_type;
#endif // _WIN64
	typedef atomic_type index_t;
	class Void;
}