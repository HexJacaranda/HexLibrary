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
				//��ȡT��Сֵ
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
				@brief wchar_t�ͱ�����Сֵ*/
				static const wchar_t WChar = 0;
				/**
				@brief signed char�ͱ�����Сֵ*/
				static const signed char SignedChar = -128;
				/**
				@brief unsigned char�ͱ�����Сֵ*/
				static const unsigned char UnsignedChar = 0;
				/**
				@brief short�ͱ�����Сֵ*/
				static const short Short = -32768;
				/**
				@brief short�ͱ�����Сֵ*/
				static const short Int16 = -32768;
				/**
				@brief unsigned short�ͱ�����Сֵ*/
				static const unsigned short UnsignedShort = 0;
				/**
				@brief unsigned short�ͱ�����Сֵ*/
				static const unsigned short UInt16 = 0;
				/**
				@brief int�ͱ�����Сֵ*/
				static const int Int = (int)0x80000000;
				/**
				@brief int�ͱ�����Сֵ*/
				static const int Int32 = (int)0x80000000;
				/**
				@brief unsigned int�ͱ�����Сֵ*/
				static const unsigned int UnsignedInt = 0;
				/**
				@brief unsigned int�ͱ�����Сֵ*/
				static const unsigned int UInt32 = 0;
				/**
				@brief long�ͱ�����Сֵ*/
				static const long Long = (long)0x80000000;
				/**
				@brief unsigned long�ͱ�����Сֵ*/
				static const unsigned long UnsignedLong = 0;
				/**
				@brief long long�ͱ�����Сֵ*/
				static const long long LongLong = (long long)0x8000000000000000;
				/**
				@brief long long�ͱ�����Сֵ*/
				static const long long Int64 = (long long)0x8000000000000000;
				/**
				@brief unsigned long long�ͱ�����Сֵ*/
				static const unsigned long long UnsignedLongLong = 0;
				/**
				@brief unsigned long long�ͱ�����Сֵ*/
				static const unsigned long long UInt64 = 0;
				/**
				@brief float�ͱ�����Сֵ*/
				static const float Float() {
					return 1.17549e-038;
				}
				/**
				@brief double�ͱ�����Сֵ*/
				static const double Double() {
					return 2.22507e-308;
				}
				/**
				@brief long double�ͱ�����Сֵ*/
				static const long double LongDouble() {
					return 3.3621e-4932;
				}
				/**
				@brief long double�ͱ�������ֵ��Сֵ*/
				static const long double LongDoubleAbs;
				/**
				@brief float�ͱ�������ֵ��Сֵ*/
				static const float FloatAbs;
				/**
				@brief double�ͱ�������ֵ��Сֵ*/
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
				@brief char�ͱ������ֵ*/
				static const char Char = (char)-1>(char)0 ? 255 : 127;
				/**
				@brief wchar_t�ͱ������ֵ*/
				static const wchar_t WChar = 0xffff;
				/**
				@brief signed char�ͱ������ֵ*/
				static const signed char SignedChar = 127;
				/**
				@brief unsigned char�ͱ������ֵ*/
				static const unsigned char UnsignedChar = 0xff;
				/**
				@brief short�ͱ������ֵ*/
				static const short Short = 0x7fff;
				/**
				@brief short�ͱ������ֵ*/
				static const short Int16 = 0x7fff;
				/**
				@brief unsigned short�ͱ������ֵ*/
				static const unsigned short UnsignedShort = 0xffff;
				/**
				@brief unsigned short�ͱ������ֵ*/
				static const unsigned short UInt16 = 0xffff;
				/**
				@brief int�ͱ������ֵ*/
				static const int Int = 0x7fffffff;
				/**
				@brief int�ͱ������ֵ*/
				static const int Int32 = 0x7fffffff;
				/**
				@brief unsigned int�ͱ������ֵ*/
				static const unsigned int UnsignedInt = 0xffffffff;
				/**
				@brief unsigned int�ͱ������ֵ*/
				static const unsigned int UInt32 = 0xffffffff;
				/**
				@brief long�ͱ������ֵ*/
				static const long Long = 0x7fffffff;
				/**
				@brief unsigned long�ͱ������ֵ*/
				static const unsigned long UnsignedLong = 0xffffffff;
				/**
				@brief long long�ͱ������ֵ*/
				static const long long LongLong = 0x7fffffffffffffff;
				/**
				@brief long long�ͱ������ֵ*/
				static const long long Int64 = 0x7fffffffffffffff;
				/**
				@brief unsigned long long�ͱ������ֵ*/
				static const unsigned long long UnsignedLongLong = 0xffffffffffffffff;
				/**
				@brief unsigned long long�ͱ������ֵ*/
				static const unsigned long long UInt64 = 0xffffffffffffffff;
				/**
				@brief float�ͱ������ֵ*/
				const static float Float() {
					return 3.40282e+038;
				}
				/**
				@brief double�ͱ������ֵ*/
				const static double Double() {
					return 1.79769e+308;
				}
				/**
				@brief long double�ͱ������ֵ*/
				const static long double LongDouble() {
					return 1.0e+300;
				}
				/**
				@brief float�ͱ������ֵ*/
				const static float FloatAbs;
				/**
				@brief double�ͱ������ֵ*/
				const static double DoubleAbs;
			};
		}
	}
}