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
			//����
			class ContentBase
			{
			protected:
				bool m_not;
				wchar_t m_first;
			public:
				ContentBase(wchar_t First, bool Not = false) :m_first(First), m_not(Not) {}
				//�Ƿ����
				virtual bool Accept(wchar_t Target) {
					if (m_first == L'.')
						return !m_not;
					return Target == m_first && !m_not;
				}
				virtual ~ContentBase() {}
			};
			//�ַ���
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
			//�߶���
			class Action
			{
			public:
				virtual void Act(Edge*) = 0;
				virtual ~Action() {}
			};
			//��
			class Edge
			{
			public:
				Edge() :From(nullptr), To(nullptr), Content(nullptr) {}
				Status*From;//��ʼ״̬
				Status*To;//Ŀ��״̬
				ContentBase*Content;//ƥ������
				Generic::Array<Action*> Actions;//����
			};
			//״̬
			class Status
			{
			public:
				Generic::Array<Edge*> InEdges;
				Generic::Array<Edge*> OutEdges;
				bool Final;
			};
			//��Դ
			class Resource
			{
			public:
				Pool<NFA> NFAPool;
				Pool<ContentBase> ContentPool;
				Pool<Edge> EdgePool;
				Pool<Status> StatusPool;
			};
			//NFA״̬��
			class NFA
			{
				Resource*Resources;
			public:
				NFA(Resource*Origin) :Resources(Origin), Head(nullptr), Tail(nullptr) {}
				NFA() :Head(nullptr), Tail(nullptr) {}
				Status*Head;
				Status*Tail;
				//����ڵ�
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
				//��This,R��������
				void Joint(NFA*Right) {
					Connect(Resources->EdgePool.CreateAndAppend(), this->Tail, Right->Head);
				}
				//��Target��������
				void Parallel(NFA*Target) {
					Connect(Resources->EdgePool.CreateAndAppend(), this->Head, Target->Head);
					Connect(Resources->EdgePool.CreateAndAppend(), Target->Tail, this->Tail);
				}
				//�����ظ�ƥ��
				void Repeat(NFA*Target, int Min, int Max) {

				}
				//�ñ߽�����״̬��������
				static void Connect(Edge*ConnectEdge, Status*From, Status*To) {
					ConnectEdge->From = From;
					ConnectEdge->To = To;
					From->OutEdges.Add(ConnectEdge);
					To->InEdges.Add(ConnectEdge);
				}
			};
			//�﷨������
			class Parser
			{
				Tokenizer m_tokenizer;
				Resource*m_resource = new Resource();
				//Parse�ַ�����
				NFA* ParseSet() {
					NFA* out = m_resource->NFAPool.CreateAndAppend(m_resource);
					out->BuildNode(nullptr);
					Token token;
					m_tokenizer.Peek(token);//Peekһ��
					bool not= false;//ȡ��
					int type = 0;//0Ϊƴ�ӣ�1�Ƿ�Χ��2���ڽ�����
					if (token.Type == TokenType::Pow)//ȡ��
					{
						not= true;
						m_tokenizer.Consume(token);//ǰ��
					}
					//�ж�Set���ͣ����ַ�ƴ�ӣ����Ƿ�Χ(�����ڽ�����)��
					m_tokenizer.Peek(token);
					if (token.Type == TokenType::Escape)//�ڽ�����
						type = 2;
					else
					{
						if (*(token.Begin + 1) == L'-')//��Χ�ڶ����ַ���Ϊ-
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
							case TokenType::Escape://����ת���
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
					return nullptr;//Ԥ���ڽ���ǰ����
				}
				//[Base,Top)�������Parse
				NFA* ParseNFA(index_t Base, index_t Top)
				{
					m_tokenizer.Position(Base);//��λ

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
							node->BuildNode(m_resource->ContentPool.CreateAndAppend(*token.Begin));
							if (m_tokenizer.Position() + 1 < Top)//Peek�߽���
							{
								index_t offset = 0;
								index_t current = m_tokenizer.Position();
								NFA*suffix = Suffix(current - 1, current, offset);
								if (suffix != nullptr)//��׺����ɹ�
								{
									node = suffix;
									m_tokenizer.Position(current + offset);//���¶�λ
								}
							}
							break;
						}
						case TokenType::Or://����
							node = ParseNFA(m_tokenizer.Position(), Top);
							out->Parallel(node);//����
							break;
						case TokenType::LBracket:
							node = ParseSet(); //�ַ���
							break;
						case TokenType::LParen:
						{
							index_t before = m_tokenizer.Position();
							node = ParseNFA(m_tokenizer.Position(), Top);
							index_t after = m_tokenizer.Position();
							if (m_tokenizer.Position() + 1 < Top)//Peek�߽���
							{
								index_t offset = 0;
								NFA*suffix = Suffix(before, after, offset);
								if (suffix != nullptr)
								{
									node = suffix;
									m_tokenizer.Position(after + offset);//���¶�λ
								}
								break;
							}
						}
						case TokenType::RParen://��С���ţ�����������˳�SubParse
							break;
						}
						if (iter != nullptr)
							NFA::Connect(m_resource->EdgePool.CreateAndAppend(), iter->Tail, node->Head);//������һ�Σ�����ǰ������
						else
							NFA::Connect(m_resource->EdgePool.CreateAndAppend(), out->Head, node->Head);//��һ�ν���ͷͷ����
						iter = node;
					}
					NFA::Connect(m_resource->EdgePool.CreateAndAppend(), iter->Tail, out->Tail);//β������
					return out;
				}
				//����
				NFA* Repeat(index_t Base,index_t Top, int Min, int Max)
				{
					NFA*ret = m_resource->NFAPool.CreateAndAppend(m_resource);
					ret->BuildNode(nullptr);
					//����ͷ
					NFA*head = ParseNFA(Base, Top);
					NFA*tail = nullptr;
					NFA::Connect(m_resource->EdgePool.CreateAndAppend(), ret->Head, head->Head);
					if (Min == 1)
						NFA::Connect(m_resource->EdgePool.CreateAndAppend(), head->Tail, ret->Tail);
					else if (Min > 1)
					{
						NFA*prev = head;
						while (Min - 2 >= 0)//�м�ڵ�
						{
							NFA*mid = ParseNFA(Base, Top);
							prev->Joint(mid);
							prev = mid;
						}
						tail = ParseNFA(Base, Top);
						prev->Joint(tail);
						NFA::Connect(m_resource->EdgePool.CreateAndAppend(), tail->Tail, ret->Tail);
					}
					else//���
						tail = head;
					//����ѭ��
					if (Max == -1)
					{
						NFA::Connect(m_resource->EdgePool.CreateAndAppend(), tail->Tail, tail->Head);//�Ի�
						NFA::Connect(m_resource->EdgePool.CreateAndAppend(), tail->Tail, ret->Tail);//ָ���β
					}
					else
					{
						NFA*prev = tail;
						while (Max - Min - 1 >= 0)//�м�ڵ�
						{
							NFA*mid = ParseNFA(Base, Top);
							NFA::Connect(m_resource->EdgePool.CreateAndAppend(), mid->Tail, ret->Tail);//��ӱ���������
							prev->Joint(mid);
							prev = mid;
						}
						tail = ParseNFA(Base, Top);
						prev->Joint(tail);
						NFA::Connect(m_resource->EdgePool.CreateAndAppend(), tail->Tail, ret->Tail);//ָ���β
					}
					return ret;
				}
				//Parse��׺
				NFA* Suffix(index_t Base, index_t Top, index_t&Offset)
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
							right_count = StringFunction::IntSniff(token.Begin + 1);
							if (right_count > 0)
								right = StringFunction::StringToInt<int>(token.Begin + 1, right_count);
							else//���û�����֣��������޲���
								right = -1;
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
					ret = Repeat(Base, Top, left, right);
					return ret;
				}
				//Parseǰ׺
				NFA* Prefix() {
					//TODO (���ܱߣ����粶�񣬺������
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