#pragma once
namespace HL
{
	namespace System
	{
		namespace Hash
		{
			class Hash
			{
			public:
				template<class T>
				inline static size_t HashSeq(const T *object_ptr)
				{
					const unsigned char *_First = (const unsigned char *)object_ptr;
#if defined(_WIN64)
					const size_t _FNV_offset_basis = 14695981039346656037ULL;
					const size_t _FNV_prime = 1099511628211ULL;

#else
					const size_t _FNV_offset_basis = 2166136261U;
					const size_t _FNV_prime = 16777619U;
#endif 
					size_t _Val = _FNV_offset_basis;
					for (size_t _Next = 0; _Next < sizeof(T); ++_Next)
					{
						_Val ^= (size_t)_First[_Next];
						_Val *= _FNV_prime;
					}
					return (_Val);
				}
				template<class T>
				inline static size_t HashSeq(const T *object_ptr, size_t size)
				{
					const unsigned char *_First = (const unsigned char *)object_ptr;
#if defined(_WIN64)
					const size_t _FNV_offset_basis = 14695981039346656037ULL;
					const size_t _FNV_prime = 1099511628211ULL;

#else
					const size_t _FNV_offset_basis = 2166136261U;
					const size_t _FNV_prime = 16777619U;
#endif 
					size_t _Val = _FNV_offset_basis;
					for (size_t _Next = 0; _Next < size * sizeof(T); ++_Next)
					{
						_Val ^= (size_t)_First[_Next];
						_Val *= _FNV_prime;
					}
					return (_Val);
				}
			};
			//对象Hash码计算支持接口
			class ISupportHash
			{
			public:
				//获取对象HashCode
				virtual size_t GetHashCode()const = 0;
				virtual ~ISupportHash() {}
			};
		}
	}
}