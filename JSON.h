#pragma once
namespace HL
{
	namespace System
	{
		template<>
		struct Interface::EnumerableSupportInterface<Json::JsonArray>
		{
			typedef UPointer::uptr<Iteration::Iterator<Json::UJsonObject>> IteratorType;
			typedef UPointer::uptr<Iteration::Iterator<Json::UJsonObject const>> ConstIteratorType;
		};
		template<>
		struct Interface::EnumerableSupportInterface<Json::JsonObject>
		{
			typedef UPointer::uptr<Iteration::Iterator<Generic::ObservePair<String, Json::UIJsonValue>>> IteratorType;
			typedef UPointer::uptr<Iteration::Iterator<Generic::ObservePair<String, Json::UIJsonValue const>>> ConstIteratorType;
		};

		namespace Json
		{
			class JsonParseException :public HL::Exception::IException
			{
				String exception_msg;
			public:
				JsonParseException() {
					exception_msg = L"Parse error!";
				}
				JsonParseException(System::String const&msg) {
					exception_msg = L"Parse error:";
					exception_msg.Append(msg);
				}
				virtual wchar_t const* Message()throw() {
					return exception_msg.GetData();
				}
			};
			class JsonRuntimeException :public HL::Exception::IException
			{
				String exception_msg;
			public:
				JsonRuntimeException() {
					exception_msg = L"Runtime error!";
				}
				JsonRuntimeException(System::String const&msg) {
					exception_msg = L"Runtime error:";
					exception_msg.Append(msg);
				}
				virtual wchar_t const* Message()throw() {
					return exception_msg.GetData();
				}
			};
			enum class JsonValueType
			{
				Null,
				Boolean,
				Number,
				String,
				Array,
				Object
			};
			//Json对象接口
			class IJsonValue
			{
			protected:
				JsonValueType m_value_type;
			public:
				IJsonValue(JsonValueType type) :m_value_type(type) {}
				JsonValueType GetType()const {
					return m_value_type;
				}
				virtual ~IJsonValue() {}
			};
			//Json对象
			class JsonObject :public IJsonValue, public Linq::LinqBase<Generic::ObservePair<String, UIJsonValue>>
			{
			public:
				typedef Generic::Dictionary<String, UIJsonValue> ContainerT;
			private:
				ContainerT m_pairs;
			public:
				JsonObject() :IJsonValue(JsonValueType::Object) {}
				JsonObject(ContainerT const&Pairs) :IJsonValue(JsonValueType::Object), m_pairs(Pairs) {}
				JsonObject(ContainerT&& Pairs) :IJsonValue(JsonValueType::Object), m_pairs(Move(Pairs)) {}
				JsonObject(JsonObject const&rhs) :IJsonValue(JsonValueType::Object), m_pairs(rhs.m_pairs) {}
				JsonObject(JsonObject &&lhs)noexcept :IJsonValue(JsonValueType::Object), m_pairs(Move(lhs.m_pairs)) {}
				JsonObject&operator=(JsonObject const&rhs)
				{
					this->m_pairs = rhs.m_pairs;
					return *this;
				}
				JsonObject&operator=(JsonObject&&lhs)noexcept
				{
					this->m_pairs = static_cast<ContainerT&&>(lhs.m_pairs);
					return *this;
				}
				UIJsonValue const& Get(String const&Key)const
				{
					return m_pairs[Key];
				}
				UIJsonValue & Get(String const&Key)
				{
					return m_pairs[Key];
				}
				UJsonValue GetValueAt(String const&Key)const {
					UIJsonValue ret = m_pairs[Key];
					if (ret->GetType() == JsonValueType::Number || ret->GetType() == JsonValueType::Boolean || ret->GetType() == JsonValueType::String || ret->GetType() == JsonValueType::Null)
						return (UJsonValue)ret;
					else
						return nullptr;
				}
				UJsonArray GetArrayAt(String const&Key)const
				{
					UIJsonValue ret = m_pairs[Key];
					if (ret->GetType() == JsonValueType::Array)
						return (UJsonArray)ret;
					else
						return nullptr;
				}
				UJsonObject GetObjectAt(String const&Key)const
				{
					UIJsonValue ret = m_pairs[Key];
					if (ret->GetType() == JsonValueType::Object)
						return (UJsonObject)ret;
					else
						return nullptr;
				}
				UIJsonValue const& operator[](String const&Key)const {
					return Get(Key);
				}
				UIJsonValue & operator[](String const&Key) {
					return Get(Key);
				}
				void Add(String const&Key, UIJsonValue const& Value)
				{
					m_pairs.Add(Key, Value);
				}
				void Remove(String const&Key) {
					this->m_pairs.Remove(Key);
				}
				virtual UPointer::uptr<Iteration::IEnumerator<Generic::ObservePair<String, UIJsonValue>>> GetEnumerator()const {
					return m_pairs.GetEnumerator();
				}
				virtual ~JsonObject() = default;
			};
			//Json数组
			class JsonArray :public IJsonValue, public Linq::LinqBase<UIJsonValue>
			{
			public:
				typedef Generic::Array<UIJsonValue> ContainerT;
			private:
				ContainerT m_list;
			public:
				JsonArray() :IJsonValue(JsonValueType::Array) {}
				JsonArray(ContainerT const&list) :IJsonValue(JsonValueType::Array), m_list(list) {}
				JsonArray(ContainerT &&list) :IJsonValue(JsonValueType::Array), m_list(Move(list)) {}
				JsonArray(JsonArray const&rhs) :IJsonValue(JsonValueType::Array), m_list(rhs.m_list) {}
				JsonArray(JsonArray &&rhs)noexcept :IJsonValue(JsonValueType::Array), m_list(Move(rhs.m_list)) {}
				JsonArray&operator=(JsonArray const&rhs)
				{
					this->m_list = rhs.m_list;
					return *this;
				}
				JsonArray&operator=(JsonArray&&lhs)noexcept
				{
					this->m_list = static_cast<ContainerT&&>(lhs.m_list);
					return *this;
				}
				void Add(UIJsonValue const&target) {
					this->m_list.Add(target);
				}
				UIJsonValue const& operator[](index_t index)const
				{
					return m_list[index];
				}
				UIJsonValue & operator[](index_t index)
				{
					return m_list[index];
				}
				UJsonObject GetObjectAt(index_t index) {
					UIJsonValue ret = m_list[index];
					if (ret->GetType() == JsonValueType::Object)
						return (UJsonObject)ret;
					else
						return nullptr;
				}
				UJsonArray GetArrayAt(index_t index) {
					UIJsonValue ret = m_list[index];
					if (ret->GetType() == JsonValueType::Array)
						return (UJsonArray)ret;
					else
						return nullptr;
				}
				UJsonValue GetValueAt(index_t index) {
					UIJsonValue ret = m_list[index];
					if (ret->GetType() == JsonValueType::Number || ret->GetType() == JsonValueType::Boolean || ret->GetType() == JsonValueType::String || ret->GetType() == JsonValueType::Null)
						return (UJsonValue)ret;
					else
						return nullptr;
				}
				virtual UPointer::uptr<Iteration::IEnumerator<UIJsonValue>> GetEnumerator()const {
					return m_list.GetEnumerator();
				}
				virtual ~JsonArray() = default;
			};
			//Json值
			class JsonValue:public IJsonValue
			{
				String m_value;
				double m_value_cache;
			public:
				JsonValue() :IJsonValue(JsonValueType::Null) {}
				JsonValue(String const&value, JsonValueType type) :IJsonValue(type), m_value(value) {}
				JsonValue(double value_cache) :IJsonValue(JsonValueType::Number), m_value_cache(value_cache) {}
				JsonValue(JsonValue const&rhs) :IJsonValue(rhs.m_value_type), m_value(rhs.m_value), m_value_cache(rhs.m_value_cache) {}
				JsonValue(JsonValue &&lhs)noexcept :IJsonValue(lhs.m_value_type), m_value(Move(lhs.m_value)), m_value_cache(lhs.m_value_cache) {}
				JsonValue&operator=(JsonValue const& rhs) {
					this->m_value = rhs.m_value;
					this->m_value_cache = rhs.m_value_cache;
					this->m_value_type = rhs.m_value_type;
					return *this;
				}
				JsonValue&operator=(JsonValue &&lhs)noexcept {
					this->m_value = Move(lhs.m_value);
					this->m_value_cache = lhs.m_value_cache;
					this->m_value_type = lhs.m_value_type;
					return *this;
				}
				double AsDouble()const {
					if (this->m_value_type != JsonValueType::Number)
						HL::Exception::Throw<JsonRuntimeException>(L"Type not matched");
					return m_value_cache;
				}
				bool AsBoolean()const {
					if (this->m_value_type != JsonValueType::Boolean)
						HL::Exception::Throw<JsonRuntimeException>(L"Type not matched");
					if (m_value[0] == L't')
						return true;
					else
						return false;
				}
				String const& AsString()const {
					if (this->m_value_type != JsonValueType::String)
						HL::Exception::Throw<JsonRuntimeException>(L"Type not matched");
					return m_value;
				}
				virtual ~JsonValue() = default;
			};

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
				Represent,//:
				End
			};
			struct Token
			{
				TokenType Type;
				String Value;
				double ValueCache;
				Token() :Value(size_t(0)) {}
				Token(TokenType Type, double const&Value) :ValueCache(Value), Type(Type) {}
				Token(TokenType Type, String const&Value) :Value(Value), Type(Type) {}
				Token(Token&&lhs) :Value(lhs.Value), Type(lhs.Type),ValueCache(lhs.ValueCache) {}
			};
			class JsonTokenizer
			{
				wchar_t const* m_source = nullptr;
				index_t m_inx = 0;
				size_t m_size = 0;
			private:
				static void Throw(String const& Msg)
				{
					HL::Exception::Throw<JsonParseException>(Msg);
				}
				bool IsNull()const
				{
					return Algorithm::BasicStringSingleMatch(L"null", m_source + m_inx, 4);
				}
				bool IsTrue()const
				{
					return Algorithm::BasicStringSingleMatch(L"true", m_source + m_inx, 4);
				}
				bool IsFalse()const
				{
					return Algorithm::BasicStringSingleMatch(L"false", m_source + m_inx, 5);
				}
				bool IsSpace(wchar_t &corresponding)const
				{
					wchar_t wc = m_source[m_inx];
					if (wc == L'n')
						corresponding = L'\n';
					else if (wc == L'b')
						corresponding = L'\b';
					else if (wc == L'r')
						corresponding = L'\r';
					else if (wc == L't')
						corresponding = L'\t';
					else if (wc == L'f')
						corresponding = L'\f';
					else if (wc == L'"')
						corresponding = L'"';
					else if (wc == L'\\')
						corresponding = L'\\';
					else if (wc == L'/')
						corresponding = L'/';
					else if (wc == L'u')
						corresponding = L'u';
					else
						return false;
					return true;
				}
				bool IsWhite()const {
					wchar_t ch = m_source[m_inx];
					return  ch == L' ' || ch == '\n' || ch == '\t' || ch == '\r';
				}
				bool IsHex()const
				{
					wchar_t wc = m_source[m_inx];
					return ((wc >= L'0' && wc <= L'9') || (L'a' <= wc && wc <= L'f')
						|| (L'A' <= wc && wc <= L'F'));
				}
				Token ReadString()
				{					
					String target;
					for (;;)
					{
						m_inx++;
						if (m_source[m_inx] == L'\\')//转移开头
						{
							m_inx++;
							wchar_t corresponding = 0;
							if (IsSpace(corresponding))//是否为转义符
							{
								if (m_source[m_inx] == L'u')//16进制处理
								{
									for (int i = 0; i < 4; ++i)
									{
										m_inx++;
										if (!IsHex())
											Throw(L"Invalid hex format");
									}
									target.Append((wchar_t)StringFunction::HexToInt<int>(m_source + m_inx - 3, 4));//字符处理
								}
								else
								{
									target.Append(corresponding);
								}
							}
							else
								Throw(L"Invalid escape character");
						}
						else if (m_source[m_inx] == L'\r' || m_source[m_inx] == L'\n')
						{
							Throw(L"Invalid character");
						}
						else if (m_source[m_inx] == L'"')
						{
							m_inx++;
							return Token(TokenType::String, target);
						}
						else
							target.Append(m_source[m_inx]);
					}
				}
				Token ReadNumber()
				{
					size_t count = StringFunction::FloatSniff(m_source + m_inx);
					if (count == 0)
						Throw(L"Invalid number format");
					double first_part = StringFunction::StringToFloat<double>(m_source + m_inx, count);
					m_inx += count;
					if (m_source[m_inx] == L'E' || m_source[m_inx] == L'e')
					{
						m_inx++;
						size_t sec_count = StringFunction::FloatSniff(m_source + m_inx);
						if (sec_count == 0)
							Throw(L"Invalid number format");
						else
						{
							double second_part = StringFunction::StringToFloat<double>(m_source + m_inx, sec_count);
							m_inx += sec_count;
							return Token(TokenType::Number, pow(first_part, second_part));
						}
					}
					else
						return Token(TokenType::Number, first_part);
					return Token(TokenType::Number, 0);
				}
			public:
				JsonTokenizer(wchar_t const*source, size_t size) :m_source(source), m_size(size) {}
				JsonTokenizer(wchar_t const*source) :m_source(source), m_size(Algorithm::BasicStringLength(source)) {}
				Token Go() {
					while (IsWhite() && m_inx < m_size) {
						m_inx++;
					}
					switch (m_source[m_inx])
					{
					case L'{':
						m_inx++;
						return Token(TokenType::ObjectStart, L"{");
					case L'}':
						m_inx++;
						return Token(TokenType::ObjectEnd, L"}");
					case L'[':
						m_inx++;
						return Token(TokenType::ArrayStart, L"[");
					case L']':
						m_inx++;
						return Token(TokenType::ArrayEnd, L"]");
					case L',':
						m_inx++;
						return Token(TokenType::Split, L",");
					case L':':
						m_inx++;
						return Token(TokenType::Represent, L":");
					case L'n':
						if (IsNull())
						{
							m_inx += 4;
							return Token(TokenType::Null, L"null");
						}
						else
							Throw(L"Invalid key word : null");
					case L't':
						if (IsTrue())
						{
							m_inx += 4;
							return Token(TokenType::Boolean, L"true");
						}
						else
							Throw(L"Invalid key word : true");
					case L'f':
						if (IsFalse())
						{
							m_inx += 5;
							return Token(TokenType::Boolean, L"false");
						}
						else
							Throw(L"Invalid key word : false");
					case L'"':
						return ReadString();
					case L'-':
						return ReadNumber();
					}
					if (StringFunction::IsInt(m_source[m_inx]))
						return ReadNumber();
					Throw(L"Syntax error");
					return Token();
				}
				bool IsDone()const {
					return m_inx >= m_size;
				}
			};
			class JsonParser
			{
				JsonTokenizer m_tokenizer;
				UJsonArray InnerParseArray(bool skip_self);
			public:
				JsonParser(String const&target) :m_tokenizer(target.GetData(), target.Count()) {}
				JsonParser(wchar_t const*target) :m_tokenizer(target) {}
				UJsonArray ParseArray()
				{
					return InnerParseArray(true);
				}
				UJsonObject ParseObject();
			};
			UJsonArray JsonParser::InnerParseArray(bool skip_self) {
				UJsonArray ret = Reference::newptr<JsonArray>();
				if (skip_self)
					m_tokenizer.Go();
				while (!m_tokenizer.IsDone())
				{
					Token current = m_tokenizer.Go();
					switch (current.Type)
					{
					case TokenType::ObjectStart:
						ret->Add(ParseObject());
						break;
					case TokenType::ArrayStart:
						ret->Add(InnerParseArray(false));
						break;
					case TokenType::String:
						ret->Add(Reference::newptr<JsonValue>(current.Value, JsonValueType::String));
						break;
					case TokenType::Null:
						ret->Add(Reference::newptr<JsonValue>(current.Value, JsonValueType::Null));
						break;
					case TokenType::Number:
						ret->Add(Reference::newptr<JsonValue>(current.ValueCache));
						break;
					case TokenType::Boolean:
						ret->Add(Reference::newptr<JsonValue>(current.Value, JsonValueType::Boolean));
						break;
					case TokenType::Split:
						break;
					case TokenType::ArrayEnd:
						return ret;
					default:
						HL::Exception::Throw<JsonParseException>(L"Unexpected token");
					}
				}
				HL::Exception::Throw<JsonParseException>(L"Unexpected end");
				return nullptr;
			}
			UJsonObject JsonParser::ParseObject() {
				UJsonObject ret = Reference::newptr<JsonObject>();
				bool is_represented = false;
				bool item_is_done = false;
				String key;
				UIJsonValue value = nullptr;
				while (!m_tokenizer.IsDone())
				{
					Token current = m_tokenizer.Go();
					switch (current.Type)
					{
					case TokenType::Represent:
						is_represented = true;
						break;
					case TokenType::String:
						if (!is_represented)
							key = current.Value;
						else
						{
							value = Reference::newptr<JsonValue>(current.Value, JsonValueType::String);
							is_represented = false;
							item_is_done = true;
						}
						break;
					case TokenType::ObjectStart:
						if (!is_represented)
							HL::Exception::Throw<JsonParseException>(L"Object can only be values");
						value = ParseObject();
						is_represented = false;
						item_is_done = true;
						break;
					case TokenType::Boolean:
						if (!is_represented)
							HL::Exception::Throw<JsonParseException>(L"Booleans can only be values");
						value = Reference::newptr<JsonValue>(current.Value, JsonValueType::Boolean);
						is_represented = false;
						item_is_done = true;
						break;
					case TokenType::ArrayStart:
						if (!is_represented)
							HL::Exception::Throw<JsonParseException>(L"Array can only be values");
						value = InnerParseArray(false);
						is_represented = false;
						item_is_done = true;
						break;
					case TokenType::Number:
						if (!is_represented)
							HL::Exception::Throw<JsonParseException>(L"Numbers can only be values");
						value = Reference::newptr<JsonValue>(current.ValueCache);
						is_represented = false;
						item_is_done = true;
						break;
					case TokenType::Null:
						if (!is_represented)
							HL::Exception::Throw<JsonParseException>(L"Null can only be values");
						value = Reference::newptr<JsonValue>();
						is_represented = false;
						item_is_done = true;
					case TokenType::Split:
						break;
					case TokenType::ObjectEnd:
						return ret;
					}
					if (item_is_done)
					{
						ret->Add(key, value);
						item_is_done = false;
					}
				}
				HL::Exception::Throw<JsonParseException>(L"Syntax error");
				return nullptr;
			}
		}
	}
}