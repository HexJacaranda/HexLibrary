#pragma once
#include <string.h>
#include <stdlib.h>
#pragma warning(disable:4311)
#pragma warning(disable:4302)
namespace HL
{
	namespace System
	{
		namespace Memory
		{
			class Allocator
			{
				static void* internal_alloc(size_t size) {
					void* ret = malloc(size);
					if (ret == nullptr)
						HL::Exception::Throw<HL::Exception::BadAllocateException>(size);
					return ret;
				} 
				static void internal_free(void*ptr) {
					free(ptr);
				}
				static void Memmove(unsigned char* dest, const unsigned char* src, size_t _len) {
					size_t len = _len;
					if ((size_t)(dest - src) >= len)
					{
						switch (len)
						{
						case 0u:
							return;
						case 1u:
							*dest = *src;
							return;
						case 2u:
							*(short*)dest = *(short*)src;
							return;
						case 3u:
							*(short*)dest = *(short*)src;
							dest[2] = *(src + 2);
							return;
						case 4u:
							*(int*)dest = *(int*)src;
							return;
						case 5u:
							*(int*)dest = *(int*)src;
							dest[4] = *(src + 4);
							return;
						case 6u:
							*(int*)dest = *(int*)src;
							*(short*)(dest + 4) = *(short*)(src + 4);
							return;
						case 7u:
							*(int*)dest = *(int*)src;
							*(short*)(dest + 4) = *(short*)(src + 4);
							dest[6] = *(src + 6);
							return;
						case 8u:
							*(int*)dest = *(int*)src;
							*(int*)(dest + 4) = *(int*)(src + 4);
							return;
						case 9u:
							*(int*)dest = *(int*)src;
							*(int*)(dest + 4) = *(int*)(src + 4);
							dest[8] = *(src + 8);
							return;
						case 10u:
							*(int*)dest = *(int*)src;
							*(int*)(dest + 4) = *(int*)(src + 4);
							*(short*)(dest + 8) = *(short*)(src + 8);
							return;
						case 11u:
							*(int*)dest = *(int*)src;
							*(int*)(dest + 4) = *(int*)(src + 4);
							*(short*)(dest + 8) = *(short*)(src + 8);
							dest[10] = *(src + 10);
							return;
						case 12u:
							*(int*)dest = *(int*)src;
							*(int*)(dest + 4) = *(int*)(src + 4);
							*(int*)(dest + 8) = *(int*)(src + 8);
							return;
						case 13u:
							*(int*)dest = *(int*)src;
							*(int*)(dest + 4) = *(int*)(src + 4);
							*(int*)(dest + 8) = *(int*)(src + 8);
							dest[12] = *(src + 12);
							return;
						case 14u:
							*(int*)dest = *(int*)src;
							*(int*)(dest + 4) = *(int*)(src + 4);
							*(int*)(dest + 8) = *(int*)(src + 8);
							*(short*)(dest + 12) = *(short*)(src + 12);
							return;
						case 15u:
							*(int*)dest = *(int*)src;
							*(int*)(dest + 4) = *(int*)(src + 4);
							*(int*)(dest + 8) = *(int*)(src + 8);
							*(short*)(dest + 12) = *(short*)(src + 12);
							dest[14] = *(src + 14);
							return;
						case 16u:
							*(int*)dest = *(int*)src;
							*(int*)(dest + 4) = *(int*)(src + 4);
							*(int*)(dest + 8) = *(int*)(src + 8);
							*(int*)(dest + 12) = *(int*)(src + 12);
							return;
						default:
							if (len < 512u)
							{
								if (((int)dest & 3) != 0)
								{
									if (((int)dest & 1) != 0)
									{
										*dest = *src;
										src += 1;
										dest += 1;
										len -= 1u;
										if (((int)dest & 2) == 0)
										{
											goto IL_218;
										}
									}
									*(short*)dest = *(short*)src;
									src += 2 / 1;
									dest += 2 / 1;
									len -= 2u;
								}
							IL_218:
								for (size_t num = len / 16u; num > 0u; num -= 1u)
								{
									*(int*)dest = *(int*)src;
									*(int*)(dest + 4) = *(int*)(src + 4);
									*(int*)(dest + 8) = *(int*)(src + 8);
									*(int*)(dest + 12) = *(int*)(src + 12);
									dest += 16;
									src += 16;
								}
								if ((len & 8u) != 0u)
								{
									*(int*)dest = *(int*)src;
									*(int*)(dest + 4) = *(int*)(src + 4);
									dest += 8;
									src += 8;
								}
								if ((len & 4u) != 0u)
								{
									*(int*)dest = *(int*)src;
									dest += 4;
									src += 4;
								}
								if ((len & 2u) != 0u)
								{
									*(short*)dest = *(short*)src;
									dest += 2;
									src += 2;
								}
								if ((len & 1u) != 0u)
								{
									*dest = *src;
								}
								return;
							}
							break;
						}
					}
					memmove(dest, src, len);
				}
			public:
				static inline void* Alloc(size_t size) {
					return internal_alloc(size);
				}
				static inline void Free(void*target) {
					internal_free(target);
				}
				static void* AllocBlank(size_t size) {
					void*ret = internal_alloc(size);
					SetBlank(ret, (unsigned char*)ret + size);
					return ret;
				}
				static void SetBlank(void*start, void*end) {
					if (start == end)
						return;
					unsigned char*bstart = (unsigned char*)start;
					unsigned char*bend = (unsigned char*)end;
					do {
						*bstart = 0;
					} while (++bstart != bend);
				}
				static void* ReAlloc(void*target, size_t size) {
					if (target)
						internal_free(target);
					return internal_alloc(size);
				}
				static inline void* ReAllocTransfer(void*target, size_t size, size_t transfer_size) {
					void*ret = internal_alloc(size);
					MemoryCopy(target, ret, size, transfer_size <= size ? transfer_size : size);
					internal_free(target);
					return ret;
				}
				//source指示拷贝源，destination是待写入数据块，destinationsizeinbytes指示待写入数据源的大小(byte计)，sourcebytestocopy指示要拷贝的个数
				static void MemoryCopy(const void* source, void* destination, size_t destinationSizeInBytes, size_t sourceBytesToCopy) {
					if (sourceBytesToCopy == 0)
						return;
					Memmove((unsigned char*)destination, (unsigned char*)source, sourceBytesToCopy);
				}
			};

		}
	}
}