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
				Escape,
				//字符串结尾
				EOS
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
					PeekToken(Type);
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
					case L'\n':
						return SetToken(TokenType::EOS);
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
					case L'\n':
						return PeekToken(TokenType::EOS);
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
				Pool(Pool &&lhs) :m_objects(Move(lhs.m_objects)) {}
				Pool(Pool const& rhs) :m_objects(rhs.m_objects) {}
				Pool& operator=(Pool const&rhs) {
					this->m_objects = rhs.m_objects;
					return *this;
				}
				Pool& operator=(Pool &&lhs) {
					this->m_objects = Move(lhs);
					return *this
				}
				T* Add(T*Target) {
					m_objects.Add((intptr_t)Target, Target);
					return Target;
				}
				void Remove(T*Target) {
					m_objects.Remove((intptr_t)Target);
					delete Target;
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
				Generic::Dictionary<intptr_t, T*>&GetContainer() {
					return this->m_objects;
				}
				Generic::Dictionary<intptr_t, T*> const&GetContainer()const {
					return this->m_objects;
				}
				~Pool() {
					for (auto&item : m_objects)
					{
						delete item.Value();
						m_objects.Remove(item.Key());
					}
				}
			};
			//基础内容表示(连续字符集合)
			class BasicContent:public Interface::ICloneable
			{
			protected:
				bool m_is_complementary = false;
				bool m_any = false;
				wchar_t m_left;
				wchar_t m_right;
			public:
				BasicContent(BasicContent const&rhs) :m_is_complementary(rhs.m_is_complementary),
					m_any(rhs.m_any), m_left(rhs.m_left), m_right(rhs.m_right) {}
				BasicContent(bool Any) :m_any(true) {}
				BasicContent(wchar_t Target, bool Complementary = false) :m_left(Target), 
					m_right(Target), 
					m_is_complementary(Complementary) {}
				BasicContent(wchar_t Left, wchar_t Right, bool Complementary = false) :m_left(Left),
					m_right(Right),
					m_is_complementary(Complementary) {}
				inline wchar_t First()const {
					return m_left;
				}
				virtual bool Accept(wchar_t Target) {
					if (m_any)
						return true;
					if (!m_is_complementary)
						return (Target <= m_right && Target >= m_left);
					else
						return !(Target <= m_right && Target >= m_left);
				}
				virtual void* ClonePtr()const {
					return new BasicContent(*this);
				}
				virtual ~BasicContent() {}
			};
			//离散字符集合
			class InconsistentCharSet :public BasicContent
			{
				Generic::Dictionary<wchar_t, wchar_t> m_set;
			public:
				InconsistentCharSet(InconsistentCharSet const&rhs):m_set(rhs.m_set),
					BasicContent(rhs.m_left,rhs.m_right,rhs.m_is_complementary) {
				}
				InconsistentCharSet(bool Complementary = false) :BasicContent(L'\0', Complementary) {}
				inline void Add(wchar_t Target) {
					if (this->m_left == L'\0')
						this->m_left = Target;
					m_set.Add(Target, Target);
				}
				virtual void* ClonePtr()const {
					return new InconsistentCharSet(*this);
				}
				virtual bool Accept(wchar_t Target) {
					if (!this->m_is_complementary)
						return m_set.Contains(Target);
					else
						return !m_set.Contains(Target);
				}
				virtual ~InconsistentCharSet() {}
			};
			//聚合字符集合
			class CompoundCharSet :public BasicContent
			{
				Generic::Array<BasicContent*> m_set;
			public:
				CompoundCharSet(CompoundCharSet const&rhs) :BasicContent(rhs.m_left, rhs.m_right, rhs.m_is_complementary) {
					for (index_t i = 0; i < rhs.m_set.Count(); ++i)
						m_set.Add(static_cast<BasicContent*>(rhs.m_set[i]->ClonePtr()));
				}
				CompoundCharSet(bool Complementary = false) :BasicContent(L'\0', Complementary) {}
				virtual bool Accept(wchar_t Target) {
					for (index_t i = 0; i < m_set.Count(); ++i)
						if (m_set[i]->Accept(Target))return true;
					return false;
				}
				inline void Add(BasicContent* Target) {
					m_set.Add(Target);
				}
				virtual void* ClonePtr()const {
					return new CompoundCharSet(*this);
				}
				virtual ~CompoundCharSet() {
					for (index_t i = 0; i < m_set.Count(); ++i)
						if (m_set[i])delete m_set[i];
					m_set.Clear();
				}
			};
			//内建字符集合
			class BuiltInCharSet
			{
			public:
				//空白字符集合即:\\s
				static BasicContent* BlankSet(bool Complementary=false) {
					InconsistentCharSet* ret = new InconsistentCharSet(Complementary);
					ret->Add('\f');
					ret->Add('\n');
					ret->Add('\r');
					ret->Add('\t');
					ret->Add('\v');
					return ret;
				}
				//非空白字符集合即:\\S
				static BasicContent* ComplementaryBlankSet(bool Complementary = false) {
					return BlankSet(!Complementary);
				}
				//数字字符集合即:\\d
				static BasicContent* DigitSet(bool Complementary = false) {
					return new BasicContent(L'0', L'9', Complementary);
				}
				//非数字字符集合即:\\D
				static BasicContent* ComplementaryDigitSet(bool Complementary = false) {
					return new BasicContent(L'0', L'9', !Complementary);
				}
				static BasicContent* LowerAToZSet(bool Complementary = false) {
					return new BasicContent(L'a', L'z', Complementary);
				}
				static BasicContent* UpperAToZSet(bool Complementary = false) {
					return new BasicContent(L'A', L'Z', Complementary);
				}
				//单词字符集合即:\\w
				static CompoundCharSet* WordSet(bool Complementary = false) {
					CompoundCharSet*set = new CompoundCharSet(Complementary);
					set->Add(LowerAToZSet());
					set->Add(UpperAToZSet());
					set->Add(DigitSet());
					set->Add(new BasicContent(L'_'));
					return set;
				}
				//非单词字符集合即:\\W
				static CompoundCharSet* ComplementaryWordSet(bool Complementary = false) {
					return WordSet(!Complementary);
				}
			};

			struct CaptureInfo
			{
				wchar_t const* Base;
				size_t Size;
			};
			//捕获包
			struct CapturePackage
			{
				const wchar_t* Source = nullptr;
				Generic::Dictionary<String, CaptureInfo> NamedCapture;
				Generic::array<CaptureInfo> AnonymousCapture;
			public:
				CapturePackage(const wchar_t* Target, size_t AnonymousCaptureLength, Generic::Dictionary<String, index_t> const&NamedCaptureTable)
					:AnonymousCapture(AnonymousCaptureLength), Source(Target) {
					for (auto&var : NamedCaptureTable)
					{
						NamedCapture.Add(var.Key(), CaptureInfo{ 0,0 });
					}
				}
				void SetToZero() {
					for (auto&var : NamedCapture)
						var.Value().Size = 0;
					for (index_t i = 0; i < AnonymousCapture.Count(); ++i)
						AnonymousCapture[i].Size = 0;
				}
			};
			//捕获
			class Capture
			{
				String m_name;
				index_t m_anonymous_index = -1;
			public:
				Capture(index_t AnonymousIndex) :m_anonymous_index(AnonymousIndex){}
				Capture(String const&Name) :m_name(Name) {}
				bool Go(CapturePackage*Package, wchar_t const* Current) {
					if (Package == nullptr)
						return false;
					if (m_anonymous_index != -1)
					{
						CaptureInfo&info = Package->AnonymousCapture[m_anonymous_index];
						if (info.Size == 0)
							info.Base = Current;
						info.Size++;
					}
					else
					{
						CaptureInfo&info = Package->NamedCapture[m_name];
						if (info.Size == 0)
							info.Base = Current;
						info.Size++;
					}
					return true;
				}
				void WithDraw(CapturePackage*Package) {
					if (Package == nullptr)
						return;
					if (m_anonymous_index != -1)
						Package->AnonymousCapture[m_anonymous_index].Size--;
					else
						Package->NamedCapture[m_name].Size--;
				}
				~Capture(){}
			};
			//边
			class Edge
			{
			public:
				Edge(){}
				Edge(BasicContent*Target) :Content(Target) {}
				//起始状态
				Status*From = nullptr;
				//目标状态
				Status*To = nullptr;
				//匹配内容
				BasicContent*Content = nullptr;
				//是否有效
				bool Valid = true;
				//是否为自环边
				bool LoopEdge = false;
				//动作
				Generic::Array<Capture*> Actions;
			};
			//状态
			class Status
			{
			public:
				//进入状态的边
				Generic::Array<Edge*> InEdges;
				//出去的边
				Generic::Array<Edge*> OutEdges;
				//是否是结束状态
				bool Final = false;
				//是否有效
				bool Valid = false;
				//是否访问过,用于Debug
				bool Visited = false;
				//等效结束
				bool EquivalentFinal = false;
			};
			//资源
			class NFAResource
			{
			public:
				Pool<NFA> NFAPool;
				Pool<BasicContent> ContentPool;
				Pool<Edge> EdgePool;
				Pool<Status> StatusPool;
				Pool<Capture> CapturePool;
			};
			//NFA状态机
			class NFA
			{
				NFAResource*Resources;
			public:
				NFA(NFAResource*Origin) :Resources(Origin), Head(nullptr), Tail(nullptr) {}
				NFA() :Head(nullptr), Tail(nullptr) {}
				Status*Head;
				Status*Tail;
				//构造节点
				void BuildNode(BasicContent*Content)
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
				//将Target并联起来
				void Parallel(Edge*Target) {
					Connect(Target, this->Head, this->Tail);
				}
				//用边将两个状态连接起来
				static void Connect(Edge*ConnectEdge, Status*From, Status*To) {
					ConnectEdge->From = From;
					ConnectEdge->To = To;
					From->OutEdges.Add(ConnectEdge);
					To->InEdges.Add(ConnectEdge);
				}
			};
			//转义类型
			enum class EscapeType
			{
				//内建字符集合转义
				BuiltInCharSet,
				//关键字转义
				KeyWord
			};
			//语法分析器
			class Parser
			{
				Tokenizer m_tokenizer;
				NFAResource*m_resource = nullptr;
				Generic::Array<Capture*> m_capture_stack;
				Generic::Dictionary<index_t, index_t> m_anonymous_index_table;
				Generic::Dictionary<String, index_t> m_named_index_table;
			private:
				EscapeType GetEscapeType() {
					Token token;
					m_tokenizer.Peek(token);
					if (Algorithm::Any(*token.Begin, L'S', L's', L'd', L'D', L'W', L'w'))
						return EscapeType::BuiltInCharSet;
					else if (Algorithm::Any(*token.Begin,
						L'{', L'}', L'[', L']', L'(', L')', L'+', L'?', L'*', L'\\', L'-'))
						return EscapeType::KeyWord;
					//TODO 异常
					return EscapeType::KeyWord;
				}
				//内建字符集合
				NFA* BuiltInSet() {
					NFA* out = NewNFA();
					out->BuildNode(nullptr);
					Token token;
					m_tokenizer.Consume(token);
					switch (*token.Begin)
					{
					case L'S':
						out->Parallel(SetCapture(NewEdge(this->m_resource->ContentPool.Add(BuiltInCharSet::ComplementaryBlankSet())))); break;
					case L's':
						out->Parallel(SetCapture(NewEdge(this->m_resource->ContentPool.Add(BuiltInCharSet::BlankSet())))); break;
					case L'd':
						out->Parallel(SetCapture(NewEdge(this->m_resource->ContentPool.Add(BuiltInCharSet::DigitSet())))); break;
					case L'D':
						out->Parallel(SetCapture(NewEdge(this->m_resource->ContentPool.Add(BuiltInCharSet::ComplementaryDigitSet())))); break;
					case L'w':
						out->Parallel(SetCapture(NewEdge(this->m_resource->ContentPool.Add(BuiltInCharSet::WordSet())))); break;
					case L'W':
						out->Parallel(SetCapture(NewEdge(this->m_resource->ContentPool.Add(BuiltInCharSet::ComplementaryWordSet())))); break;
					default:
						break;
					}
					return out;
				}
				//Parse字符集合
				NFA* ParseSet() {
					NFA* out = NewNFA();
					out->BuildNode(nullptr);
					Token token;
					m_tokenizer.Peek(token);//Peek一个
					bool not= false;//取反
					if (token.Type == TokenType::Pow)//取反
					{
						not= true;
						m_tokenizer.Consume(token);//前进
					}
					while (!m_tokenizer.Done())
					{
						m_tokenizer.Consume(token);
						if (token.Type == TokenType::RBracket)//结束退出
							break;
						if (token.Type == TokenType::Escape)//是转义符的话
							out->Parallel(BuiltInSet());
						else
						{
							wchar_t left = *token.Begin;
							wchar_t right = L'\0';
							m_tokenizer.Peek(token);//是否为连续集合
							if (token.Type == TokenType::Minus)
							{
								m_tokenizer.Move(1);//移动到Range右侧边界
								m_tokenizer.Consume(token);
								right = *token.Begin;
							}
							else
								right = left;
							out->Parallel(NewEdge(this->m_resource->ContentPool.CreateAndAppend(left, right, not)));
						}
					}
					return out;
				}
				//在[Base,Top)区间进行最小单位Parse
				NFA* ParseUnit(index_t Base, index_t Top, bool& Single, Token&Last) {
					Token token;
					m_tokenizer.Position(Base);//定位
					m_tokenizer.Peek(token);
					if (!Algorithm::Any(token.Type, TokenType::Char, TokenType::Escape))
					{
						m_tokenizer.Consume(token);
						Last = token;
						return nullptr;
					}
					NFA* out = NewNFA();
					out->BuildNode(nullptr);
					NFA* iter = nullptr;
					NFA* node = nullptr;
					index_t before = Base;
					index_t after = Base;
					while (!m_tokenizer.Done() && m_tokenizer.Position() < Top)
					{
						before = m_tokenizer.Position();
						m_tokenizer.Consume(token);
						switch (token.Type)
						{
						case TokenType::Char:
							node = m_resource->NFAPool.CreateAndAppend(m_resource);//单字符NFA创建
							node->BuildNode(nullptr);
							if (*token.Begin == L'.')//任意匹配
								node->Parallel(SetCapture(NewEdge(m_resource->ContentPool.CreateAndAppend(true))));
							else
								node->Parallel(SetCapture(NewEdge(m_resource->ContentPool.CreateAndAppend(*token.Begin))));
							break;
						case TokenType::Escape:
							switch (GetEscapeType())
							{
							case EscapeType::BuiltInCharSet:
								node = this->BuiltInSet();
								break;
							case EscapeType::KeyWord:
								m_tokenizer.Consume(token);
								node = m_resource->NFAPool.CreateAndAppend(m_resource);
								node->BuildNode(nullptr);
								node->Parallel(SetCapture(NewEdge(m_resource->ContentPool.CreateAndAppend(*token.Begin))));
								break;
							}
							break;
						default:
							Last = token;
							goto Exit;
						}
						after = m_tokenizer.Position();
						//单字符后缀处理
						if (m_tokenizer.Position() < Top)//Peek边界检查
						{
							index_t offset = 0;
							index_t current = m_tokenizer.Position();
							NFA*suffix = Suffix(node, before, after, offset);
							if (suffix != nullptr)//后缀捕获成功
							{
								node = suffix;
								m_tokenizer.Position(current + offset);//重新定位
								Single = true;

								if (iter != nullptr)
									NFA::Connect(NewEdge(), iter->Tail, node->Head);//除开第一次，进行前后连接
								else
									NFA::Connect(NewEdge(), out->Head, node->Head);//第一次进行头头连接
								iter = node;

								goto Exit;
							}
						}
						if (iter != nullptr)
							NFA::Connect(NewEdge(), iter->Tail, node->Head);//除开第一次，进行前后连接
						else
							NFA::Connect(NewEdge(), out->Head, node->Head);//第一次进行头头连接
						iter = node;
					}
				Exit:
					NFA::Connect(NewEdge(), iter->Tail, out->Tail);//尾部相连
					if (m_tokenizer.Done())
						Last.Type = TokenType::EOS;
					return out;
				}
				//在[Base,Top)区间进行Parse
				NFA* ParseNFA(index_t Base, index_t Top) {
					m_tokenizer.Position(Base);
					Token token;
					NFA*iter = nullptr;
					NFA*node = nullptr;
					NFA* out = NewNFA();
					out->BuildNode(nullptr);
					bool single = false;
					index_t before = Base;
					index_t after = Base;
					while (!m_tokenizer.Done() && m_tokenizer.Position() < Top)
					{
						before = m_tokenizer.Position();//记录子Parse开始
						node = ParseUnit(m_tokenizer.Position(), Top, single, token);
						switch (token.Type)
						{
						case TokenType::LBracket:
							node = ParseSet(); break;
						case TokenType::LParen:
							if (node != nullptr)//判断是不是第一个外层(
							{
								if (iter != nullptr)
									NFA::Connect(NewEdge(), iter->Tail, node->Head);//除开第一次，进行前后连接
								else
									NFA::Connect(NewEdge(), out->Head, node->Head);//第一次进行头头连接
								iter = node;
							}
							before = m_tokenizer.Position() - 1;
							node = ParseCapture(m_tokenizer.Position(), Top);
							break;
						case TokenType::Or:
							node = Parallel(node, m_tokenizer.Position(), Top); break;
						case TokenType::RParen:
							if (iter == nullptr)
							{
								NFA::Connect(NewEdge(), out->Head, node->Head);//第一次进行头头连接
								iter = node;
							}
							goto Exit;
						default:
							break;
						}
						after = m_tokenizer.Position();//记录子Parse结束
						//后缀检查
						if (m_tokenizer.Position() < Top && !single)//Peek边界检查
						{
							index_t offset = 0;
							NFA*suffix = Suffix(node, before, after, offset);
							if (suffix != nullptr)//后缀捕获成功
							{
								node = suffix;
								m_tokenizer.Position(after + offset);//重新定位
							}
						}
						if (iter != nullptr)
							NFA::Connect(NewEdge(), iter->Tail, node->Head);//除开第一次，进行前后连接
						else
							NFA::Connect(NewEdge(), out->Head, node->Head);//第一次进行头头连接
						iter = node;
					}
				Exit:
					NFA::Connect(NewEdge(), iter->Tail, out->Tail);//尾部相连
					return out;
				}
				//设置捕获
				Edge* SetCapture(Edge*Target) {
					for (index_t i = 0; i < m_capture_stack.Count(); ++i)
						Target->Actions.Add(m_capture_stack[i]);
					return Target;
				}
				inline void PushCapture(Capture*Target)
				{
					m_capture_stack.Add(Target);
				}
				inline void PopCapture() {
					m_capture_stack.Pop();
				}
				//并联
				NFA* Parallel(NFA*Parsed, index_t Base, index_t Top) {
					NFA* out = NewNFA();
					out->BuildNode(nullptr);
					out->Parallel(Parsed);
					NFA* iter = Parsed;
					bool single = false;
					Token token;
					while (!m_tokenizer.Done() && m_tokenizer.Position() < Top)
					{
						iter = ParseUnit(m_tokenizer.Position(), Top, single, token);
						if (token.Type == TokenType::Or)
							out->Parallel(iter);
						else if (token.Type == TokenType::RParen || m_tokenizer.Done())
						{
							out->Parallel(iter);
							break;
						}
						else {
							//Exception
						}
					}
					return out;
				}
				//反复
				NFA* Repeat(NFA*Parsed,index_t Base,index_t Top, int Min, int Max)
				{
					NFA*ret = m_resource->NFAPool.CreateAndAppend(m_resource);
					ret->BuildNode(nullptr);
					NFA*tail = nullptr;
					

					if (Min == 1)
					{
						NFA* node = Parsed;
						NFA::Connect(NewEdge(), ret->Head, node->Head);
						tail = node;
					}
					else if (Min > 1)
					{
						NFA* node = Parsed;
						NFA::Connect(NewEdge(), ret->Head, node->Head);
						NFA*prev = node;
						for (index_t i = Min - 2; i > 0; i--)//中间节点
						{
							NFA*mid = ParseNFA(Base, Top);
							prev->Joint(mid);
							prev = mid;
						}
						tail = ParseNFA(Base, Top);
						prev->Joint(tail);
					}

					if (Max - Min == 1)
					{
						NFA*node = Parsed;
						if (Min > 0)
							node = ParseNFA(Base, Top);
						if (tail == nullptr)
							NFA::Connect(NewEdge(), ret->Head, node->Head);
						else
							tail->Joint(node);
						NFA::Connect(NewEdge(), node->Head, ret->Tail);//可跳过
						NFA::Connect(NewEdge(), node->Tail, ret->Tail);//结尾连接
					}
					else if (Max == -1)
					{
						NFA*node = Parsed;
						if (Min > 0)
							node = ParseNFA(Base, Top);
						Status*hollow =NewStatus();
						NFA::Connect(NewEdge(), hollow, node->Head);
						NFA::Connect(NewEdge(), node->Tail, hollow);
						if (tail == nullptr)
							NFA::Connect(NewEdge(), ret->Head, hollow);
						else
							NFA::Connect(NewEdge(), tail->Tail, hollow);
						NFA::Connect(NewEdge(), hollow, ret->Tail);//结尾连接
					}
					else if (Max - Min > 1)
					{
						NFA*node = Parsed;
						if (Min > 0)
							node = ParseNFA(Base, Top);
						if (tail == nullptr)
							NFA::Connect(NewEdge(), ret->Head, node->Head);
						else
							tail->Joint(node);
						NFA::Connect(NewEdge(), node->Head, ret->Tail);//可跳过
						NFA*prev = node;
						for (index_t i = Max - Min - 2; i > 0; i--)//中间节点
						{
							NFA*mid = ParseNFA(Base, Top);
							NFA::Connect(NewEdge(), mid->Head, ret->Tail);//可跳过
							prev->Joint(mid);
							prev = mid;
						}
						tail = ParseNFA(Base, Top);
						prev->Joint(tail);
						NFA::Connect(NewEdge(), tail->Head, ret->Tail);//可跳过
						NFA::Connect(NewEdge(), tail->Tail, ret->Tail);//结尾连接
					}
					else if (Max - Min == 0)
					{
						NFA::Connect(NewEdge(), tail->Tail, ret->Tail);
					}
					return ret;
				}
				//Parse后缀
				NFA* Suffix(NFA*Parsed,index_t Base, index_t Top, index_t&Offset)
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
							m_tokenizer.Consume(token);
							right_count = StringFunction::IntSniff(token.Begin);
							if (right_count > 0)
								right = StringFunction::StringToInt<int>(token.Begin, right_count);
							else//如果没有数字，就是上限不限
								right = -1;
							m_tokenizer.Move(right_count - 1);
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
					m_tokenizer.Peek(token);
					if (token.Type == TokenType::QMark)//非贪婪匹配
					{
						//TODO
					}
					ret = Repeat(Parsed, Base, Top, left, right);
					return ret;
				}
				//Parse捕获组
				NFA* ParseCapture(index_t Base,index_t Top) {
					m_tokenizer.Position(Base);
					Token token;
					m_tokenizer.Peek(token);
					if (token.Type != TokenType::QMark)
					{
						if (!m_anonymous_index_table.Contains(Base - 1))
							m_anonymous_index_table.Add(Base - 1, m_anonymous_index_table.Count());
						PushCapture(NewCapture(m_anonymous_index_table[Base - 1]));
						NFA*ret = ParseNFA(Base, Top);
						PopCapture();
						return ret;
					}
					else
						m_tokenizer.Consume(token);
					m_tokenizer.Consume(token);
					if (*token.Begin == L'<')
					{
						const wchar_t*end = StringFunction::GoUntil(token.Begin + 1, L'>');
						size_t count = end - token.Begin - 1;
						String name;
						name.Append(token.Begin + 1, 0, count);
						if (!m_named_index_table.Contains(name))
							m_named_index_table.Add(name, Base - 1);
						PushCapture(NewCapture(name));
						NFA*ret = ParseNFA(m_tokenizer.Position() + count + 1, Top);
						PopCapture();
						return ret;
					}
					else if (*token.Begin == L':')
						return ParseNFA(m_tokenizer.Position(), Top);
					else
					{
						//Exception
					}
					return nullptr;
				}
				//新边
				inline Edge* NewEdge() {
					return m_resource->EdgePool.CreateAndAppend();
				}
				inline Edge* NewEdge(BasicContent*Content) {
					return m_resource->EdgePool.CreateAndAppend(Content);
				}
				//新状态
				inline Status* NewStatus() {
					return m_resource->StatusPool.CreateAndAppend();
				}
				inline NFA* NewNFA() {
					return m_resource->NFAPool.CreateAndAppend(m_resource);
				}
				template<class...Args>
				inline Capture* NewCapture(Args&&...args) {
					return this->m_resource->CapturePool.CreateAndAppend(args...);
				}
			public:
				Parser(String const&Source,NFAResource*Resource) :m_tokenizer(Source.GetData(), Source.Count()),m_resource(Resource) {}
				Parser(wchar_t const* Source, size_t Size, NFAResource*Resource) :m_tokenizer(Source, Size), m_resource(Resource) {}
				inline NFA* Parse()
				{
					return ParseNFA(0, m_tokenizer.Count());
				}
				inline Generic::Dictionary<index_t, index_t>const& AnonymousCaptureTable()const {
					return this->m_anonymous_index_table;
				}
				inline Generic::Dictionary<String, index_t>const& NamedCaptureTable()const {
					return this->m_named_index_table;
				}
				~Parser() {}
			};
			//闭包
			struct Closure
			{
				Status*Target;
				Edge*ValidEdge;
			};
			//NFA简化
			class NFASimplify
			{
			public:
				//标记所有有效状态
				static void MarkValidStatus(Pool<Status> &Target) {
					for (auto&var : Target.GetContainer())
					{
						for (index_t i = 0; i < var.Value()->InEdges.Count(); ++i)
						{
							if (var.Value()->InEdges[i]->Content != nullptr
								|| var.Value()->InEdges[i]->LoopEdge)
							{
								var.Value()->Valid = true;
								break;
							}
						}
					}
				}
				//获得状态的所有Depth超过1的闭包(若要指定所有的包将Depth设置为-1)
				static void GetClosure(Generic::Array<Closure>&Out, Status*Where, index_t Depth = 0) {
					for (index_t i = 0; i < Where->OutEdges.Count(); i++) {
						if (Where->OutEdges[i]->Content != nullptr
							|| Where->OutEdges[i]->To->Final)
						{
							if (Depth < 1)
								continue;
							Closure closure;
							closure.Target = Where->OutEdges[i]->To;
							closure.ValidEdge = Where->OutEdges[i];
							Out.Add(closure);
						}
						else
						{
							Where->OutEdges[i]->Valid = false;
							GetClosure(Out, Where->OutEdges[i]->To, Depth + 1);
						}
					}
				}
				//标记等效结束状态
				static void MarkEquivalentEndStatus(Pool<Status> &Target) {
					for (auto&var : Target.GetContainer())
					{
						if (!var.Value()->Valid)
							continue;
						for (index_t i = 0; i < var.Value()->OutEdges.Count(); ++i)
							if (var.Value()->OutEdges[i]->To->Final)
								var.Value()->EquivalentFinal = true;
					}
				}
				//移除无效边与状态
				static void RemoveInvalidStatusAndEdges(NFAResource*Re) {
					for (auto&var : Re->StatusPool.GetContainer()) 
					{
						if (var.Value()->Valid)
						{
							index_t offset = 0;
							for (index_t i = 0; i < var.Value()->OutEdges.Count(); ++i) {
								if (!var.Value()->OutEdges[i + offset]->Valid)
								{
									var.Value()->OutEdges.RemoveAt(i + offset);
									offset--;
								}
							}
						}
						else
							Re->StatusPool.Remove(var.Value());
					}
					for (auto&var : Re->EdgePool.GetContainer()) {
						if (!var.Value()->Valid)
							Re->EdgePool.Remove(var.Value());
					}
				}
				//简化NFA
				static NFA* SimplifyNFA(NFAResource*Pool,NFA*Target) {
					MarkValidStatus(Pool->StatusPool);
					for (auto&var : Pool->StatusPool.GetContainer())
					{
						if (!var.Value()->Valid)
							continue;
						Generic::Array<Closure> closures;
						GetClosure(closures, var.Value());
						for (index_t i = 0; i < closures.Count(); ++i) {
							index_t index = closures[i].Target->InEdges.IndexOf(closures[i].ValidEdge);
							if (index >= 0)
								closures[i].Target->InEdges[index]->Valid = false;
							Edge*edge = Pool->EdgePool.CreateAndAppend();
							edge->Actions = closures[i].ValidEdge->Actions;//动作拷贝
							if (closures[i].ValidEdge->Content != nullptr)//内容拷贝
							{
								edge->Content = static_cast<BasicContent*>(closures[i].ValidEdge->Content->ClonePtr()); 
								Pool->ContentPool.Add(edge->Content);
							}
							if (closures[i].Target == var.Value())//记录Loop边
								edge->LoopEdge = true;
							NFA::Connect(edge, var.Value(), closures[i].Target);
						}
					}
					RemoveInvalidStatusAndEdges(Pool);
					MarkEquivalentEndStatus(Pool->StatusPool);
					return Target;
				}
			};
			//匹配结果
			class MatchResult
			{
				index_t m_index;
				size_t m_length;
				Generic::Dictionary<String, String> m_named_capture;
				Generic::Array<String> m_anonymous_capture;
			public:
				MatchResult(index_t Index, size_t Length, CapturePackage const& Capture) :m_index(Index), m_length(Length) {
					for (auto&var : Capture.NamedCapture)
					{
						String str((size_t)0);
						str.Append(var.Value().Base, 0, var.Value().Size);
						m_named_capture.Add(var.Key(), str);
					}
					for (index_t i = 0; i < Capture.AnonymousCapture.Count(); ++i)
					{
						String str((size_t)0);
						str.Append(Capture.AnonymousCapture[i].Base, 0, Capture.AnonymousCapture[i].Size);
						m_anonymous_capture.Add(Move(str));
					}
				}
				String const& operator[](index_t Index)const {
					return m_anonymous_capture[Index];
				}
				String const& operator[](String const&Key)const {
					return m_named_capture[Key];
				}
				String const& GetAnonymousCapture(index_t Index)const {
					return m_anonymous_capture[Index];
				}
				String const& GetNamedCapture(String const&Key)const {
					return m_named_capture[Key];
				}
			};
			//NFA匹配
			class NFAMatch
			{
			public:
				static void CaptureGo(Edge*Target, const wchar_t*Current, CapturePackage*Package) {
					for (index_t i = 0; i < Target->Actions.Count(); ++i)
						Target->Actions[i]->Go(Package, Current);
				}
				
				static void CaptureWithdraw(Edge*Target, CapturePackage*Package) {
					for (index_t i = 0; i < Target->Actions.Count(); ++i)
						Target->Actions[i]->WithDraw(Package);
				}

				static const wchar_t* MatchUnit(const wchar_t*Target, const wchar_t*Top, Status*Where, CapturePackage*Package) {
					if (Where->Final || Where->EquivalentFinal)//检查是否到达最终状态
						return Target;
					if (Target == Top)
						if (Where->Final || Where->EquivalentFinal)
							return Target;
					if (Where->OutEdges.Count() == 1)
					{
						//单分支进行直接迭代
						Status* iter = Where;
						while (iter->OutEdges.Count() == 1) {
							if (iter->OutEdges[0]->Content != nullptr)
							{
								if (Package != nullptr)
									CaptureGo(iter->OutEdges[0], Target, Package);
								if (!iter->OutEdges[0]->Content->Accept(*Target))
								{
									if (Package != nullptr)
										CaptureWithdraw(iter->OutEdges[0], Package);
									return nullptr;
								}
								Target++;
							}
							iter = iter->OutEdges[0]->To;

							if (iter->Final || iter->EquivalentFinal)//检查是否到达最终状态
								return Target;
							if (Target == Top)
								if (iter->Final || iter->EquivalentFinal)
									return Target;
						}
						return MatchUnit(Target, Top, iter, Package);
					}
					else
					{
						//多分枝
						for (index_t i = 0; i < Where->OutEdges.Count(); ++i) {
							if (Where->OutEdges[i]->Content == nullptr)
							{
								const wchar_t*ret = MatchUnit(Target, Top, Where->OutEdges[i]->To, Package);
								if (ret != nullptr)
									return ret;
							}
							else if (Where->OutEdges[i]->Content->Accept(*Target))
							{
								if (Package != nullptr)
									CaptureGo(Where->OutEdges[i], Target, Package);
								const wchar_t*ret = MatchUnit(Target + 1, Top, Where->OutEdges[i]->To, Package);
								if (ret != nullptr)
									return ret;
								else
									if (Package != nullptr)
										CaptureWithdraw(Where->OutEdges[i], Package);
							}
						}
						return nullptr;
					}
				}
				static bool IsMatchUnit(const wchar_t*Target, const wchar_t*Top, Status*Where, CapturePackage*Package) {
					if (Where->Final || Where->EquivalentFinal)//检查是否到达最终状态
					{
						if (Target == Top)
							return true;
						if (Where->EquivalentFinal)
						{
							for (index_t i = 0; i < Where->OutEdges.Count(); ++i) {
								if (Where->OutEdges[i]->Content != nullptr&&Where->OutEdges[i]->Content->Accept(*Target))
								{
									if (Package != nullptr)
										CaptureGo(Where->OutEdges[i], Target, Package);
									if (IsMatchUnit(Target + 1, Top, Where->OutEdges[i]->To, Package))
										return true;
									else
										if (Package != nullptr)
											CaptureGo(Where->OutEdges[i], Target, Package);
								}
							}
						}
					}
					if (Target == Top)
						if (Where->Final || Where->EquivalentFinal)
							return true;
					for (index_t i = 0; i < Where->OutEdges.Count(); ++i) {
						if (Where->OutEdges[i]->Content == nullptr)
						{
							if (IsMatchUnit(Target, Top, Where->OutEdges[i]->To, Package))
								return true;
						}
						else if (Where->OutEdges[i]->Content->Accept(*Target))
						{
							if (Package != nullptr)
								CaptureGo(Where->OutEdges[i], Target, Package);
							if (IsMatchUnit(Target + 1, Top, Where->OutEdges[i]->To, Package))
								return true;
							else
								if (Package != nullptr)
									CaptureWithdraw(Where->OutEdges[i], Package);
						}
					}
					return false;
				}
				static UPointer::uptr<MatchResult> MatchFirst(const wchar_t*Target, const wchar_t*Top, Status*Where, size_t AnonymousLength, Generic::Dictionary<String, index_t>const&NamedTable)
				{
					const wchar_t*iter = Target;
					CapturePackage package(Target, AnonymousLength, NamedTable);
					while (iter != Top)
					{
						const wchar_t* ret = MatchUnit(iter, Top, Where, &package);
						if (ret != nullptr)
							return Reference::newptr<MatchResult>(iter - Target, ret - Target, package);
						iter++;
					}
					return nullptr;
				}
				static bool IsMatchFirst(const wchar_t*Target, const wchar_t*Top, Status*Where)
				{
					const wchar_t*iter = Target;
					while (iter != Top)
					{
						if (MatchUnit(iter, Top, Where, nullptr) != nullptr)
							return true;
						iter++;
					}
					return false;
				}
				static inline bool IsMatch(const wchar_t*Target, const wchar_t*Top, Status*Where)
				{
					return IsMatchUnit(Target, Top, Where, nullptr);
				}
			};
			class Visualize
			{
			public:
				static void DebugPrint(String Str, Status*Where) {
					if (Where->Visited && !Where->Final)
					{
						std::wcout << Str << L"(Loop/E)" << std::endl;
						return;
					}
					Where->Visited = true;
					for (int i = 0; i < Where->OutEdges.Count(); ++i) {
						String next = Str;
						wchar_t ch = '?';
						if (Where->OutEdges[i]->Content != nullptr)
							ch = Where->OutEdges[i]->Content->First();
						next += L"(Status)->";
						next.Append(ch);
						next += L"->";
						if (!Where->OutEdges[i]->LoopEdge)
							DebugPrint(next, Where->OutEdges[i]->To);
						else
						{
							Str += L"[Loop]";
							continue;
						}
					}
					if (Where->Final)
						std::wcout << Str << L"End" << std::endl;
				}
			};
			//正则表达式
			class Regex
			{
				String m_pattern;
				NFA* m_enter = nullptr;
				NFAResource * m_resource = nullptr;
				Parser *m_parser = nullptr;
			private:
				void Clear() {
					if (m_enter != nullptr)
						m_enter = nullptr;
					if (m_resource != nullptr) {
						delete m_resource;
						m_resource = nullptr;
					}
					if (m_parser) {
						delete m_parser;
						m_parser = nullptr;
					}
				}
				void From(String const&Pattern) {
					m_pattern = Pattern;
					m_resource = new NFAResource;
					m_parser = new Parser(m_pattern, m_resource);
					m_enter = m_parser->Parse();
					m_enter->Head->Valid = true;
					m_enter->Tail->Valid = true;
					m_enter->Tail->Final = true;
					NFASimplify::SimplifyNFA(m_resource, m_enter);
					
				}
			public:
				Regex(Regex&&lhs)noexcept {
					m_pattern = lhs.m_pattern;
					m_enter = lhs.m_enter;
					m_resource = lhs.m_resource;
					lhs.m_enter = nullptr;
					lhs.m_resource = nullptr;
				}
				Regex(Regex const&rhs) {
					From(rhs.m_pattern);
				}
				Regex(String const&Pattern) {
					From(Pattern);
				}
				Regex&operator=(Regex const&rhs) {
					Clear();
					From(rhs.m_pattern);
					return *this;
				}
				Regex&operator=(Regex&&lhs)
				{
					Clear();
					m_pattern = Move(lhs.m_pattern);
					m_enter = lhs.m_enter;
					m_resource = lhs.m_resource;
					lhs.m_enter = nullptr;
					lhs.m_resource = nullptr;
					return *this;
				}
				//是否匹配目标字符串(这会抛弃掉字符串捕获以加快速度)
				inline bool IsMatch(String const&Target)const {
					return NFAMatch::IsMatch(Target.GetData(), Target.GetData() + Target.Count(), m_enter->Head);
				}
				//是否匹配目标字符串(这会抛弃掉字符串捕获以加快速度)
				inline bool IsMatchFirst(String const&Target)const {
					return NFAMatch::IsMatchFirst(Target.GetData(), Target.GetData() + Target.Count(), m_enter->Head);
				}
				//匹配第一个字符串
				inline UPointer::uptr<MatchResult> MatchFirst(String const&Target)const {
					return NFAMatch::MatchFirst(Target.GetData(), Target.GetData() + Target.Count(), m_enter->Head, m_parser->AnonymousCaptureTable().Count(), m_parser->NamedCaptureTable());
				}
				//获取源构造字符串
				String const& GetPattern()const {
					return this->m_pattern;
				}
				~Regex() {
					Clear();
				}
			};
		}
	};
};