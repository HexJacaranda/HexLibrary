#pragma once
namespace HL
{
	namespace System
	{
		namespace RegularExpression
		{
			//标记的类型
			enum class TokenType
			{
				//字符(非特殊
				Char,
				//左花括号
				LCurly,
				//右花括号
				RCurly,
				//左小括号
				LParen,
				//右小括号
				RParen,
				//左中括号
				LBracket,
				//右中括号
				RBracket,
				//加号
				Plus,
				//减号
				Minus,
				//乘号
				Mul,
				//逗号
				Comma,
				//指数符
				Pow,
				//问号
				QMark,
				//冒号
				Colon,
				//竖线
				Or,
				//转义
				Escape
			};
			//标记
			class Token
			{
			public:
				wchar_t const* Begin;
				size_t Size;
				TokenType Type;
			};
			//词法分析器
			class Tokenizer
			{
				wchar_t const* m_source;
				index_t m_index;
				size_t m_size;
				Token* m_current;
			private:
				//是否为空白
				bool IsBlank() {
					wchar_t ch = m_source[m_index];
					return  ch == L' ' || ch == '\n' || ch == '\t' || ch == '\r';
				}
				inline void SkipBlank() {
					while (IsBlank() && m_index < m_size)m_index++;
				}
				void SetToken(TokenType Type) {
					m_current->Begin = m_source + m_index;
					m_current->Type = Type;
					m_current->Size = 1;
					m_index++;
				}
				void PeekToken(TokenType Type) {
					m_current->Begin = m_source + m_index;
					m_current->Type = Type;
					m_current->Size = 1;
				}
			public:
				Tokenizer(wchar_t const* Source, size_t Size) :m_source(Source), m_size(Size), m_index(0), m_current(nullptr) {
				}
				void Consume(Token &Out)
				{
					m_current = &Out;
					SkipBlank();
					switch (m_source[m_index])
					{
					case L'-':
						return SetToken(TokenType::Minus);
					case L'+':
						return SetToken(TokenType::Plus);
					case L'*':
						return SetToken(TokenType::Mul);
					case L',':
						return SetToken(TokenType::Comma);
					case L'^':
						return SetToken(TokenType::Pow);
					case L'?':
						return SetToken(TokenType::QMark);
					case L':':
						return SetToken(TokenType::Colon);
					case L'|':
						return SetToken(TokenType::Or);
					case L'\\':
						return SetToken(TokenType::Escape);
					case L'{':
						return SetToken(TokenType::LCurly);
					case L'}':
						return SetToken(TokenType::RCurly);
					case L'(':
						return SetToken(TokenType::LParen);
					case L')':
						return SetToken(TokenType::RParen);
					case L'[':
						return SetToken(TokenType::LBracket);
					case L']':
						return SetToken(TokenType::RBracket);
					default:
						return SetToken(TokenType::Char);
					}
				}
				void Peek(Token &Out)
				{
					m_current = &Out;
					SkipBlank();
					switch (m_source[m_index])
					{
					case L'-':
						return PeekToken(TokenType::Minus);
					case L'+':
						return PeekToken(TokenType::Plus);
					case L'*':
						return PeekToken(TokenType::Mul);
					case L',':
						return PeekToken(TokenType::Comma);
					case L'^':
						return PeekToken(TokenType::Pow);
					case L'?':
						return PeekToken(TokenType::QMark);
					case L':':
						return PeekToken(TokenType::Colon);
					case L'|':
						return PeekToken(TokenType::Or);
					case L'\\':
						return PeekToken(TokenType::Escape);
					case L'{':
						return PeekToken(TokenType::LCurly);
					case L'}':
						return PeekToken(TokenType::RCurly);
					case L'(':
						return PeekToken(TokenType::LParen);
					case L')':
						return PeekToken(TokenType::RParen);
					case L'[':
						return PeekToken(TokenType::LBracket);
					case L']':
						return PeekToken(TokenType::RBracket);
					default:
						return PeekToken(TokenType::Char);
					}
				}
				void Move(index_t Offset) {
					m_index += Offset;
				}
				inline void Position(index_t Target) {
					m_index = Target;
				}
				inline index_t Position() {
					return m_index;
				}
				inline size_t Count() {
					return m_size;
				}
				inline bool Done() {
					return m_index >= m_size;
				}
			};
			//用于Regex管理内存的容器
			template<class T>
			class Pool
			{
				Generic::Dictionary<intptr_t, T*> m_objects;
			public:
				Pool() {}
				Pool(Pool &&lhs) :m_objects(Forward(lhs.m_objects)) {}
				Pool(Pool const& rhs) :m_objects(rhs.m_objects) {}
				Pool& operator=(Pool const&rhs) {
					this->m_objects = rhs.m_objects;
					return *this;
				}
				Pool& operator=(Pool &&lhs) {
					this->m_objects = Forward(lhs);
					return *this
				}
				void Add(T*&Target) {
					m_objects.Add((intptr_t)Target, Target);
				}
				void Remove(T*Target) {
					m_objects.Add()
				}
				inline T* Create() {
					return new T;
				}
				template<class...Args>
				inline T* CreateAndAppend(Args const&...args) {
					T*ret = new T(args...);
					m_objects.Add((intptr_t)ret, ret);
					return ret;
				}
				template<class U, class...Args>
				inline U* CreateAndAppendWith(Args const&...args) {
					U*ret = new U(args...);
					m_objects.Add((intptr_t)ret, ret);
					return ret;
				}
				auto GetEnumerator() {
					return m_objects.GetEnumerator();
				}
				~Pool() {
					for (auto&item : m_objects)
						delete item.Value();
				}
			};
			//内容
			class ContentBase
			{
			protected:
				bool m_not;
				wchar_t m_first;
			public:
				ContentBase(wchar_t First, bool Not = false) :m_first(First), m_not(Not) {}
				//是否接受
				virtual bool Accept(wchar_t Target) {
					if (m_first == L'.')
						return !m_not;
					return Target == m_first && !m_not;
				}
				virtual ~ContentBase() {}
			};
			//字符集
			class CharRange :public ContentBase
			{
				wchar_t m_left;
				wchar_t m_right;
			public:
				CharRange(wchar_t left, wchar_t right, bool not= false) :m_left(left), m_right(right), ContentBase(left, not) {}

				virtual bool Accept(wchar_t Target) {
					return (Target >= m_left && Target <= m_right) && !m_not;
				}
				virtual ~CharRange() {}
			};
			//边动作
			class Action
			{
			public:
				virtual void Act(Edge*) = 0;
				virtual ~Action() {}
			};
			//边
			class Edge
			{
			public:
				Edge() :From(nullptr), To(nullptr), Content(nullptr) {}
				Status*From;//起始状态
				Status*To;//目标状态
				ContentBase*Content;//匹配内容
				Generic::Array<Action*> Actions;//动作
			};
			//状态
			class Status
			{
			public:
				Generic::Array<Edge*> InEdges;
				Generic::Array<Edge*> OutEdges;
				bool Final;
			};
			//资源
			class Resource
			{
			public:
				Pool<NFA> NFAPool;
				Pool<ContentBase> ContentPool;
				Pool<Edge> EdgePool;
				Pool<Status> StatusPool;
			};
			//NFA状态机
			class NFA
			{
				Resource*Resources;
			public:
				NFA(Resource*Origin) :Resources(Origin), Head(nullptr), Tail(nullptr) {}
				NFA() :Head(nullptr), Tail(nullptr) {}
				Status*Head;
				Status*Tail;
				//构造节点
				void BuildNode(ContentBase*Content)
				{
					Head = Resources->StatusPool.CreateAndAppend();
					Tail = Resources->StatusPool.CreateAndAppend();
					if (Content)
					{
						Edge* edge = Resources->EdgePool.CreateAndAppend();
						edge->Content = Content;
						Connect(edge, this->Head, this->Tail);
					}
				}
				//将This,R串联起来
				void Joint(NFA*Right) {
					Connect(Resources->EdgePool.CreateAndAppend(), this->Tail, Right->Head);
				}
				//将Target并联起来
				void Parallel(NFA*Target) {
					Connect(Resources->EdgePool.CreateAndAppend(), this->Head, Target->Head);
					Connect(Resources->EdgePool.CreateAndAppend(), Target->Tail, this->Tail);
				}
				//构造重复匹配
				void Repeat(NFA*Target, int Min, int Max) {

				}
				//用边将两个状态连接起来
				static void Connect(Edge*ConnectEdge, Status*From, Status*To) {
					ConnectEdge->From = From;
					ConnectEdge->To = To;
					From->OutEdges.Add(ConnectEdge);
					To->InEdges.Add(ConnectEdge);
				}
			};
			//语法分析器
			class Parser
			{
				Tokenizer m_tokenizer;
				Resource*m_resource = new Resource();
				//Parse字符集合
				NFA* ParseSet() {
					NFA* out = m_resource->NFAPool.CreateAndAppend(m_resource);
					out->BuildNode(nullptr);
					Token token;
					m_tokenizer.Peek(token);//Peek一个
					bool not= false;//取反
					int type = 0;//0为拼接，1是范围，2是内建集合
					if (token.Type == TokenType::Pow)//取反
					{
						not= true;
						m_tokenizer.Consume(token);//前进
					}
					//判定Set类型（纯字符拼接，还是范围(包括内建集合)）
					m_tokenizer.Peek(token);
					if (token.Type == TokenType::Escape)//内建集合
						type = 2;
					else
					{
						if (*(token.Begin + 1) == L'-')//范围第二个字符必为-
							type = 1;
					}
					switch (type)
					{
					case 0:
						while (m_tokenizer.Done()) {
							m_tokenizer.Consume(token);
							switch (token.Type)
							{
							case TokenType::Char:
							{
								NFA*node = m_resource->NFAPool.CreateAndAppend(m_resource);
								node->BuildNode(m_resource->ContentPool.CreateAndAppend(*token.Begin, not));
								out->Parallel(node);
							}
							case TokenType::RBracket:
								return out;
							}
						}
						return nullptr;
					case 1:
						NFA* node = nullptr;
						bool range = false;
						wchar_t left = '\0';
						while (!m_tokenizer.Done())
						{
							m_tokenizer.Consume(token);
							switch (token.Type)
							{
							case TokenType::Char:
								if (!range)
									left = *token.Begin;
								else
								{
									range = false;
									node = m_resource->NFAPool.CreateAndAppend(m_resource);
									node->BuildNode(m_resource->ContentPool.CreateAndAppendWith<CharRange>(left, *token.Begin, not));
								}
								break;
							case TokenType::Minus:
								range = true; break;
							case TokenType::Pow:
								not= true; break;
							case TokenType::Escape://其他转义符
								m_tokenizer.Consume(token); break;
								//TODO
							case TokenType::RBracket:
								return out;
							}
							if (node)
								out->Parallel(node);
						}
						return nullptr;
					}
					return nullptr;//预期在结束前返回
				}
				//[Base,Top)区间进行Parse
				NFA* ParseNFA(index_t Base, index_t Top)
				{
					m_tokenizer.Position(Base);//定位

					NFA* out = m_resource->NFAPool.CreateAndAppend(m_resource);
					out->BuildNode(nullptr);
					NFA* iter = nullptr;
					NFA* node = nullptr;

					Token token;
					while (!m_tokenizer.Done() && m_tokenizer.Position() < Top)
					{
						m_tokenizer.Consume(token);
						switch (token.Type)
						{
						case TokenType::Char:
						{
							node = m_resource->NFAPool.CreateAndAppend(m_resource);//单字符NFA创建
							node->BuildNode(m_resource->ContentPool.CreateAndAppend(*token.Begin));
							if (m_tokenizer.Position() + 1 < Top)//Peek边界检查
							{
								index_t offset = 0;
								index_t current = m_tokenizer.Position();
								NFA*suffix = Suffix(current - 1, current, offset);
								if (suffix != nullptr)//后缀捕获成功
								{
									node = suffix;
									m_tokenizer.Position(current + offset);//重新定位
								}
							}
							break;
						}
						case TokenType::Or://并联
							node = ParseNFA(m_tokenizer.Position(), Top);
							out->Parallel(node);//并联
							break;
						case TokenType::LBracket:
							node = ParseSet(); //字符集
							break;
						case TokenType::LParen:
						{
							index_t before = m_tokenizer.Position();
							node = ParseNFA(m_tokenizer.Position(), Top);
							index_t after = m_tokenizer.Position();
							if (m_tokenizer.Position() + 1 < Top)//Peek边界检查
							{
								index_t offset = 0;
								NFA*suffix = Suffix(before, after, offset);
								if (suffix != nullptr)
								{
									node = suffix;
									m_tokenizer.Position(after + offset);//重新定位
								}
								break;
							}
						}
						case TokenType::RParen://右小括号，分组结束，退出SubParse
							break;
						}
						if (iter != nullptr)
							NFA::Connect(m_resource->EdgePool.CreateAndAppend(), iter->Tail, node->Head);//除开第一次，进行前后连接
						else
							NFA::Connect(m_resource->EdgePool.CreateAndAppend(), out->Head, node->Head);//第一次进行头头连接
						iter = node;
					}
					NFA::Connect(m_resource->EdgePool.CreateAndAppend(), iter->Tail, out->Tail);//尾部相连
					return out;
				}
				//反复
				NFA* Repeat(index_t Base,index_t Top, int Min, int Max)
				{
					NFA*ret = m_resource->NFAPool.CreateAndAppend(m_resource);
					ret->BuildNode(nullptr);
					//处理头
					NFA*head = ParseNFA(Base, Top);
					NFA*tail = nullptr;
					NFA::Connect(m_resource->EdgePool.CreateAndAppend(), ret->Head, head->Head);
					if (Min == 1)
						NFA::Connect(m_resource->EdgePool.CreateAndAppend(), head->Tail, ret->Tail);
					else if (Min > 1)
					{
						NFA*prev = head;
						while (Min - 2 >= 0)//中间节点
						{
							NFA*mid = ParseNFA(Base, Top);
							prev->Joint(mid);
							prev = mid;
						}
						tail = ParseNFA(Base, Top);
						prev->Joint(tail);
						NFA::Connect(m_resource->EdgePool.CreateAndAppend(), tail->Tail, ret->Tail);
					}
					else//零次
						tail = head;
					//无限循环
					if (Max == -1)
					{
						NFA::Connect(m_resource->EdgePool.CreateAndAppend(), tail->Tail, tail->Head);//自环
						NFA::Connect(m_resource->EdgePool.CreateAndAppend(), tail->Tail, ret->Tail);//指向结尾
					}
					else
					{
						NFA*prev = tail;
						while (Max - Min - 1 >= 0)//中间节点
						{
							NFA*mid = ParseNFA(Base, Top);
							NFA::Connect(m_resource->EdgePool.CreateAndAppend(), mid->Tail, ret->Tail);//添加边连接起来
							prev->Joint(mid);
							prev = mid;
						}
						tail = ParseNFA(Base, Top);
						prev->Joint(tail);
						NFA::Connect(m_resource->EdgePool.CreateAndAppend(), tail->Tail, ret->Tail);//指向结尾
					}
					return ret;
				}
				//Parse后缀
				NFA* Suffix(index_t Base, index_t Top, index_t&Offset)
				{
					NFA*ret = nullptr;
					int left = 0;
					int right = 0;
					Token token;
					m_tokenizer.Consume(token);
					Offset++;//假定为0
					switch (token.Type)
					{
					case TokenType::LCurly:
					{
						size_t left_count = 0;
						size_t right_count = 0;
						left_count = StringFunction::IntSniff(token.Begin + 1);//嗅探最小次数
						if (left_count > 0)
							left = StringFunction::StringToInt<int>(token.Begin + 1, left_count);
						else
							return nullptr;
						m_tokenizer.Move(left_count);//跳过继续
						m_tokenizer.Consume(token);
						Offset += left_count + 1;
						if (token.Type == TokenType::RCurly)//如果是右括号,返回
						{
							right = left; 
							break;
						}
						else if (token.Type == TokenType::Comma)//逗号检查
						{
							right_count = StringFunction::IntSniff(token.Begin + 1);
							if (right_count > 0)
								right = StringFunction::StringToInt<int>(token.Begin + 1, right_count);
							else//如果没有数字，就是上限不限
								right = -1;
							m_tokenizer.Consume(token);
							Offset += right_count + 1;
							if (token.Type != TokenType::RCurly)//右花括号检查
								return nullptr;
							break;
						}
					}
					case TokenType::Mul:
					{
						left = 0;
						right = -1;
						break;
					}
					case TokenType::Plus:
					{
						left = 1;
						right = -1;
						break;
					}
					case TokenType::QMark:
					{
						left = 0;
						right = 1;
						break;
					}
					default:
						m_tokenizer.Move(-1);
						return nullptr;
					}
					ret = Repeat(Base, Top, left, right);
					return ret;
				}
				//Parse前缀
				NFA* Prefix() {
					//TODO (功能边，例如捕获，后期完成
				}
			public:
				Parser(String const&Source) :m_tokenizer(Source.GetData(), Source.Count()) {}
				Parser(wchar_t const* Source, size_t Size) :m_tokenizer(Source, Size) {}
				NFA* Parse()
				{
					return ParseNFA(0, m_tokenizer.Count());
				}
			};

			class DFA
			{

			};
		}
	};
};