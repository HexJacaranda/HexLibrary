#pragma once
#include<chrono>
namespace HL
{
	namespace System
	{
		namespace Time
		{
			class Timer
			{
				std::chrono::steady_clock::time_point begin;
			public:
				inline void Start() {
					begin = std::chrono::steady_clock::now();
				}
				inline double Stop() {
					std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
					std::chrono::microseconds ret = std::chrono::duration_cast<std::chrono::microseconds>(end - begin);
					return double(ret.count()) / 1000;
				}
			};
		}
	}
}