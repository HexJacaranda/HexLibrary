#pragma once
#include <sstream>
namespace HL
{
	namespace System
	{
		//ʵ��String������
		namespace StringFunction
		{
			template<class CharT>
			static inline bool IsInt(CharT const&target_char) {
				if (target_char <= (CharT)'9'&&target_char >= (CharT)'0')
					return true;
				else
					return false;
			}
			//�Ƿ�ΪA-Z,a-z�ַ�
			template<class CharT>
			static inline bool IsChar(CharT const&target_char){
				return (target_char >= (CharT)'A'&&target_char <= (CharT)'Z') || (target_char >= (CharT)'a'&&target_char <= (CharT)'z');
			}
			template<class CharT>
			static size_t UIntSniff(CharT const*string) {
				size_t out = 0;
				while (IsInt(*string))
				{
					++string;
					++out;
				}
				return out;
			}
			template<class IntT, class CharT>
			static IntT CharToInt(CharT const& target_char) {
				if (IsInt(target_char))
					return (IntT)(target_char - (CharT)'0');
				else
					return (IntT)0;
			}
			template<class IntT,class CharT>
			static IntT StringToInt(CharT const*string, size_t count) {
				IntT ret_value = 0;
				int sign = 1;
				if (string[0] == (CharT)'-')
					sign = -1;

				size_t iter = 0;
				if (sign < 0)
					iter = 1;

				for (size_t i=iter; i < count; i++)
				{
					if (IsInt(string[i]))
						ret_value = ret_value * 10 + (IntT)(string[i] - (CharT)'0');
					else
						return ret_value*sign;
				}
				return ret_value*sign;
			}
			template<class FloatT,class CharT>
			static FloatT StringToFloat(CharT const*string, size_t count) {
				int sign = 1;
				if (string[0] == (CharT)'-')
					sign = -1;
				FloatT int_part = 0;
				FloatT float_part = 0;
				FloatT pow_iter = 10;
				bool dot_sign = false;
				size_t iter = 0;
				if (sign < 0)
					iter = 1;
				for (size_t i=iter;i < count; ++i) 
				{
					if (string[i] == (CharT)'.')
					{
						dot_sign = true;
						continue;
					}
					if (IsInt(string[i]))
					{
						if (!dot_sign)
							int_part = int_part * 10 + (FloatT)(string[i] - (CharT)'0');
						else
						{
							float_part += (FloatT)(string[i] - (CharT)'0') / pow_iter;
							pow_iter *= 10;
						}
					}
					else
						return (int_part + float_part)*sign;
				}
				return (int_part + float_part)*sign;
			}
			template<class FloatT,class CharT>
			static void FloatToString(Memory::MemoryManager<CharT> &out, FloatT const&object, size_t precision ,bool no_precision) {
				long int_part = (long)object;
				bool sign = int_part < 0;
				FloatT float_part = object - int_part;
				if (int_part < 0)
				{
					out.Append((CharT)'-');
					sign = true;
				}
				//��������
				int temp = 0;
				for (;;)
				{
					temp = int_part % 10;
					out.Append(temp + (CharT)'0');
					int_part /= 10;
					if (int_part == 0)
						break;
				}
				//��ת����

				size_t iter = 0;
				if (sign)
					++iter;
				for (size_t i=iter; i < out.GetUsedSize()/2; ++i)
				{
					CharT temp_char = out[i];
					out[i] = out[out.GetUsedSize() - 1 - i];
					out[out.GetUsedSize() - 1 - i] = temp_char;
				}

				if (precision != 0 && no_precision == false)
					out.Append((CharT)'.');
				//С������
				int float_temp = 0;
				if (no_precision)
				{
					//�Զ�����
					for (;;) {
						float_part *= 10;
						float_temp = (int)float_part;
						if (float_temp == 0)
							break;
						out.Append(float_temp + (CharT)'0');
						float_part -= float_temp;
					}
					if (out[out.GetUsedSize() - 1] == (CharT)'.')
						out.SetUsed(out.GetUsedSize() - 1);
				}
				else
				{
					//������
					for (size_t i = 0; i < precision; ++i)
					{
						float_part *= 10;
						float_temp = (int)float_part;
						out.Append(float_temp + (CharT)'0');
						float_part -= float_temp;
					}
				}
			}
			template<class IntT,class CharT>
			static void IntToString(Memory::MemoryManager<CharT> &out, IntT const&object, size_t radix) {
				bool sign = object < 0;
				if (sign)
					out.Append((CharT)'-');
				IntT val = object;
				size_t digval = 0;
				for (;;)
				{
					digval = (size_t)val % radix;
					val /= radix;
					if (digval > 9)
						out.Append((CharT)(digval - 10 + (CharT)'A'));
					else
						out.Append((CharT)(digval + (CharT)'0'));
					if (val <= 0)
						break;
				}
				size_t iter = 0;
				if (sign)
					++iter;
				for (size_t i = iter; i < out.GetUsedSize() / 2; ++i)
				{
					CharT temp_char = out[i];
					out[i] = out[out.GetUsedSize() - 1 - i];
					out[out.GetUsedSize() - 1 - i] = temp_char;
				}
			}
			//һֱ��ǰ�ƶ�,ֱ������ָ���Ľ�����
			template<class CharT>
			static CharT const* GoUntil(CharT const*string, CharT endchar)
			{
				for (;*string != endchar; string++){}
				return string;
			}
			//һֱ��ǰ�ƶ�,ֱ������ָ���Ľ�����
			template<class CharT, unsigned size>
			static CharT const* GoUntil(CharT const*string, CharT(&chars)[size]) {
				for (;; string++) {
					for (unsigned i = 0; i < size; ++i) {
						if (chars[i] == *string)
							return string;
					}
				}
				return string;
			}
			//����ָ���ķ�����ǰ��
			template<class CharT,unsigned size>
			static CharT const* GoNotUntil(CharT const*string, CharT(&chars)[size]) {
				bool flag = false;
				for (;; string++) {
					for (unsigned i = 0; i < size; ++i) {
						if (chars[i] == *string)
						{
							flag = true;
							break;
						}
					}
					if (!flag)
						return string;
					else
						flag = false;
				}
				return string;
			}
			//����ָ���ķ�����ǰ��
			template<class CharT>
			static CharT const* GoNotUntil(CharT const*string, CharT target_char) {
				for (; *string == target_char; string++) {}
				return string;
			}

		}

		//�ַ�����ʽ���ӿ�
		template<class InT,class CharT>
		class StringFormat;

		namespace Internal
		{
			template<class CharT>
			struct BasicStringUpperLower
			{
				inline static bool IsLower(CharT c) {
					if ((c >= (CharT)'a') && (c <= (CharT)'z'))
						return true;
					return false;
				}
				inline static bool IsUpper(CharT c) {
					if ((c >= (CharT)'A') && (c <= (CharT)'Z'))
						return true;
					return false;
				}
				static CharT ToUpper(CharT c) {
					if ((c >= (CharT)'a') && (c <= (CharT)'z'))
						c = c - ((CharT)'a' - (CharT)'A');
					return c;
				}
				static CharT ToLower(CharT c) {
					if ((c >= (CharT)'A') && (c <= (CharT)'Z'))
						c = c + ((CharT)'a' - (CharT)'A');
					return c;
				}
				static void ToUpper(CharT *string) {
					while (*string != (CharT)'\0')
					{
						*string = ToUpper(*string);
						++string;
					}
				}
				static void ToLower(CharT *string) {
					while (*string != (CharT)'\0')
					{
						*string = ToLower(*string);
						++string;
					}
				}
			};

			template<class T>
			class FormatPattern {
			public:
				static void Match(const T* string, size_t count, Memory::MemoryManager<Container::Tuple<index_t, index_t, index_t,size_t>>&out) {
					bool find_ending = false;
					const T* last_beginning = string;
					const T* str_ptr = string;
					while (*str_ptr != (T)'\0') {
						if (str_ptr - string + 1 > count)
							return;
						if (*str_ptr == (T)'{') {
							last_beginning = str_ptr;
							find_ending = true;
						}
						else
						{
							if (*str_ptr == (T)'}') {
								size_t length = StringFunction::UIntSniff(last_beginning + 1);
								if (str_ptr - last_beginning - 1 == length)//�м����κβ��� 
								{
									index_t number = StringFunction::StringToInt<index_t>(last_beginning + 1, length);
									out.Append({ last_beginning - string,number,str_ptr - string,length + 1 });
								}
								else
								{
									if (*(last_beginning + length + 1) == (T)':')//��֤�ǲ���
									{
										index_t number = StringFunction::StringToInt<index_t>(last_beginning + 1, length);
										out.Append({ last_beginning - string,number,str_ptr - string,length + 1 });
									}
								}
								find_ending = false;
							}
						}
						++str_ptr;
					}
				}
			};
		}

		template<class CharT>
		class BasicString:public System::Hash::ISupportHash,public System::Interface::ICloneable
		{
			typedef Memory::MemoryManager<CharT> ContainerT;
			ContainerT data;
		private:
			//��ʼд��
			void BeginWrite() {
				index_t inx = (index_t)data.GetUsedSize() - 1;
				if (inx >= 0)
					if (data[inx] == (CharT)'\0')
						data.SetUsed(inx >= 0 ? inx : 0);
			}
			//����д��
			void EndWrite() {
				index_t inx = (index_t)data.GetUsedSize() - 1;
				if (inx >= 0)
					if (data[inx] != (CharT)'\0')
						data.Append((CharT)'\0');
			}
		private:
			void ReplaceInternal(const CharT*Old, const CharT* New, index_t StartInx, index_t EndInx) {
				if (!Old || !New)
					return;
				if (EndInx < StartInx)
					return;
				if (StartInx < 0 && EndInx >= this->Count())
					return;
				size_t old_length = Algorithm::BasicStringLength(Old);
				size_t new_length = Algorithm::BasicStringLength(New);
				index_t differ = new_length - old_length;
				Memory::MemoryManager<index_t> outer;
				Internal::BasicStringIndexOfAll(Old, this->data.GetMemoryBlock(), outer, old_length, this->Count(), StartInx, EndInx);
				if (outer.GetUsedSize() > 0)//���������ַ�
				{
					if (differ > 0)//���ַ�����ԭ�ַ�����
					{
						this->data.Expand(differ*outer.GetUsedSize());
						for (index_t i = 0; i < outer.GetUsedSize(); ++i)
						{
							this->data.MoveBackward(outer[i] + differ*i, differ);//����ƶ���ֵ��λ
							Memory::Allocator::MemoryCopy(New, this->data.GetMemoryBlock() + outer[i] + differ*i, 0, sizeof(CharT)*new_length);//�����ַ�������
						}
					}
					else if (differ < 0)
					{
						for (index_t i = 0; i < outer.GetUsedSize(); ++i)
						{
							Memory::Allocator::MemoryCopy(New, this->data.GetMemoryBlock() + outer[i] + differ*i, 0, sizeof(CharT)*new_length);//�����ַ�������
							Memory::Allocator::MemoryCopy(this->data.GetMemoryBlock() + outer[i] + differ*i + old_length, this->data.GetMemoryBlock() + outer[i] + differ*i + new_length, 0, sizeof(CharT)*(this->data.GetUsedSize() - (outer[i] + differ*i)));//��ǰ�ƶ�
						}
					}
					else//differ==0
					{
						for (index_t i = 0; i < outer.GetUsedSize(); ++i)
							Memory::Allocator::MemoryCopy(New, this->data.GetMemoryBlock() + outer[i], 0, sizeof(CharT)*new_length);//�����ַ�������
					}

				}
			}
			template<class...Args>
			static void FormatInternal(const CharT*format, ContainerT*out, Args const&...args);
		public:
			BasicString():data(0) {}
			BasicString(size_t length) :data(length) {}
			BasicString(BasicString const&rhs) :data(rhs.data) {}
			BasicString(ContainerT const&rhs) :data(rhs) {
				EndWrite();
			}
			BasicString(ContainerT &&lhs) :data(static_cast<ContainerT&&>(lhs)) {
				EndWrite();
			}
			BasicString(System::Generic::Array<CharT> const&rhs) {
				this->data.Append(rhs.GetData(), rhs.Count());
				EndWrite();
			}
			BasicString(BasicString&& lhs) :data(static_cast<ContainerT&&>(lhs.data)) {}
		    BasicString(const CharT*string) {
				if (string)
				{
					this->BeginWrite();
					this->data.Append(string, Algorithm::BasicStringLength(string));
					this->EndWrite();
				}
			}
			BasicString&operator=(BasicString const&rhs) {
				this->data.ReWrite(rhs.data.GetMemoryBlock(), rhs.data.GetUsedSize());
				this->EndWrite();
				return *this;
			}
			BasicString&operator=(BasicString &&lhs) {
				this->data = static_cast<ContainerT&&>(lhs.data);
				this->EndWrite();
				return *this;
			}
			BasicString&operator=(const CharT*string) {
				if (string)
				{
					this->data.ReWrite(string, Algorithm::BasicStringLength(string));
					this->EndWrite();
				}
				return *this;
			}
			BasicString&operator+=(BasicString const&rhs) {
				this->Append(rhs);
				return *this;
			}
			BasicString&operator+=(const CharT*string) {
				this->Append(string);
				return *this;
			}
			BasicString operator+(BasicString const&rhs)const {
				BasicString ret(*this);
				ret.Append(rhs);
				return ret;
			}
			BasicString operator+(const CharT* string)const {
				BasicString ret(*this);
				ret.Append(string);
				return ret;
			}
			bool operator==(BasicString const&rhs)const {
				return Algorithm::BasicStringEquals(this->data.GetMemoryBlock(), rhs.data.GetMemoryBlock(), this->data.GetUsedSize() - 1, rhs.data.GetUsedSize() - 1);
			}
			bool operator==(const CharT* string)const {
				return Algorithm::BasicStringEquals(this->data.GetMemoryBlock(), string, this->data.GetUsedSize() - 1, Algorithm::BasicStringLength(string));
			}
			//׷���ַ���
			void Append(BasicString const&rhs) {
				this->BeginWrite();
				this->data.Append(rhs.GetData(), rhs.data.GetUsedSize());
				this->EndWrite();
			}
			//׷���ַ���
			void Append(const CharT*string) {
				if (string)
				{
					this->BeginWrite();
					this->data.Append(string, Algorithm::BasicStringLength(string));
					this->EndWrite();
				}
			}
			//׷�Ӵ�index��ʼcount���ַ���
			void Append(const CharT*string, index_t index, size_t count) {
				if (string)
				{
					if (index >= 0)
					{
						this->BeginWrite();
						this->data.Append(string + index, count);
						this->EndWrite();
					}
				}
			}
			//׷���ַ���
			void Append(ContainerT const&rhs) {
				this->BeginWrite();
				this->data.Append(rhs);
				this->EndWrite();
			}
			//׷���ַ���
			void Append(ContainerT const&rhs, index_t index, size_t count) {
				if (index >= 0 && index + count < rhs.GetUsedSize())
				{
					this->BeginWrite();
					this->data.Append(rhs.GetMemoryBlock() + index, count);
					this->EndWrite();
				}
			}
			//׷���ַ���
			void Append(System::Generic::Array<CharT> const&rhs) {
				this->BeginWrite();
				this->data.Append(rhs.GetData(), rhs.Count());
				this->EndWrite();
			}
			//׷���ַ���
			void Append(System::Generic::Array<CharT> const&rhs, index_t index, size_t count) {
				if (index >= 0 && index + count < rhs.Count())
				{
					this->BeginWrite();
					this->data.Append(rhs.GetData() + index, count);
					this->EndWrite();
				}
			}
			//׷���ַ�
			void Append(CharT target) {
				this->BeginWrite();
				this->data.Append(target);
				this->EndWrite();
			}
			//��������:[start_index,end_index]
			index_t IndexOf(const CharT* string, index_t start_index, index_t end_index)const {
				return Algorithm::BasicStringIndexOf(string, this->data.GetMemoryBlock(), Algorithm::BasicStringLength(string), this->data.GetUsedSize() - 1, start_index, end_index);
			}
			//��������:[start_index,end]
			index_t IndexOf(const CharT* string, index_t start_index)const {
				return Algorithm::BasicStringIndexOf(string, this->data.GetMemoryBlock(), Algorithm::BasicStringLength(string), this->data.GetUsedSize() - 1, start_index, this->Count() - 1);
			}
			//��������:[0,end]
			index_t IndexOf(const CharT* string)const {
				return Algorithm::BasicStringIndexOf(string, this->data.GetMemoryBlock(), Algorithm::BasicStringLength(string), this->data.GetUsedSize() - 1, 0, this->Count() - 1);
			}
			//��������:[start_index,end_index]
			index_t IndexOf(BasicString const& string, index_t start_index, index_t end_index)const {
				return Algorithm::BasicStringIndexOf(string.GetData(), this->data.GetMemoryBlock(), string.Count(), this->data.GetUsedSize() - 1, start_index, end_index);
			}
			//��������:[start_index,end]
			index_t IndexOf(BasicString const& string, index_t start_index)const {
				return Algorithm::BasicStringIndexOf(string.GetData(), this->data.GetMemoryBlock(), string.Count(), this->data.GetUsedSize() - 1, start_index, this->Count() - 1);
			}
			//��������:[0,end]
			index_t IndexOf(BasicString const& string)const {
				return Algorithm::BasicStringIndexOf(string.GetData(), this->data.GetMemoryBlock(), string.Count(), this->data.GetUsedSize() - 1, 0, this->Count() - 1);
			}
			//��������:[start_index,end_index]
			index_t LastIndexOf(const CharT* string, index_t start_index, index_t end_index)const {
				return Algorithm::BasicStringLastIndexOf(string, this->data.GetMemoryBlock(), Algorithm::BasicStringLength(string), this->data.GetUsedSize() - 1, start_index, end_index);
			}
			//��������:[start_index,end]
			index_t LastIndexOf(const CharT* string, index_t start_index)const {
				return Algorithm::BasicStringLastIndexOf(string, this->data.GetMemoryBlock(), Algorithm::BasicStringLength(string), this->data.GetUsedSize() - 1, start_index, this->Count() - 1);
			}
			//��������:[0,end]
			index_t LastIndexOf(const CharT* string)const {
				return Algorithm::BasicStringLastIndexOf(string, this->data.GetMemoryBlock(), Algorithm::BasicStringLength(string), this->data.GetUsedSize() - 1, 0, this->Count() - 1);
			}
			//��������:[start_index,end_index]
			index_t LastIndexOf(BasicString const& string, index_t start_index, index_t end_index)const {
				return Algorithm::BasicStringLastIndexOf(string.GetData(), this->data.GetMemoryBlock(), string.Count(), this->data.GetUsedSize() - 1, start_index, end_index);
			}
			//��������:[start_index,end]
			index_t LastIndexOf(BasicString const& string, index_t start_index)const {
				return Algorithm::BasicStringLastIndexOf(string.GetData(), this->data.GetMemoryBlock(), string.Count(), this->data.GetUsedSize() - 1, start_index, this->Count() - 1);
			}
			//��������:[0,end]
			index_t LastIndexOf(BasicString const& string)const {
				return Algorithm::BasicStringLastIndexOf(string.GetData(), this->data.GetMemoryBlock(), string.Count(), this->data.GetUsedSize() - 1, 0, this->Count() - 1);
			}
			//�滻�ַ���
			void Replace(const CharT* oldstring, const CharT* newstring) {
				this->ReplaceInternal(oldstring, newstring, 0, this->data.GetUsedSize());
			}
			//�滻�ַ���
			void Replace(const CharT* oldstring, const CharT* newstring, index_t start_index, index_t end_index) {
				this->ReplaceInternal(oldstring, newstring, start_index, end_index);
			}
			//�滻�ַ���
			void Replace(const CharT* oldstring, const CharT* newstring, index_t start_index) {
				this->ReplaceInternal(oldstring, newstring, start_index, this->Count() - 1);
			}
			//�滻�ַ���
			void Replace(BasicString const& oldstring, BasicString const& newstring) {
				this->ReplaceInternal(oldstring.GetData(), newstring.GetData(), 0, this->data.GetUsedSize());
			}
			//�滻�ַ���
			void Replace(BasicString const& oldstring, BasicString const& newstring, index_t start_index, index_t end_index) {
				this->ReplaceInternal(oldstring.GetData(), newstring.GetData(), start_index, end_index);
			}
			//�滻�ַ���
			void Replace(BasicString const& oldstring, BasicString const& newstring, index_t start_index) {
				this->ReplaceInternal(oldstring.GetData(), newstring.GetData(), start_index, this->Count() - 1);
			}
			//��������:[start_index,end_index]
			index_t IndexOfAny(const CharT* string, index_t start_index, index_t end_index)const {
				return Algorithm::BasicStringIndexOfAny(string.GetData(), this->data.GetMemoryBlock(), string.Count(), this->data.GetUsedSize() - 1, start_index, end_index);
			}
			//��������:[start_index,end]
			index_t IndexOfAny(const CharT* string, index_t start_index)const {
				return Algorithm::BasicStringIndexOfAny(string, this->data.GetMemoryBlock(), Algorithm::BasicStringLength(string), this->data.GetUsedSize() - 1, start_index, this->Count() - 1);
			}
			//��������:[0,end]
			index_t IndexOfAny(const CharT* string)const {
				return Algorithm::BasicStringIndexOfAny(string, this->data.GetMemoryBlock(), Algorithm::BasicStringLength(string), this->data.GetUsedSize() - 1, 0, this->Count() - 1);
			}
			//��������:[start_index,end_index]
			index_t IndexOfAny(BasicString const& string, index_t start_index, index_t end_index)const {
				return Algorithm::BasicStringIndexOfAny(string.GetData(), this->data.GetMemoryBlock(), string.Count(), this->data.GetUsedSize() - 1, start_index, end_index);
			}
			//��������:[start_index,end]
			index_t IndexOfAny(BasicString const& string, index_t start_index)const {
				return Algorithm::BasicStringIndexOfAny(string.GetData(), this->data.GetMemoryBlock(), string.Count(), this->data.GetUsedSize() - 1, start_index, this->Count() - 1);
			}
			//��������:[0,end]
			index_t IndexOfAny(BasicString const& string)const {
				return Algorithm::BasicStringIndexOfAny(string.GetData(), this->data.GetMemoryBlock(), string.Count(), this->data.GetUsedSize() - 1, 0, this->Count() - 1);
			}
			//��ȡ[start_index,end_index]�ڵ��ַ���
			BasicString SubString(index_t start_index, index_t end_index)const {
				if (end_index >= start_index&&start_index >= 0 && end_index < this->Count())
				{
					BasicString ret(end_index - start_index + 1);
					ret.Append(this->data, start_index, end_index - start_index + 1);
					return ret;
				}
				return BasicString((size_t)0);
			}
			//�Ƴ��ַ���:[start_index,end_index]
			void Remove(index_t start_index, index_t end_index) {
				this->data.Remove(start_index, end_index);
			}
			//���ַ�����index�𿽱�count����
			void CopyTo(CharT* dest, index_t start_index, size_t count)const {
				if (start_index >= 0 && start_index + count < this->Count())
					Memory::Allocator::MemoryCopy(this->data.GetMemoryBlock() + start_index, dest, 0, count * sizeof(CharT));
			}
			//�Էָ����ָ��ַ���
			Generic::Array<BasicString> Split(BasicString const&separator)const {
				return Split(separator.GetData());
			}
			//�Էָ����ָ��ַ���
			Generic::Array<BasicString> Split(const CharT *separator)const {
				Generic::Array<BasicString> ret;
				size_t length = Algorithm::BasicStringLength(separator);
				index_t last_index = -(index_t)length;
				index_t result = 0;
				for (;;) {
					if (result + length > this->Count() - 1)
						break;
					result = this->IndexOf(separator, last_index + length);
					if (result == -1)
						break;
					else
					{
						String temp = this->SubString(last_index + length, result - 1);
						if (temp.Count() > 0)
							ret.Add(temp);
					}
					last_index = result;
				}
				ret.Add(this->SubString(last_index + length, this->Count() - 1));
				return ret;
			}
			//ת��Ϊ��д
			void ToUpper() {
				Algorithm::BasicStringUpperLower<CharT>::ToUpper(this->data.GetMemoryBlock());
			}
			//ת��ΪСд
			void ToLower() {
				Algorithm::BasicStringUpperLower<CharT>::ToLower(this->data.GetMemoryBlock());
			}
			//ȥ�����˿ո�
			void Trim() {
				BeginWrite();
				//���ͷ��
				CharT*str_ptr = this->data.GetMemoryBlock();
				while (*str_ptr == (CharT)' ')
					++str_ptr;
				CharT*block_ptr = this->data.GetMemoryBlock();
				this->data.Remove(0, (str_ptr - block_ptr) - 1);
				//β������
				str_ptr = block_ptr + this->data.GetUsedSize() - 1;
				while (*str_ptr == (CharT)' ')
					--str_ptr;
				this->data.Remove((str_ptr - block_ptr) + 1, this->data.GetUsedSize() - 1);
				EndWrite();
			}
			//����ַ�������
			size_t Count()const {
				size_t length = this->data.GetUsedSize();
				return length > 0 ? length - 1 : 0;
			}
			//�ַ����Ƿ�Ϊ��
			bool IsEmpty()const {
				return this->Count() == 0;
			}
			//���ԭʼָ��
			const CharT* GetData()const {
				return this->data.GetMemoryBlock();
			}
			static BasicString<CharT> ManageFrom(CharT*str, size_t count, size_t total)
			{
				BasicString<CharT> ret;
				ret.data.ManageFrom(str, count, total);
				return ret;
			}

			inline CharT const&operator[](index_t index)const {
				return this->data[index];
			}
			inline CharT &operator[](index_t index) {
				return this->data[index];
			}
		public:
			friend std::wostream& operator << (std::wostream&Stream, const BasicString&Obj) {
				if (Obj.Count() == 0)
					Stream << L"NullString";
				else
					Stream << Obj.data.GetMemoryBlock();
				return Stream;
			}
		public:
			//�߼�����
			//[����]func:��ָ����в����ĺ���
			//[����]func:�����Ϣ([In]CharT*buffer_ptr,[In]size_t capcity,[Out]size_t*used)
			template<class Fx>
			void AsBuffer(Fx func) {
				this->BeginWrite();
				size_t used = 0;
				func(this->data.GetMemoryBlock(), this->data.GetMaxSize(), &used);
				this->data.SetUsed(used);
				this->EndWrite();
			}
			template<class...Args>
			static BasicString Format(const CharT*format, Args const&...args) {
				ContainerT ret;
				FormatInternal(format, &ret, args...);
				ret.ShrinkToFit();
				return ret;
			}
			virtual size_t GetHashCode()const final {
				return Hash::Hash::HashSeq(this->GetData(), this->Count());
			}
			virtual void* ClonePtr()const final{
				return new BasicString<CharT>(*this);
			}
		};

		//Ϊ���������ṩ��ת��ΪString�Ľӿ�
		template<class InT, class CharT>
		class ToString
		{
		public:
			static BasicString<CharT> ConvertTo(InT const&obj) {
				return BasicString<CharT>((size_t)0);
			}
		};
		template<>
		class ToString<BasicString<wchar_t>, wchar_t> {
		public:
			static BasicString<wchar_t> ConvertTo(BasicString<wchar_t> const&obj) {
				return obj;
			}
		};
		template<>
		class ToString<int, wchar_t>
		{
		public:
			static BasicString<wchar_t> ConvertTo(int const&obj) {
				wchar_t pbuf[11];
				swprintf_s(pbuf, L"%d", obj);
				return BasicString<wchar_t>(pbuf);
			}
		};
		template<>
		class ToString<bool, wchar_t>
		{
		public:
			inline static BasicString<wchar_t> ConvertTo(bool const&obj) {
				return BasicString<wchar_t>(obj ? L"True" : L"False");
			}
		};
		template<>
		class ToString<long long, wchar_t> {
		public:
			static BasicString<wchar_t> ConvertTo(long long const&obj) {
				wchar_t pbuf[20];
				_i64tow_s(obj, pbuf, 20, 10);
				return BasicString<wchar_t>(pbuf);
			}
		};
		template<>
		class ToString<unsigned long long, wchar_t> {
		public:
			static BasicString<wchar_t> ConvertTo(unsigned long long const&obj) {
				wchar_t pbuf[20];
				_ui64tow_s(obj, pbuf, 20, 10);
				return BasicString<wchar_t>(pbuf);
			}
		};

#define NUM_TO_STRING(T,format,bufsize)template<>class ToString<T, wchar_t>{ \
	public: \
		static BasicString<wchar_t> ConvertTo(T const&obj) {\
			wchar_t pbuf[bufsize];\
			swprintf_s(pbuf,format, obj);\
			return BasicString<wchar_t>(pbuf);\
		}\
	}
		NUM_TO_STRING(long, L"%ld", 11);
		NUM_TO_STRING(unsigned long, L"%lu", 11);
		NUM_TO_STRING(float, L"%f", 26);
		NUM_TO_STRING(double, L"%f", 26);
		NUM_TO_STRING(long double, L"%lf", 26);
		NUM_TO_STRING(unsigned, L"%u", 11);

		namespace Internal
		{
			struct AllToString
			{
				template<class CharT, class T, class...Args>
				inline static void To(Memory::MemoryManager<BasicString<CharT>>&out, const CharT * format ,Container::Tuple<index_t, index_t, index_t, size_t> *format_args, T const&current, Args const&...args) {
					CharT const* format_ptr = format + format_args->GetAt<0>() + format_args->GetAt<3>() + 1;//ԭָ�� + ������'{'��λ�� + ռλ������ռ�ĳ��� + 1 [��Ϊformat������ʼ�ĵط�]
					if (format_ptr == format + format_args->GetAt<2>() + 1)
						format_ptr = nullptr;
					size_t length = format_args->GetAt<2>() - format_args->GetAt<0>() - format_args->GetAt<3>() - 1;//������'}'��λ�� - ������'{'��λ�� - ռλ������ռ�ĳ��� - 1 [��Ϊ�ڴ���format��������]
					BasicString<CharT> str = StringFormat<T, CharT>::Format(current, format_ptr, length);
					out.Append(str);
					To<CharT, Args...>(out, format, ++format_args, args...);//����,��format_args�����Ƶ���һ������
				}
				template<class CharT, class T>
				inline static void To(Memory::MemoryManager<BasicString<CharT>>&out, const CharT * format , Container::Tuple<index_t, index_t, index_t, size_t> *format_args, T const&current) {
					CharT const* format_ptr = format + format_args->GetAt<0>() + format_args->GetAt<3>() + 1;//ԭָ�� + ������'{'��λ�� + ռλ������ռ�ĳ��� + 1 [��Ϊformat������ʼ�ĵط�]
					if (format_ptr == format + format_args->GetAt<2>() + 1)
						format_ptr = nullptr;
					size_t length = format_args->GetAt<2>() - format_args->GetAt<0>() - format_args->GetAt<3>() - 1;//������'}'��λ�� - ������'{'��λ�� - ռλ������ռ�ĳ��� - 1 [��Ϊ�ڴ���format��������]
					BasicString<CharT> str = StringFormat<T, CharT>::Format(current, format_ptr, length);
					out.Append(str);
				}
			};
		}

		template<class CharT>
		template<class ...Args>
		inline void BasicString<CharT>::FormatInternal(const CharT * format, ContainerT * out, Args const & ...args)
		{
			size_t length = Algorithm::BasicStringLength(format);
			out->Append(format, length);
			Memory::MemoryManager<Container::Tuple<index_t, index_t, index_t, size_t>>outer;//����,���ڴ��ռλ��λ�ü�����Ϣ {�����Ŵ�����,�м�����,�����Ŵ�����}
			Memory::MemoryManager<BasicString<CharT>>to_string(sizeof...(Args)); //����,���ڴ��args����ת��Ϊ��string
			Internal::FormatPattern<CharT>::Match(format, length, outer);//ƥ��ռλ��
			Internal::AllToString::To(to_string, format, outer.GetMemoryBlock(), args...);//������ת��Ϊ�ַ���
			
			index_t offset = 0;//���Ķ���,�����ͻᷢ����Ӧƫ��
			for (index_t i = 0; i < outer.GetUsedSize(); ++i) {
				if (outer[i].GetAt<1>() <= sizeof...(Args) && outer[i].GetAt<1>() >= 0)//ȷ���м�����������ڷ�Χ�ڵ�
				{
					BasicString&current_str = to_string[outer[i].GetAt<1>()];//ȡ��Ӧ����ת����string������
					size_t num_length = (outer[i].GetAt<2>() - outer[i].GetAt<0>() + 1);//����ռλ��ռ�˶೤ ����{0}��3
					index_t differ = current_str.Count() - num_length;//��������ַ�����ռλ�����ȵĲ�ֵ
					//������Replace���һ��
					if (differ > 0)//���ַ�����ԭ�ַ�����
					{
						out->Expand(differ * 2);
						out->MoveBackward(outer[i].GetAt<0>() + offset, differ);//����ƶ���ֵ��λ
						Memory::Allocator::MemoryCopy(current_str.GetData(), out->GetMemoryBlock() + outer[i].GetAt<0>() + offset, 0, sizeof(CharT)*current_str.Count());//�����ַ�������
					}
					else if (differ < 0)
					{
						Memory::Allocator::MemoryCopy(current_str.GetData(), out->GetMemoryBlock() + outer[i].GetAt<0>() + offset, 0, sizeof(CharT)*current_str.Count());//�����ַ�������
						Memory::Allocator::MemoryCopy(out->GetMemoryBlock() + outer[i].GetAt<0>() + offset + num_length, out->GetMemoryBlock() + outer[i].GetAt<0>() + offset + current_str.Count(), 0, sizeof(CharT)*(out->GetUsedSize() - (outer[i].GetAt<0>() + offset)));//��ǰ�ƶ�
					}
					else//differ==0
					{
						Memory::Allocator::MemoryCopy(current_str.GetData(), out->GetMemoryBlock() + outer[i].GetAt<0>() + offset, 0, sizeof(CharT)*current_str.Count());//�����ַ�������
					}
					offset += differ;//���Ķ���Ӧ��offset���ϸĶ�ֵ
				}
			}
			out->Append((CharT)'\0');//��ӽ�����
		}

		//Ĭ��ֵ�����ػ�
		template<class InT,class CharT>
		class StringFormat
		{
		public:
			inline static BasicString<CharT> Format(InT const& object, CharT const*format_args, size_t length)
			{
				return BasicString<CharT>((size_t)0);
			}
		};
		template<class CharT>
		class StringFormat<int, CharT>
		{
		public:
			inline static BasicString<CharT> Format(int const& object, CharT const*format_args, size_t length)
			{
				Memory::MemoryManager<CharT> ret(12);
				if (format_args == nullptr)
				{
					StringFunction::IntToString(ret, object, 10);
					return ret;
				}
				if (format_args[0] == (CharT)L'X')
					StringFunction::IntToString(ret, object, 16);
				else if (format_args[0] == (CharT)L'R')
				{
					size_t radix = StringFunction::StringToInt<size_t>(format_args + 1, 2);//���XX����
					StringFunction::IntToString(ret, object, radix);
				}
				else if (format_args[0] == (CharT)L'G')
					StringFunction::IntToString(ret, object, 10);
				return ret;
			}
		};
		template<class CharT>
		class StringFormat<short, CharT>
		{
		public:
			inline static BasicString<CharT> Format(int const& object, CharT const*format_args, size_t length)
			{
				Memory::MemoryManager<CharT> ret(6);
				if (format_args == nullptr)
				{
					StringFunction::IntToString(ret, object, 10);
					return ret;
				}
				if (format_args[0] == (CharT)L'X')
					StringFunction::IntToString(ret, object, 16);
				else if (format_args[0] == (CharT)L'R')
				{
					size_t radix = StringFunction::StringToInt<size_t>(format_args + 1, 2);//���XX����
					StringFunction::IntToString(ret, object, radix);
				}
				else if (format_args[0] == (CharT)L'G')
					StringFunction::IntToString(ret, object, 10);
				return ret;
			}
		};
		template<class CharT>
		class StringFormat<long, CharT>
		{
		public:
			inline static BasicString<CharT> Format(long const& object, CharT const*format_args, size_t length)
			{
				Memory::MemoryManager<CharT> ret(12);
				if (format_args == nullptr)
				{
					StringFunction::IntToString(ret, object, 10);
					return ret;
				}
				if (format_args[0] == (CharT)L'X')
					StringFunction::IntToString(ret, object, 16);
				else if (format_args[0] == (CharT)L'R')
				{
					size_t radix = StringFunction::StringToInt<size_t>(format_args + 1, 2);//���XX����
					StringFunction::IntToString(ret, object, radix);
				}
				else if (format_args[0] == (CharT)L'G')
					StringFunction::IntToString(ret, object, 10);
				return ret;
			}
		};
		template<class CharT>
		class StringFormat<long long, CharT>
		{
		public:
			inline static BasicString<CharT> Format(long long const& object, CharT const*format_args, size_t length)
			{
				Memory::MemoryManager<CharT> ret(20);
				if (format_args == nullptr)
				{
					StringFunction::IntToString(ret, object, 10);
					return ret;
				}
				if (format_args[0] == (CharT)L'X')
					StringFunction::IntToString(ret, object, 16);
				else if (format_args[0] == (CharT)L'R')
				{
					size_t radix = StringFunction::StringToInt<size_t>(format_args + 1, 2);//���XX����
					StringFunction::IntToString(ret, object, radix);
				}
				else if (format_args[0] == (CharT)L'G')
					StringFunction::IntToString(ret, object, 10);
				return ret;
			}
		};
		template<class CharT>
		class StringFormat<unsigned int, CharT>
		{
		public:
			inline static BasicString<CharT> Format(unsigned int const& object, CharT const*format_args, size_t length)
			{
				Memory::MemoryManager<CharT> ret(12);
				if (format_args == nullptr)
				{
					StringFunction::IntToString(ret, object, 10);
					return ret;
				}
				if (format_args[0] == (CharT)L'X')
					StringFunction::IntToString(ret, object, 16);
				else if (format_args[0] == (CharT)L'R')
				{
					size_t radix = StringFunction::StringToInt<size_t>(format_args + 1, 2);//���XX����
					StringFunction::IntToString(ret, object, radix);
				}
				else if (format_args[0] == (CharT)L'G')
					StringFunction::IntToString(ret, object, 10);
				return ret;
			}
		};
		template<class CharT>
		class StringFormat<unsigned long, CharT>
		{
		public:
			inline static BasicString<CharT> Format(unsigned long const& object, CharT const*format_args, size_t length)
			{
				Memory::MemoryManager<CharT> ret(12);
				if (format_args == nullptr)
				{
					StringFunction::IntToString(ret, object, 10);
					return ret;
				}
				if (format_args[0] == (CharT)L'X')
					StringFunction::IntToString(ret, object, 16);
				else if (format_args[0] == (CharT)L'R')
				{
					size_t radix = StringFunction::StringToInt<size_t>(format_args + 1, 2);//���XX����
					StringFunction::IntToString(ret, object, radix);
				}
				else if (format_args[0] == (CharT)L'G')
					StringFunction::IntToString(ret, object, 10);
				return ret;
			}
		};
		template<class CharT>
		class StringFormat<unsigned long long, CharT>
		{
		public:
			inline static BasicString<CharT> Format(unsigned long long const& object, CharT const*format_args, size_t length)
			{
				Memory::MemoryManager<CharT> ret(20);
				if (format_args == nullptr)
				{
					StringFunction::IntToString(ret, object, 10);
					return ret;
				}
				if (format_args[0] == (CharT)L'X')
					StringFunction::IntToString(ret, object, 16);
				else if (format_args[0] == (CharT)L'R')
				{
					size_t radix = StringFunction::StringToInt<size_t>(format_args + 1, 2);//���XX����
					StringFunction::IntToString(ret, object, radix);
				}
				else if (format_args[0] == (CharT)L'G')
					StringFunction::IntToString(ret, object, 10);
				return ret;
			}
		};
		template<class CharT>
		class StringFormat<unsigned short, CharT>
		{
		public:
			inline static BasicString<CharT> Format(unsigned short const& object, CharT const*format_args, size_t length)
			{
				Memory::MemoryManager<CharT> ret(5);
				if (format_args == nullptr)
				{
					StringFunction::IntToString(ret, object, 10);
					return ret;
				}
				if (format_args[0] == (CharT)L'X')
					StringFunction::IntToString(ret, object, 16);
				else if (format_args[0] == (CharT)L'R')
				{
					size_t radix = StringFunction::StringToInt<size_t>(format_args + 1, 2);//���XX����
					StringFunction::IntToString(ret, object, radix);
				}
				else if (format_args[0] == (CharT)L'G')
					StringFunction::IntToString(ret, object, 10);
				return ret;
			}
		};
		template<class CharT>
		class StringFormat<double, CharT>
		{
		public:
			inline static BasicString<wchar_t> Format(double const& object, CharT const*format_args, size_t length)
			{
				Memory::MemoryManager<CharT> ret(15);
				if (format_args == nullptr)
				{
					StringFunction::FloatToString(ret, object, 0, true);
					return ret;
				}
				if (format_args[0] == (CharT)L'D')
				{
					size_t precision = StringFunction::StringToInt<size_t>(format_args + 1, 2);//�����XXλ
					StringFunction::FloatToString(ret, object, precision, false);
				}
				else if (format_args[0] == (CharT)L'G')
				{
					StringFunction::FloatToString(ret, object, 0, true);
				}
				return ret;
			}
		};
		template<class CharT>
		class StringFormat<long double, CharT>
		{
		public:
			inline static BasicString<wchar_t> Format(long double const& object, CharT const*format_args, size_t length)
			{
				Memory::MemoryManager<CharT> ret(15);
				if (format_args == nullptr)
				{
					StringFunction::FloatToString(ret, object, 0, true);
					return ret;
				}
				if (format_args[0] == (CharT)L'D')
				{
					size_t precision = StringFunction::StringToInt<size_t>(format_args + 1, 2);//�����XXλ
					StringFunction::FloatToString(ret, object, precision, false);
				}
				else if (format_args[0] == (CharT)L'G')
				{
					StringFunction::FloatToString(ret, object, 0, true);
				}
				return ret;
			}
		};
		template<class CharT>
		class StringFormat<float, CharT>
		{
		public:
			inline static BasicString<wchar_t> Format(float const& object, CharT const*format_args, size_t length)
			{
				Memory::MemoryManager<CharT> ret(15);
				if (format_args == nullptr)
				{
					StringFunction::FloatToString(ret, object, 0, true);
					return ret;
				}
				if (format_args[0] == (CharT)L'D')
				{
					size_t precision = StringFunction::StringToInt<size_t>(format_args + 1, 2);//�����XXλ
					StringFunction::FloatToString(ret, object, precision, false);
				}
				else if (format_args[0] == (CharT)L'G')
				{
					StringFunction::FloatToString(ret, object, 0, true);
				}
				return ret;
			}
		};
		template<class CharT>
		class StringFormat<BasicString<CharT>, CharT>
		{
		public:
			inline static BasicString<wchar_t> Format(BasicString<CharT> const& object, CharT const*format_args, size_t length) {
				//����Ĭ���޹���
				return object;
			}
		};
	}
	namespace Exception
	{
		class IndexOutOfException:public HL::Exception::IException 
		{
			System::String exception_msg;
		public:
			IndexOutOfException(index_t index, index_t range){
				exception_msg = System::String::Format(L"����Խ��,��:{0}��;��ΧΪ:{1}", index, range);
			}
			IndexOutOfException() {
				exception_msg = L"����Խ��";
			}
			virtual wchar_t const* Message()throw(){
				return exception_msg.GetData();
			}
		};
		class BadAllocateException :public HL::Exception::IException {
			System::String exception_msg;
		public:
			BadAllocateException() {
				exception_msg = L"�ڴ����ʧ��";
			}
			BadAllocateException(size_t size) {
				exception_msg = System::String::Format(L"�ڴ����ʧ��,��СΪ:{0} byte", size);
			}
			virtual wchar_t const* Message()throw() {
				return exception_msg.GetData();
			}
		};
		class ParserException :public HL::Exception::IException
		{
			System::String exception_msg;
		public:
			ParserException() {
				exception_msg = L"��������!";
			}
			ParserException(System::String const&msg) {
				exception_msg = L"��������:";
				exception_msg.Append(msg);
			}
			virtual wchar_t const* Message()throw() {
				return exception_msg.GetData();
			}
		};
	};
	namespace System
	{
		namespace Functional
		{
			class InvokeFailedException :public HL::Exception::IException
			{
				String exception_msg;
			public:
				InvokeFailedException() {
					exception_msg = L"�������÷�ʽ����!";
				}
				InvokeFailedException(Internal::InvokeType Type) {
					switch (Type)
					{
					case HL::System::Functional::Internal::InvokeType::Default:
						exception_msg = L"�������÷�ʽ����!��Ӧ��ʹ��Default";
						break;
					case HL::System::Functional::Internal::InvokeType::InvokeWithNativePtr:
						exception_msg = L"�������÷�ʽ����!��Ӧ��ʹ��InvokeWithNativePtr";
						break;
					case HL::System::Functional::Internal::InvokeType::InvokeWithUPtr:
						exception_msg = L"�������÷�ʽ����!��Ӧ��ʹ��InvokeWithUPtr";
						break;
					default:
						break;
					}
				}
				virtual wchar_t const* Message()throw() {
					return exception_msg.GetData();
				}
			};
		}
	}
}