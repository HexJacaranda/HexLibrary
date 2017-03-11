#pragma once
namespace HL
{
	namespace System
	{
		namespace Values
		{
			class MinValues
			{
			public:
				//获取T最小值
				template <class T>
				inline static T Of(T* p = nullptr)
				{
					if (Template::IsSame<T, float>::R)return (T)Float();
					if (Template::IsSame<T, double>::R)return (T)Double();
					if (Template::IsSame<T, long double>::R)return (T)LongDouble();
					if ((T)-1 > (T)0)return (T)0;
					return (T)(1ull << (sizeof(T) * 8 - 1));
				}
				static const char Char = -128;
				/**
				@brief wchar_t型变量最小值*/
				static const wchar_t WChar = 0;
				/**
				@brief signed char型变量最小值*/
				static const signed char SignedChar = -128;
				/**
				@brief unsigned char型变量最小值*/
				static const unsigned char UnsignedChar = 0;
				/**
				@brief short型变量最小值*/
				static const short Short = -32768;
				/**
				@brief short型变量最小值*/
				static const short Int16 = -32768;
				/**
				@brief unsigned short型变量最小值*/
				static const unsigned short UnsignedShort = 0;
				/**
				@brief unsigned short型变量最小值*/
				static const unsigned short UInt16 = 0;
				/**
				@brief int型变量最小值*/
				static const int Int = (int)0x80000000;
				/**
				@brief int型变量最小值*/
				static const int Int32 = (int)0x80000000;
				/**
				@brief unsigned int型变量最小值*/
				static const unsigned int UnsignedInt = 0;
				/**
				@brief unsigned int型变量最小值*/
				static const unsigned int UInt32 = 0;
				/**
				@brief long型变量最小值*/
				static const long Long = (long)0x80000000;
				/**
				@brief unsigned long型变量最小值*/
				static const unsigned long UnsignedLong = 0;
				/**
				@brief long long型变量最小值*/
				static const long long LongLong = (long long)0x8000000000000000;
				/**
				@brief long long型变量最小值*/
				static const long long Int64 = (long long)0x8000000000000000;
				/**
				@brief unsigned long long型变量最小值*/
				static const unsigned long long UnsignedLongLong = 0;
				/**
				@brief unsigned long long型变量最小值*/
				static const unsigned long long UInt64 = 0;
				/**
				@brief float型变量最小值*/
				static const float Float() {
					return 1.17549e-038;
				}
				/**
				@brief double型变量最小值*/
				static const double Double() {
					return 2.22507e-308;
				}
				/**
				@brief long double型变量最小值*/
				static const long double LongDouble() {
					return 3.3621e-4932;
				}
				/**
				@brief long double型变量绝对值最小值*/
				static const long double LongDoubleAbs;
				/**
				@brief float型变量绝对值最小值*/
				static const float FloatAbs;
				/**
				@brief double型变量绝对值最小值*/
				static const double DoubleAbs;
			};
			class MaxValues
			{
			public:
				template <class T>inline static T Of(T* p = nullptr)
				{
					if (Template::IsSame<T, float>::R)return (T)Float();
					if (Template::IsSame<T, double>::R)return (T)Double();
					if (Template::IsSame<T, long double>::R)return (T)LongDouble();
					if ((T)-1 > (T)0)return (T)-1;
					return (T)(((unsigned long long) - 1) >> (64 - sizeof(T) * 8 + 1));
				}
				/**
				@brief char型变量最大值*/
				static const char Char = (char)-1>(char)0 ? 255 : 127;
				/**
				@brief wchar_t型变量最大值*/
				static const wchar_t WChar = 0xffff;
				/**
				@brief signed char型变量最大值*/
				static const signed char SignedChar = 127;
				/**
				@brief unsigned char型变量最大值*/
				static const unsigned char UnsignedChar = 0xff;
				/**
				@brief short型变量最大值*/
				static const short Short = 0x7fff;
				/**
				@brief short型变量最大值*/
				static const short Int16 = 0x7fff;
				/**
				@brief unsigned short型变量最大值*/
				static const unsigned short UnsignedShort = 0xffff;
				/**
				@brief unsigned short型变量最大值*/
				static const unsigned short UInt16 = 0xffff;
				/**
				@brief int型变量最大值*/
				static const int Int = 0x7fffffff;
				/**
				@brief int型变量最大值*/
				static const int Int32 = 0x7fffffff;
				/**
				@brief unsigned int型变量最大值*/
				static const unsigned int UnsignedInt = 0xffffffff;
				/**
				@brief unsigned int型变量最大值*/
				static const unsigned int UInt32 = 0xffffffff;
				/**
				@brief long型变量最大值*/
				static const long Long = 0x7fffffff;
				/**
				@brief unsigned long型变量最大值*/
				static const unsigned long UnsignedLong = 0xffffffff;
				/**
				@brief long long型变量最大值*/
				static const long long LongLong = 0x7fffffffffffffff;
				/**
				@brief long long型变量最大值*/
				static const long long Int64 = 0x7fffffffffffffff;
				/**
				@brief unsigned long long型变量最大值*/
				static const unsigned long long UnsignedLongLong = 0xffffffffffffffff;
				/**
				@brief unsigned long long型变量最大值*/
				static const unsigned long long UInt64 = 0xffffffffffffffff;
				/**
				@brief float型变量最大值*/
				const static float Float() {
					return 3.40282e+038;
				}
				/**
				@brief double型变量最大值*/
				const static double Double() {
					return 1.79769e+308;
				}
				/**
				@brief long double型变量最大值*/
				const static long double LongDouble() {
					return 1.0e+300;
				}
				/**
				@brief float型变量最大值*/
				const static float FloatAbs;
				/**
				@brief double型变量最大值*/
				const static double DoubleAbs;
			};
		}
	}
}