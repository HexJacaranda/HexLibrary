#pragma once
namespace HL
{
	namespace System
	{
		namespace RegularExpression
		{
			class Regex;
			class MatchResult;
			
			class Edge;
			class Status;
			class ContentBase;

			//����Regex�����ڴ������
			template<class T>
			class Pool
			{
				Generic::Dictionary<intptr_t, T*> m_objects;
			public:
				Pool() {}
				void Add(T*&Target) {
					m_objects.Add((intptr_t)Target, Target);
				}
				void Remove(T*Target) {
					m_objects.Add()
				}
				inline T* Create() {
					return new T;
				}
				inline T* CreateAndAppend() {
					T*ret = new T;
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
				ContentBase(wchar_t First) :m_first(First) {}
				virtual bool Compare(wchar_t target) {
					if (m_first == L'.')
						return !m_not;
					return m_not ? target != m_first : target == m_first;
				}
				virtual ~ContentBase() {}
			};
			//�ַ���
			class CharSet :public ContentBase
			{
			public:
				typedef Generic::Dictionary<wchar_t, wchar_t> SetType;
			private:
				SetType m_set;
			public:
				CharSet(SetType const&Set) :m_set(Set), ContentBase(0) {}
				CharSet(SetType &&Set) :m_set(Forward(Set)), ContentBase(0) {}
				virtual bool Compare(wchar_t target) {
					return m_not ? !m_set.Contains(target) : m_set.Contains(target);
				}
				virtual ~CharSet() {}
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
			//NFA״̬��
			class NFA
			{
				Pool<Status>*m_status;
				Pool<Edge>*m_edges;
				Pool<NFA>*m_nfas;
			public:
				NFA():Head(nullptr), Tail(nullptr) {}
				Status*Head;
				Status*Tail;
				inline Status* StatusInstance() {
					return m_status->CreateAndAppend();
				}
				//����ڵ�
				void BuildNode(ContentBase*Content)
				{
					Head = m_status->CreateAndAppend();
					Tail = m_status->CreateAndAppend();
					Edge* edge = m_edges->CreateAndAppend();
					edge->Content = Content;
					Connect(edge, this->Head, this->Tail);
				}
				//��This,R��������
				void Joint(NFA*Right) {
					Connect(m_edges->CreateAndAppend(), this->Tail, Right->Head);
				}
				//��Target��������
				void Parallel(NFA*Target) {
					Connect(m_edges->CreateAndAppend(), this->Head, Target->Head);
					Connect(m_edges->CreateAndAppend(), Target->Tail, this->Tail);
				}
				//�����ظ�ƥ��
				void Repeat(NFA*Target, int Min, int Max) {
					//����ͷ
					NFA*head = Target->Clone();
					NFA*tail = nullptr;
					Connect(m_edges->CreateAndAppend(), this->Head, head->Head);
					if (Min == 1)
						Connect(m_edges->CreateAndAppend(), head->Tail, this->Tail);
					else
					{
						NFA*prev = head;
						while (Min - 2 >= 0)//�м�ڵ�
						{
							NFA*mid = Target->Clone();
							prev->Joint(mid);
							prev = mid;
						}
						tail = Target->Clone();
						prev->Joint(tail);
						Connect(m_edges->CreateAndAppend(), tail->Tail, this->Tail);
					}
					//����ѭ��
					if (Max == -1)
						Connect(m_edges->CreateAndAppend(), tail->Tail, tail->Head);
					else
					{
						NFA*prev = tail;
						while (Max - Min - 1 >= 0)//�м�ڵ�
						{
							NFA*mid = Target->Clone();
							Connect(m_edges->CreateAndAppend(), mid->Tail, this->Tail);//��ӱ���������
							prev->Joint(mid);
							prev = mid;
						}
						tail = Target->Clone();
						prev->Joint(tail);
						Connect(m_edges->CreateAndAppend(), tail->Tail, this->Tail);
					}
				}
				//����������NFA������,������ͳ�ȥ�ı�ȫ��ɾ��
				NFA*Clone() {
					NFA* ret = m_nfas->CreateAndAppend();
					ret->Head = m_status->CreateAndAppend();
					ret->Tail = m_status->CreateAndAppend();
					ret->Head->Final = this->Head->Final;
					ret->Tail->Final = this->Tail->Final;
					ret->Head->OutEdges = this->Head->OutEdges;
					ret->Tail->InEdges = this->Tail->InEdges;
				}
				//�ñ߽�����״̬��������
				static void Connect(Edge*ConnectEdge, Status*From, Status*To) {
					ConnectEdge->From = From;
					ConnectEdge->To = To;
					From->OutEdges.Add(ConnectEdge);
					To->InEdges.Add(ConnectEdge);
				}
			};
			//������ʽ
			class Regex
			{
			public:
				
			};
			//��ǵ�����
			enum class TokenType
			{
				//�ַ�(������
				Char,
				//{
				LCurly,
				RCurly,//}
				LParen,//(
				RParen,//)
				LBracket,//[
				RBracket,//]
				Plus,//+
				Minus,//-
				Mul,//*
				Comma,//,
				Pow,//^
				QMark,//��
				Colon,//:
				Or,//|
				Escape// '\'
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
				inline bool Done() {
					return m_index >= m_size;
				}
			};
			//�﷨������
			class Parser
			{
				Tokenizer m_tokenizer;
				void ParseStatus() 
				{
					
					Token token;
					while (m_tokenizer.Done())
					{
						m_tokenizer.Consume(token);
						switch (token.Type)
						{
						case TokenType::Char:

						default:
							break;
						}
					}
				}
			public:
				Parser(String const&Source) :m_tokenizer(Source.GetData(), Source.Count()) {}
				Parser(wchar_t const* Source, size_t Size) :m_tokenizer(Source, Size) {}
				Regex Parse()
				{

				}
			};
		}
	};
};