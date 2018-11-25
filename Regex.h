#pragma once
namespace HL
{
	namespace System
	{
		namespace RegularExpression
		{
			//��ǵ�����
			enum class TokenType
			{
				//�ַ�(������
				Char,
				//������
				LCurly,
				//�һ�����
				RCurly,
				//��С����
				LParen,
				//��С����
				RParen,
				//��������
				LBracket,
				//��������
				RBracket,
				//�Ӻ�
				Plus,
				//����
				Minus,
				//�˺�
				Mul,
				//����
				Comma,
				//ָ����
				Pow,
				//�ʺ�
				QMark,
				//ð��
				Colon,
				//����
				Or,
				//ת��
				Escape
			};
			//���
			class Token
			{
			public:
				wchar_t const* Begin;
				size_t Size;
				TokenType Type;
			};
			//�ʷ�������
			class Tokenizer
			{
				wchar_t const* m_source;
				index_t m_index;
				size_t m_size;
				Token* m_current;
			private:
				//�Ƿ�Ϊ�հ�
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
			//����Regex�����ڴ������
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
			//�������ݱ�ʾ(�����ַ�����)
			class BasicContent:public Interface::ICloneable
			{
			protected:
				bool m_is_complementary = false;
				bool m_any = false;
				wchar_t m_left;
				wchar_t m_right;
			public:
				BasicContent(BasicContent const&rhs) :m_is_complementary(rhs.m_is_complementary),
					m_any(rhs.m_any), m_left(rhs.m_left), m_right(m_right) {}
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
			//��ɢ�ַ�����
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
			//�ڽ��ַ�����
			class BuiltInCharSet
			{
			public:
				//�հ��ַ����ϼ�:\\s
				static BasicContent* BlankSet(bool Complementary=false) {
					InconsistentCharSet* ret = new InconsistentCharSet(Complementary);
					ret->Add('\f');
					ret->Add('\n');
					ret->Add('\r');
					ret->Add('\t');
					ret->Add('\v');
					return ret;
				}
				//�ǿհ��ַ����ϼ�:\\S
				static BasicContent* ComplementaryBlankSet(bool Complementary = false) {
					InconsistentCharSet* ret = new InconsistentCharSet(!Complementary);
					ret->Add('\f');
					ret->Add('\n');
					ret->Add('\r');
					ret->Add('\t');
					ret->Add('\v');
					return ret;
				}
				//�����ַ����ϼ�:\\d
				static BasicContent* DigitSet(bool Complementary = false) {
					return new BasicContent(L'0', L'9', Complementary);
				}
				//�������ַ����ϼ�:\\D
				static BasicContent* ComplementaryDigitSet(bool Complementary = false) {
					return new BasicContent(L'0', L'9', !Complementary);
				}
			};
			//��
			class Edge
			{
			public:
				Edge(){}
				Edge(BasicContent*Target) :Content(Target) {}
				//��ʼ״̬
				Status*From = nullptr;
				//Ŀ��״̬
				Status*To = nullptr;
				//ƥ������
				BasicContent*Content = nullptr;
				//�Ƿ���Ч
				bool Valid = true;
				//�Ƿ�Ϊ�Ի���
				bool LoopEdge = false;
			};
			//״̬
			class Status
			{
			public:
				//����״̬�ı�
				Generic::Array<Edge*> InEdges;
				//��ȥ�ı�
				Generic::Array<Edge*> OutEdges;
				//�Ƿ��ǽ���״̬
				bool Final = false;
				//�Ƿ���Ч
				bool Valid = false;
				//�Ƿ���ʹ�,����Debug
				bool Visited = false;
				//��Ч����
				bool EquivalentFinal = false;
			};
			//��Դ
			class NFAResource
			{
			public:
				Pool<NFA> NFAPool;
				Pool<BasicContent> ContentPool;
				Pool<Edge> EdgePool;
				Pool<Status> StatusPool;
			};
			//NFA״̬��
			class NFA
			{
				NFAResource*Resources;
			public:
				NFA(NFAResource*Origin) :Resources(Origin), Head(nullptr), Tail(nullptr) {}
				NFA() :Head(nullptr), Tail(nullptr) {}
				Status*Head;
				Status*Tail;
				//����ڵ�
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
				//��This,R��������
				void Joint(NFA*Right) {
					Connect(Resources->EdgePool.CreateAndAppend(), this->Tail, Right->Head);
				}
				//��Target��������
				void Parallel(NFA*Target) {
					Connect(Resources->EdgePool.CreateAndAppend(), this->Head, Target->Head);
					Connect(Resources->EdgePool.CreateAndAppend(), Target->Tail, this->Tail);
				}
				//��Target��������
				void Parallel(Edge*Target) {
					Connect(Target, this->Head, this->Tail);
				}
				//�ñ߽�����״̬��������
				static void Connect(Edge*ConnectEdge, Status*From, Status*To) {
					ConnectEdge->From = From;
					ConnectEdge->To = To;
					From->OutEdges.Add(ConnectEdge);
					To->InEdges.Add(ConnectEdge);
				}
			};
			//ת������
			enum class EscapeType
			{
				//�ڽ��ַ�����ת��
				BuiltInCharSet,
				//�ؼ���ת��
				KeyWord
			};
			//�﷨������
			class Parser
			{
				Tokenizer m_tokenizer;
				NFAResource*m_resource = nullptr;
			private:
				EscapeType GetEscapeType() {
					Token token;
					m_tokenizer.Peek(token);
					if (Algorithm::Any(*token.Begin, L'S', L's', L'd', L'D'))
						return EscapeType::BuiltInCharSet;
					else if (Algorithm::Any(*token.Begin,
						L'{', L'}', L'[', L']', L'(', L')', L'+', L'?', L'*', L'\\'))
						return EscapeType::KeyWord;
					//TODO �쳣
					return EscapeType::KeyWord;
				}
				//�ڽ��ַ�����
				NFA* BuiltInSet() {
					NFA* out = m_resource->NFAPool.CreateAndAppend(m_resource);
					out->BuildNode(nullptr);
					Token token;
					m_tokenizer.Consume(token);
					switch (*token.Begin)
					{
					case L'S':
						out->Parallel(NewEdge(this->m_resource->ContentPool.Add(BuiltInCharSet::ComplementaryBlankSet()))); break;
					case L's':
						out->Parallel(NewEdge(this->m_resource->ContentPool.Add(BuiltInCharSet::BlankSet()))); break;
					case L'd':
						out->Parallel(NewEdge(this->m_resource->ContentPool.Add(BuiltInCharSet::DigitSet()))); break;
					case L'D':
						out->Parallel(NewEdge(this->m_resource->ContentPool.Add(BuiltInCharSet::ComplementaryDigitSet()))); break;
					default:
						break;
					}
					return out;
				}
				//Parse�ַ�����
				NFA* ParseSet() {
					NFA* out = m_resource->NFAPool.CreateAndAppend(m_resource);
					out->BuildNode(nullptr);
					Token token;
					m_tokenizer.Peek(token);//Peekһ��
					bool not= false;//ȡ��
					if (token.Type == TokenType::Pow)//ȡ��
					{
						not= true;
						m_tokenizer.Consume(token);//ǰ��
					}
					while (!m_tokenizer.Done())
					{
						m_tokenizer.Consume(token);
						if (token.Type == TokenType::RBracket)//�����˳�
							break;
						if (token.Type == TokenType::Escape)//��ת����Ļ�
							out->Parallel(BuiltInSet());
						else
						{
							wchar_t left = *token.Begin;
							wchar_t right = L'\0';
							m_tokenizer.Peek(token);//�Ƿ�Ϊ��������
							if (token.Type == TokenType::Minus)
							{
								m_tokenizer.Move(1);//�ƶ���Range�Ҳ�߽�
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
				//[Base,Top)�������Parse,ParallelBreakָʾ�Ƿ�Բ������нض�
				NFA* ParseNFA(index_t Base, index_t Top, bool ParallelBreak = false)
				{
					m_tokenizer.Position(Base);//��λ
					bool successive = false;//����
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
							node = m_resource->NFAPool.CreateAndAppend(m_resource);//���ַ�NFA����
							if (*token.Begin == L'.')//����ƥ��
								node->BuildNode(m_resource->ContentPool.CreateAndAppend(true));
							else
								node->BuildNode(m_resource->ContentPool.CreateAndAppend(*token.Begin));
							if (m_tokenizer.Position() < Top)//Peek�߽���
							{
								index_t offset = 0;
								index_t current = m_tokenizer.Position();
								NFA*suffix = Suffix(node, current - 1, current, offset);
								if (suffix != nullptr)//��׺����ɹ�
								{
									node = suffix;
									m_tokenizer.Position(current + offset);//���¶�λ
								}
							}
							else if (successive)//����
								goto Exit;
							break;
						}
						case TokenType::Or://����
							if (ParallelBreak)
							{
								m_tokenizer.Move(-1);
								goto Exit;
							}
							successive = true;
							NFA::Connect(NewEdge(), iter->Tail, out->Tail);//��һ����NFAβ���Դ�������״̬,��������
							node = ParseNFA(m_tokenizer.Position(), Top, true);
							NFA::Connect(NewEdge(), out->Head, node->Head);
							iter = node;
							continue;
						case TokenType::Escape://ת��
							switch (GetEscapeType())
							{
							case EscapeType::BuiltInCharSet:
								node = this->BuiltInSet();
								break;
							case EscapeType::KeyWord:
								m_tokenizer.Consume(token);
								node = m_resource->NFAPool.CreateAndAppend(m_resource);
								node->BuildNode(m_resource->ContentPool.CreateAndAppend(*token.Begin));
								break;
							}
							if (m_tokenizer.Position() < Top)//Peek�߽���
							{
								index_t offset = 0;
								index_t current = m_tokenizer.Position();
								NFA*suffix = Suffix(node, current - 1, current, offset);
								if (suffix != nullptr)//��׺����ɹ�
								{
									node = suffix;
									m_tokenizer.Position(current + offset);//���¶�λ
								}
							}
							else if (successive)//����
								goto Exit;
							break;
						case TokenType::LBracket:
							node = ParseSet(); //�ַ���
							break;
						case TokenType::LParen:
						{
							index_t before = m_tokenizer.Position();
							node = ParseNFA(m_tokenizer.Position(), Top);
							index_t after = m_tokenizer.Position();
							if (m_tokenizer.Position() < Top)//Peek�߽���
							{
								index_t offset = 0;
								NFA*suffix = Suffix(node, before, after, offset);
								if (suffix != nullptr)
								{
									node = suffix;
									m_tokenizer.Position(after + offset);//���¶�λ
								}
							}
							else if (successive)
								goto Exit;
							break;
						}
						case TokenType::RParen://��С���ţ�����������˳�SubParse
							if (ParallelBreak)
								m_tokenizer.Move(-1);
							goto Exit;
						}
						if (iter != nullptr)
							NFA::Connect(NewEdge(), iter->Tail, node->Head);//������һ�Σ�����ǰ������
						else
							NFA::Connect(NewEdge(), out->Head, node->Head);//��һ�ν���ͷͷ����
						iter = node;
					}
				Exit:
					NFA::Connect(NewEdge(), iter->Tail, out->Tail);//β������
					return out;
				}
				//����
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
						for (index_t i = Min - 2; i > 0; i--)//�м�ڵ�
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
						NFA::Connect(NewEdge(), node->Head, ret->Tail);//������
						NFA::Connect(NewEdge(), node->Tail, ret->Tail);//��β����
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
						NFA::Connect(NewEdge(), hollow, ret->Tail);//��β����
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
						NFA::Connect(NewEdge(), node->Head, ret->Tail);//������
						NFA*prev = node;
						for (index_t i = Max - Min - 2; i > 0; i--)//�м�ڵ�
						{
							NFA*mid = ParseNFA(Base, Top);
							NFA::Connect(NewEdge(), mid->Head, ret->Tail);//������
							prev->Joint(mid);
							prev = mid;
						}
						tail = ParseNFA(Base, Top);
						prev->Joint(tail);
						NFA::Connect(NewEdge(), tail->Head, ret->Tail);//������
						NFA::Connect(NewEdge(), tail->Tail, ret->Tail);//��β����
					}
					return ret;
				}
				//Parse��׺
				NFA* Suffix(NFA*Parsed,index_t Base, index_t Top, index_t&Offset)
				{
					NFA*ret = nullptr;
					int left = 0;
					int right = 0;
					Token token;
					m_tokenizer.Consume(token);
					Offset++;//�ٶ�Ϊ0
					switch (token.Type)
					{
					case TokenType::LCurly:
					{
						size_t left_count = 0;
						size_t right_count = 0;
						left_count = StringFunction::IntSniff(token.Begin + 1);//��̽��С����
						if (left_count > 0)
							left = StringFunction::StringToInt<int>(token.Begin + 1, left_count);
						else
							return nullptr;
						m_tokenizer.Move(left_count);//��������
						m_tokenizer.Consume(token);
						Offset += left_count + 1;
						if (token.Type == TokenType::RCurly)//�����������,����
						{
							right = left; 
							break;
						}
						else if (token.Type == TokenType::Comma)//���ż��
						{
							m_tokenizer.Consume(token);
							right_count = StringFunction::IntSniff(token.Begin);
							if (right_count > 0)
								right = StringFunction::StringToInt<int>(token.Begin, right_count);
							else//���û�����֣��������޲���
								right = -1;
							m_tokenizer.Move(right_count - 1);
							m_tokenizer.Consume(token);
							Offset += right_count + 1;
							if (token.Type != TokenType::RCurly)//�һ����ż��
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
					ret = Repeat(Parsed, Base, Top, left, right);
					return ret;
				}
				//Parseǰ׺
				NFA* Prefix() {
					//TODO (���ܱߣ����粶�񣬺������
				}
				//�±�
				inline Edge* NewEdge() {
					return m_resource->EdgePool.CreateAndAppend();
				}
				inline Edge* NewEdge(BasicContent*Content) {
					return m_resource->EdgePool.CreateAndAppend(Content);
				}
				//��״̬
				inline Status* NewStatus() {
					return m_resource->StatusPool.CreateAndAppend();
				}
			public:
				Parser(String const&Source,NFAResource*Resource) :m_tokenizer(Source.GetData(), Source.Count()),m_resource(Resource) {}
				Parser(wchar_t const* Source, size_t Size, NFAResource*Resource) :m_tokenizer(Source, Size), m_resource(Resource) {}
				inline NFA* Parse()
				{
					return ParseNFA(0, m_tokenizer.Count());
				}
				~Parser() {}
			};
			//�հ�
			struct Closure
			{
				Status*Target;
				Edge*ValidEdge;
			};
			//NFA��
			class NFASimplify
			{
			public:
				//���������Ч״̬
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
				//���״̬������Depth����1�ıհ�(��Ҫָ�����еİ���Depth����Ϊ-1)
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
				//��ǵ�Ч����״̬
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
				//�Ƴ���Ч����״̬
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
				//��NFA
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
							if (closures[i].ValidEdge->Content != nullptr)
								edge->Content = static_cast<BasicContent*>(closures[i].ValidEdge->Content->ClonePtr());
							if (edge->Content != nullptr)
								Pool->ContentPool.Add(edge->Content);
							if (closures[i].Target == var.Value())
								edge->LoopEdge = true;//��¼Loop��
							NFA::Connect(edge, var.Value(), closures[i].Target);
						}
					}
					RemoveInvalidStatusAndEdges(Pool);
					MarkEquivalentEndStatus(Pool->StatusPool);
					return Target;
				}
			};
			//NFAƥ��
			class NFAMatch
			{
			public:
				static bool Match(const wchar_t*Target, const wchar_t*Top, Status*Where) {
					if (Where->Final)//����Ƿ񵽴�����״̬
						return Target == Top;
					if (Target == Top)
						return Where->Final || Where->EquivalentFinal;
					if (Where->OutEdges.Count() == 1)
					{
						//����֧����ֱ�ӵ���
						Status* iter = Where;
						while (iter->OutEdges.Count() == 1) {
							if (iter->OutEdges[0]->Content != nullptr)
							{
								if (!iter->OutEdges[0]->Content->Accept(*Target))
									return false;
								Target++;
							}
							iter = iter->OutEdges[0]->To;
							if (iter->Final)
								return Target == Top;
						}
						return Match(Target, Top, iter);
					}
					else
					{
						//���֦
						for (index_t i = 0; i < Where->OutEdges.Count(); ++i) {
							if (Where->OutEdges[i]->Content == nullptr)
							{
								if (Match(Target, Top, Where->OutEdges[i]->To))
									return true;
							}
							else if (Where->OutEdges[i]->Content->Accept(*Target))
								if (Match(Target + 1, Top, Where->OutEdges[i]->To))
									return true;
						}
						return false;
					}
				}
			};
			//������ʽ
			class Regex
			{
				String m_pattern;
				NFA* m_enter = nullptr;
				NFAResource * m_resource = nullptr;
			public:
				Regex(String const&Pattern) :m_pattern(Pattern),m_resource(new NFAResource) {
					Parser parser(m_pattern, m_resource);
					m_enter = parser.Parse();
					m_enter->Head->Valid = true;
					m_enter->Tail->Valid = true;
					m_enter->Tail->Final = true;
					NFASimplify::SimplifyNFA(m_resource, m_enter);
				}
				inline bool Match(String const&Target) {
					return NFAMatch::Match(Target.GetData(), Target.GetData() + Target.Count(), m_enter->Head);
				}
				~Regex() {
					if (m_enter != nullptr)m_enter = nullptr;
					if (m_resource != nullptr) {
						delete m_resource;
						m_resource = nullptr;
					}
				}
			};
		}
	};
};