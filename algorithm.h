#pragma once
namespace HL
{
	namespace System
	{
		namespace Algorithm
		{
			template<class T>
			class SortCache
			{
				void Write(void*From, void*Target) {
					Memory::Allocator::MemoryCopy(From, Target, sizeof(T), sizeof(T));
				}
				void*m_cache = nullptr;
			public:
				SortCache()
				{
					this->m_cache = Memory::Allocator::Alloc(sizeof(T));
				}
				void WriteCache(T const&Object) {
					if (m_cache)
						Memory::Allocator::MemoryCopy(&Object, this->m_cache, sizeof(T), sizeof(T));
				}
				void ReadCache(T&Target)const {
					if (m_cache)
						Memory::Allocator::MemoryCopy(this->m_cache, &Target, sizeof(T), sizeof(T));
				}
				void Swap(T&Left, T&Right)
				{
					Write(&Left, m_cache);
					Write(&Right, &Left);
					Write(m_cache, &Right);
				}
				operator T&() {
					return *(T*)m_cache;
				}
				operator T const&()const {
					return *(T*)m_cache;
				}
				~SortCache() {
					if (m_cache)
					{
						Memory::Allocator::Free(m_cache);
						m_cache = nullptr;
					}
				}
			};

			template<class U,class Functor>
			void QuickSort(U*Target, index_t low, index_t high, Functor const&compare)
			{
				if (low >= high)
					return;
				index_t first = low;
				index_t last = high;
				U Key = Target[first];/*用字表的第一个记录作为枢轴*/
				while (first < last)
				{
					while (first < last && compare(Target[last], Key) >= 0)
						--last;
					Target[first] = Target[last];/*将比第一个小的移到低端*/
					while (first < last && compare(Target[first], Key) <= 0)
						++first;
					Target[last] = Target[first];
					/*将比第一个大的移到高端*/
				}
				Target[first] = Key;/*枢轴记录到位*/
				QuickSort(Target, low, first - 1, compare);
				QuickSort(Target, first + 1, high, compare);
			}

			template<class U, class Functor>
			static void Sort(U*Array, index_t Low, index_t High, Functor Comparator) {
				QuickSort(Array, Low, High,Comparator);
			}
			template<class AnyT, class Compera>
			static index_t BinarySearch(const AnyT*Arr, size_t n, const AnyT&key, Compera comparator)
			{
				index_t mid, front = 0, back = n - 1;
				while (front <= back)
				{
					mid = (front + back) / 2;
					if (comparator(Arr[mid], key) == 0)
						return mid;
					if (comparator(Arr[mid], key) <= 0)
						front = mid + 1;
					else back = mid - 1;
				}
				return -1;
			}
			template<class AnyT>
			static index_t BinarySearch(const AnyT*Arr, size_t len, const AnyT&key)
			{
				index_t mid, front = 0, back = n - 1;
				while (front <= back)
				{
					mid = (front + back) / 2;
					if (Arr[mid] == key)
						return mid;
					if (Arr[mid] < key)
						front = mid + 1;
					else back = mid - 1;
				}
				return -1;
			}

			template<class AnyT>
			static index_t IndexOf(AnyT const*Arr, size_t length, index_t start_index, index_t end_index, AnyT const&key) {
				if (!Arr)
					return -1;
				if (end_index < start_index)
					return -1;
				if (start_index < 0 || end_index >= length)
					return -1;
				for (index_t i = start_index; i <= end_index; ++i) {
					if (Arr[i] == key)
						return i;
				}
				return -1;
			}
			template<class AnyT, class BinaryCompera>
			static index_t IndexOf(AnyT const*Arr, size_t length, index_t start_index, index_t end_index, AnyT const&key, BinaryCompera comparator) {
				if (!Arr)
					return -1;
				if (end_index < start_index)
					return -1;
				if (start_index < 0 || end_index >= length)
					return -1;
				for (index_t i = start_index; i <= end_index; ++i) {
					if (comparator(Arr[i], key) == true)
						return i;
				}
				return -1;
			}
			template<class AnyT>
			static index_t LastIndexOf(AnyT const*Arr, size_t length, index_t start_index, index_t end_index, AnyT const&key) {
				if (!Arr)
					return -1;
				if (end_index < start_index)
					return -1;
				if (start_index < 0 || end_index >= length)
					return -1;
				for (index_t i = end_index; i >= start_index; --i) {
					if (Arr[i] == key)
						return i;
				}
				return -1;
			}
			template<class AnyT, class BinaryCompera>
			static index_t LastIndexOf(AnyT const*Arr, size_t length, index_t start_index, index_t end_index, AnyT const&key, BinaryCompera comparator) {
				if (!Arr)
					return -1;
				if (end_index < start_index)
					return -1;
				if (start_index < 0 || end_index >= length)
					return -1;
				for (index_t i = end_index; i >= start_index; --i) {
					if (comparator(Arr[i], key) == true)
						return i;
				}
				return -1;
			}
			template<class AnyT, class Fx>
			static void IndexOfAll(AnyT const*Arr, size_t length, index_t start_index, index_t end_index, AnyT const&key, Fx func) {
				if (!Arr)
					return;
				if (start_index < 0 || end_index >= length || end_index < start_index)
					return;
				index_t index = start_index - 1;
				for (;;) {
					index = IndexOf(Arr, length, index + 1, end_index, key);
					if (index == -1)
						break;
					else
						func(Arr[index]);
				}
			}
			template<class AnyT, class Fx, class BinaryCompera>
			static void IndexOfAll(AnyT const*Arr, size_t length, index_t start_index, index_t end_index, AnyT const&key, BinaryCompera comparator, Fx func) {
				if (!Arr)
					return;
				if (start_index < 0 || end_index >= length || end_index < start_index)
					return;
				index_t index = start_index - 1;
				for (;;) {
					index = IndexOf(Arr, length, index + 1, end_index, key, comparator);
					if (index == -1)
						break;
					else
						func(Arr[index]);
				}
			}
			template<class AnyT, class Fx>
			static void LastIndexOfAll(AnyT const*Arr, size_t length, index_t start_index, index_t end_index, AnyT const&key, Fx func) {
				if (!Arr)
					return;
				if (start_index < 0 || end_index >= length || end_index < start_index)
					return;
				index_t index = end_index + 1;
				for (;;) {
					index = LastIndexOf(Arr, length, 0, index - 1, key);
					if (index == -1)
						break;
					else
						func(Arr[index]);
				}
			}
			template<class AnyT, class Fx, class BinaryCompera>
			static void LastIndexOfAll(AnyT const*Arr, size_t length, index_t start_index, index_t end_index, AnyT const&key, BinaryCompera comparator, Fx func) {
				if (!Arr)
					return;
				if (start_index < 0 || end_index >= length || end_index < start_index)
					return;
				index_t index = end_index + 1;
				for (;;) {
					index = LastIndexOf(Arr, length, 0, index - 1, key, comparator);
					if (index == -1)
						break;
					else
						func(Arr[index]);
				}
			}
			template<class IterT, class Fx>
			void Foreach(IterT begin, IterT end, Fx func) {
				for (IterT iter = begin; iter != end; ++iter) {
					func(*iter);
				}
			}

			template<class U,class T,class...Args>
			static bool Any(U const&Target, T const&First, Args const&...Rest) {
				if (Target == First)
					return true;
				return Any(Target, Rest...);
			}
			template<class U, class T, class...Args>
			static bool Any(U const&Target, T const&First) {
				return Target == First;
			}

			template<class CharT>
			bool BasicStringSingleMatch(const CharT*target_string, const CharT* source, size_t target_length) {
				//预判
				if (target_string[0] == source[0] && target_string[target_length - 1] == source[target_length - 1]) {
					for (index_t i = 1; i < target_length - 1; ++i) {
						if (target_string[i] != source[i])
							return false;
					}
					return true;
				}
				return false;
			}
			template<class CharT>
			bool BasicStringEquals(const CharT*target_string, const CharT* source, size_t target_length, size_t source_length) {
				if (target_length != source_length)
					return false;
				//预判
				if (target_string[0] == source[0] && target_string[target_length - 1] == source[target_length - 1]) {
					for (index_t i = 1; i < target_length - 1; ++i) {
						if (target_string[i] != source[i])
							return false;
					}
					return true;
				}
				return false;
			}
			template<class CharT>
			index_t BasicStringIndexOf(const CharT*target_string, const CharT* source, size_t target_length, size_t source_length, index_t start_index, index_t end_index) {
				//预判处理
				if (!target_string || !source)
					return -1;
				if (start_index > end_index)
					return -1;
				if (start_index < 0 && end_index >= source_length)
					return -1;
				if (end_index - start_index < target_length)
					return -1;

				index_t current_index = start_index;
				for (;;) {
					if (current_index + target_length > end_index)
						return -1;
					if (target_string[0] == source[current_index])
					{
						bool ret = BasicStringSingleMatch(target_string, source + current_index, target_length);
						if (ret)
							return current_index;
					}
					++current_index;
				}
			}
			template<class CharT>
			index_t BasicStringLastIndexOf(const CharT*target_string, const CharT* source, size_t target_length, size_t source_length, index_t start_index, index_t end_index) {
				//预判处理
				if (!target_string || !source)
					return -1;
				if (start_index > end_index)
					return -1;
				if (start_index < 0 && end_index >= source_length)
					return -1;
				if (end_index - start_index < target_length)
					return -1;

				index_t current_index = end_index;
				for (;;) {
					if (current_index - target_length < start_index)
						return -1;
					if (target_string[target_length - 1] == source[current_index])
					{
						bool ret = BasicStringSingleMatch(target_string, source + current_index - target_length + 1, target_length);
						if (ret)
							return (current_index - target_length + 1);
					}
					--current_index;
				}
			}
			template<class CharT>
			static void BasicStringIndexOfAll(const CharT*target_string, const CharT*source, Memory::MemoryManager<index_t>&Out, size_t target_length, size_t source_length, index_t start_index, index_t end_index) {
				if (!target_string || !source)
					return;
				if (start_index > end_index)
					return;
				if (start_index < 0 && end_index >= source_length)
					return;
				if (end_index - start_index < target_length)
					return;

				index_t current_index = start_index;
				for (;;)
				{
					index_t ret = BasicStringIndexOf(target_string, source, target_length, source_length, current_index, end_index);
					if (ret == -1)
						break;
					else
					{
						current_index = ret + target_length;
						Out.Append(ret);
					}
				}
			}
			template<class CharT>
			index_t BasicStringIndexOfAny(const CharT*target_string, const CharT* source, size_t target_length, size_t source_length, index_t start_index, index_t end_index) {
				index_t current_index = start_index;
				for (;;) {
					for (index_t i = 0; i < target_length; i++)
						if (target_string[i] == source[current_index + i])
							return current_index + i;
					++source;
				}
			}
			template<class CharT>
			size_t BasicStringLength(const CharT*target_string) {
				size_t i = 0;
				for (;;) {
					if (target_string[i] == (CharT)'\0')
						return i;
					++i;
				}
			}
		}
	}
}