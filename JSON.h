#pragma once
namespace HL
{
	namespace System
	{

		namespace Exception
		{
			class JsonParseException:public HL::Exception::IException
			{
				String exception_msg;
			public:
				JsonParseException() {
					exception_msg = L"Parse错误!";
				}
				JsonParseException(System::String const&msg) {
					exception_msg = L"Parse错误:";
					exception_msg.Append(msg);
				}
				virtual wchar_t const* Message()throw() {
					return exception_msg.GetData();
				}
			};
		}
		namespace Json
		{
			
			enum class JsonValueType
			{
				Null,
				Boolean,
				Number,
				String,
				Array,
				Object
			};
			//Json值
			class IJsonValue
			{
			protected:
				JsonValueType m_value_type;
			public:
				//IJsonValue(JsonValueType Type):m_value_type(Type){}
			};
			//Json对象
			class JsonObject :public IJsonValue
			{

			};
			//Json数组
			class JsonArray :public IJsonValue
			{

			};
			class JsonValue :public IJsonValue
			{

			};
			namespace Inner
			{
				enum class TokenType
				{
					String,
					Number,
					Null,
					Boolean,
					ObjectStart,//{
					ObjectEnd,//}
					ArrayStart,//[
					ArrayEnd,//]
					Split,//,
					Represent//:
				};
				struct Token
				{
					TokenType Type;
					String Value;
					Token():Value(size_t(0)){}
					Token(TokenType Type, String const&Value) :Value(Value), Type(Type) {}
					Token(Token&&lhs) :Value(lhs.Value), Type(lhs.Type) {
					}
				};
				class DFA
				{
					wchar_t const*m_source = nullptr;
					inline wchar_t const& Read() {
						wchar_t const&ret = *m_source;
						m_source++;
						return ret;
					}
					inline void UnRead() {
						m_source--;
					}
					inline void UnRead(size_t count) {
						m_source -= count;
					}
					bool IsSpace() {
						wchar_t current = Read();
						if (current == L'\\')
						{
							current = Read();
							if (current == L'"' || current == L'\\' || current == L'/' || current == L'b' ||
								current == L'f' || current == L'n' || current == L't' || current == L'r' || current == L'u'){
								return true;
							}
							else
								HL::Exception::Throw<Exception::JsonParseException>(L"不正确的空白符");
						}
						else
						{
							//归位
							UnRead();
							return false;
						}
					}
					String ReadString() {
						String ret;
						wchar_t wc;
						while (wc != L'"')
						{
							if (!IsSpace())
							{
								wc = Read();
								ret.Append(wc);
							}
						}
						return ret;
					}
					double ReadNumber() {
						
					}
					void CheckNull() {
						UnRead();
						wchar_t wc = Read();
						if (wc == L'n')
						{
							wc = Read();
							if (wc == L'u')
							{
								wc = Read();
								if (wc == L'l')
								{
									wc = Read();
									if (wc == L'l')
									{
										return;
									}
								}
							}
						}
						HL::Exception::Throw<Exception::JsonParseException>(L"无效null符");
					}
					void CheckTrue() {
						UnRead();
						wchar_t wc = Read();
						if (wc == L't')
						{
							wc = Read();
							if (wc == L'r')
							{
								wc = Read();
								if (wc == L'u')
								{
									wc = Read();
									if (wc == L'e')
									{
										return;
									}
								}
							}
						}
						HL::Exception::Throw<Exception::JsonParseException>(L"无效true标识符");
					}
					void CheckFalse() {
						UnRead();
						wchar_t wc = Read();
						if (wc == L'f')
						{
							wc = Read();
							if (wc == L'a')
							{
								wc = Read();
								if (wc == L'l')
								{
									wc = Read();
									if (wc == L's')
									{
										wc = Read();
										if (wc == L'e')
										{
											return;
										}
									}
								}
							}
						}
						HL::Exception::Throw<Exception::JsonParseException>(L"无效false标识符");
					}
				public:
					Token Go() {
						while (IsSpace());
						wchar_t wc = Read();
						if (wc == ',')
							return Token(TokenType::Split, L",");
						else if (wc == ':')
							return Token(TokenType::Represent, L":");
						else if (wc == '[')
							return Token(TokenType::ArrayStart, L"[");
						else if (wc == ']')
							return Token(TokenType::ArrayEnd, L"]");
						else if (wc == '{')
							return Token(TokenType::ObjectStart, L"{");
						else if (wc == '}')
							return Token(TokenType::ObjectEnd, L"}");
						else if (wc == '"')
							return Token(TokenType::String, ReadString());
						else if (wc == 'n')
						{
							CheckNull();
							return Token(TokenType::Null, L"null");
						}
						else if (wc == 't') {
							CheckTrue();
							return Token(TokenType::Boolean, L"True");
						}
						else if (wc == 'f') {
							CheckFalse();
							return Token(TokenType::Boolean, L"False");
						}
						else if (wc == L'-' || StringFunction::IsInt(wc))
						{

						}
					}
				};

			}

			class JsonParser
			{
			public:
				
			};
		}


	}
}