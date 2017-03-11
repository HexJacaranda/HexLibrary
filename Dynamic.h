#pragma once
namespace HL
{
	namespace System
	{
		namespace Dynamic
		{
			enum class order
			{
				mov,
				push,
				pop,
				lea,
				jmp,
				call,

				jge,
				jle,
				jl,
				jg,

				cmp,

				and,
				or ,
				xor,
				not,

				add,
				sub,
				inc,
				dec,
				mul,
				div,

				syscall,

				exit
			};

			enum class reg
			{
				eax, ebx, ecx, edx, ebp, esp
			};

			enum class JITContentType
			{
				Order,
				Register,
				Value,
				Ptr
			};

			template<class PlatformType>
			struct JITContent
			{
			private:
				template<class T>
				friend class AssemblyJIT;
				PlatformType argument;
				JITContentType type;
			public:
				JITContent() {}
				JITContent(reg re) {
					argument = (PlatformType)re;
					type = JITContentType::Register;
				}
				JITContent(order or ) {
					argument = (PlatformType) or ;
					type = JITContentType::Order;
				}
				JITContent(PlatformType value) {
					argument = value;
					type = JITContentType::Value;
				}
				JITContent(void*value) {
					argument = (PlatformType)value;
					type = JITContentType::Ptr;
				}
			};

			//���JIT
			template<class PlatformType>
			class AssemblyJIT
			{
			public:
				typedef JITContent<PlatformType> Content;
			private:
				Content*source = nullptr;

				Content*PC = nullptr;//��ǰִ����

				PlatformType*stack = nullptr;//ջ
				size_t stack_size = 0;//ջ��С

				PlatformType*arguments[5];//ָ�����,(ʵ���õ��Ľ���ǰ����,������������)

				//�Ĵ���
				PlatformType EAX, EBX, ECX, EDX, EDI, ESI;
				PlatformType*ESP = nullptr;
				PlatformType*EBP = nullptr;

				PlatformType CMP;//CMP�������

				//ƥ�����
				int MatchArguments() {
					int i = 0;
					while (i < 5) {
						arguments[i] = nullptr;//�ÿղ���
						PC++;//����ƶ�
						if (PC->type == JITContentType::Register)
						{
							reg re = (reg)PC->argument;
							if (re == reg::eax)
								arguments[i] = &EAX;
							else if (re == reg::ebx)
								arguments[i] = &EBX;
							else if (re == reg::ecx)
								arguments[i] = &ECX;
							else if (re == reg::edx)
								arguments[i] = &EDX;
							else if (re == reg::ebp)
								arguments[i] = (PlatformType*)&EBP;
							else if (re == reg::esp)
								arguments[i] = (PlatformType*)&ESP;
						}
						else if (PC->type == JITContentType::Value)
						{
							arguments[i] = &PC->argument;
						}
						else if (PC->type == JITContentType::Ptr) {
							arguments[i] = (PlatformType*)PC->argument;
						}
						else if (PC->type == JITContentType::Order)
						{
							PC--;
							break;
						}
						i++;
					}
					return i;//����ƥ��Ĳ�������
				}
			public:
				//���ؼĴ���ֵ(ebp,espΪָ��ֵ)
				PlatformType RegisterValue(reg re)const {
					if (re == reg::eax)
						return EAX;
					else if (re == reg::ebx)
						return EBX;
					else if (re == reg::ecx)
						return ECX;
					else if (re == reg::edx)
						return EDX;
					else if (re == reg::ebp)
						return (PlatformType)EBP;
					else if (re == reg::esp)
						return (PlatformType)ESP;
					else
						return 0;
				}
				AssemblyJIT() {
				}
				AssemblyJIT(Content*content) :source(content), PC(content) {
					stack_size = sizeof(PlatformType) * 4 * 1024;
					stack = (PlatformType*)malloc(stack_size);
					ESP = EBP = (PlatformType*)((PlatformType)stack + stack_size);
				}
				AssemblyJIT(Content*content, size_t stacksize) :source(content), PC(content), stack_size(stacksize) {
					stack_size = sizeof(PlatformType) * 4 * 1024;
					stack = (PlatformType*)malloc(stack_size);
					ESP = EBP = (PlatformType*)((PlatformType)stack + stack_size);
				}
				void Run() {
					for (;;) {
						if (PC->type == JITContentType::Order)
						{
						Start:
							order or = (order)PC->argument;//ת��Ϊorder

							MatchArguments();//ƥ�����

							if (or==order::mov)
								*this->arguments[0] = *this->arguments[1];//�ƶ�
							else if (or==order::jmp)
							{
								this->PC = (Content*)this->arguments[0];//��������ת
								goto Start;
							}
							else if (or==order::call)
							{
								*(ESP--) = (PlatformType)this->PC;//���浱ǰλ��
								this->PC = (Content*)this->arguments[0];//��ת
							}
							else if (or==order::lea) {
								*this->arguments[0] = (*this->arguments[1] + *this->arguments[2]);
							}
							else if (or==order::push) {
								*(ESP--) = *this->arguments[0];
							}
							else if (or==order::pop) {
								*this->arguments[0] = *(++ESP);
							}
							else if (or==order::inc)
								++*this->arguments[0];
							else if (or==order::dec)
								--*this->arguments[0];
							else if (or==order::sub)
								*this->arguments[0] -= *this->arguments[1];
							else if (or==order::add)
								*this->arguments[0] += *this->arguments[1];
							else if (or==order::div)
								*this->arguments[0] /= *this->arguments[1];
							else if (or==order::mul)
								*this->arguments[0] *= *this->arguments[1];
							else if (or==order::cmp)
							{
								if (*this->arguments[0] > *this->arguments[1])
									CMP = 1;
								else if (*this->arguments[0] == *this->arguments[0])
									CMP = 0;
								else
									CMP = -1;
							}
							else if (or==order::and)
								CMP = (*arguments[0]) & (*arguments[1]);
							else if (or==order:: or )
								CMP = (*arguments[0]) | (*arguments[1]);
							else if (or==order::xor)
							{
								if (*this->arguments[0] >= 0 || *this->arguments[1] >= 0)
									CMP = 0;
								else
									CMP = 1;
							}
							else if (or==order::jg)
							{
								if (CMP > 0)
									this->PC = (Content*)this->arguments[0];
							}
							else if (or==order::jge)
							{
								if (CMP >= 0)
									this->PC = (Content*)this->arguments[0];
							}
							else if (or==order::jl)
							{
								if (CMP < 0)
									this->PC = (Content*)this->arguments[0];
							}
							else if (or==order::jle)
							{
								if (CMP <= 0)
									this->PC = (Content*)this->arguments[0];
							}
							else if (or==order::exit)
								break;
						}
						++PC;
					}
				}
				~AssemblyJIT() {
					if (stack) {
						free(stack);
						stack = nullptr;
					}
				}
			};

			typedef AssemblyJIT<int> AssemblyJIT32;
			typedef AssemblyJIT<long long> AssemblyJIT64;

			typedef JITContent<int> JITContent32;
			typedef JITContent<long long> JITContent64;



			namespace Inner
			{
				class VariableImplement;
				class FunctionImplement;
				class RegisterImplement;
				class TypeImplement;

				template<class U>
				using uptr = UPointer::uptr<U>;
				template<class U>
				using UArray = Generic::UArray<U>;
				template<class U>
				using SymbolTable = UArray<uptr<U>>;
#define _SF	   StringFunction
#define _SI    Internal

				//��Ƿ������
				enum class AnalysisResult
				{
					Function, 
					GenericFunction,
					Variable,
					Expression,
					Register,
					Class,
					GenericClass,
					Null
				};
				//��������
				enum class TypeAttribute
				{
					Ref,
					Out,
					Null
				};
				//���͵�����
				enum class TypeOfType
				{
					Normal,
					Generic
				};
				//��������
				enum class BlockType
				{
					Brace,//������
					Bracket,//������
					Curves,//С����
					Null
				};
				//��̽����
				enum class SnifferType :unsigned int
				{
					Precise = 0,
					Fuzzy = 1
				};

				class IIdentifier
				{
				public:
					UString IdentifierName;//��ʶ������
					size_t HashForName;//��ʶ��Hash
					
				};
				//�ؼ���
				class KeyWord :public IIdentifier
				{
				public:
					AnalysisResult SelfDedution = AnalysisResult::Null;//�����ƶ�����
				};
				//����
				class TypeIdentifier:public IIdentifier
				{
				public:
					TypeOfType Type = TypeOfType::Normal;//���͵�����
					uptr<TypeImplement> Impl;//����ʵ��
				};
				//����ʵ��
				class VariableImplement
				{
				public:
					uptr<TypeIdentifier> Type;//����
					TypeAttribute TypeAttr = TypeAttribute::Null;//�������η�
					int Offset;//�ڴ�ƫ��ֵ
				};

				//����ʵ��
				class FunctionImplement
				{
				public:
					UArray<JITContent32> JITCode;
					UArray<VariableImplement> ArgTypes;
				};

				class RegisterImplement
				{
				public:
					uptr<FunctionImplement> Getter;
					uptr<FunctionImplement> Setter;
					uptr<VariableImplement> Var;//������ܵĻ�
				};
				//�����ڴ��Ų�
				class MemoryLayout
				{
				public:
					size_t TotalSize;//�ܴ�С
					UArray<uptr<VariableImplement>>VariableInfo;//����������Ϣ
					UArray<uptr<RegisterImplement>>RegisterInfo;//�Ĵ�����Ϣ(���Ǻ��ڱ����Ų�)
				};

				//����ʵ��
				class TypeImplement
				{
				public:
					UArray<uptr<FunctionImplement>> Functions;
					uptr<MemoryLayout> MemLayout;
				};

				//�����̽
				class TokenSniffer
				{
				public:
					wchar_t const*StartPosition;//��ʼλ��
					wchar_t const*EndPosition;//��ֹλ��
					wchar_t const*FinalPosition;//����λ��
					unsigned TokenHash;//��ʶ��hash��
					//ȡ��һ�����
					void NextToken() {
						wchar_t const*src = this->StartPosition;//��ʼ��ַ
																 //Ԥ����ͷ
						while (*src != 0) {
							if (*src == L' ' || *src == L'\n')//�����ո���߻��з�
								++src;
							else
							{
								this->StartPosition = src;
								break;
							}
						}
						//�����һλ(����������)
						if (_SF::IsInt(*src))
						{
							this->TokenHash = -1;
							return;
						}
						//����Token
						while (*src != 0) {
							if (_SF::IsChar(*src) || _SF::IsInt(*src) || *src == L'_')//���ַ� 
								++src;
							else
							{
								this->EndPosition = src;//������Ϣ
								this->TokenHash = Memory::Hash::HashSeq(this->StartPosition, src - this->StartPosition);
								this->FinalPosition = _SF::GoNotUntil(this->EndPosition, L' ');
								return;
							}
						}
					}
					inline void Update() {
						this->StartPosition = FinalPosition;
					}
				};

				//����������̽
				template<unsigned Type>
				class BasicBlockSniffer;

				//�������龫׼��̽
				template<>
				class BasicBlockSniffer<0>
				{
				public:
					size_t Curves = 0;//С���ż���
					size_t Brace = 0;//�����ż���
					size_t Bracket = 0;//�����ż���
					UArray<BlockType> BlockLayer;
					BlockType CurrentType;//��ǰ����
					wchar_t const*Current;//��ǰ�ַ���
					bool Next()
					{
						if (*Current == L'(')
						{
							Curves++;
							BlockLayer->Add(BlockType::Curves);
							CurrentType = BlockType::Curves;
						}
						else if (*Current == L')')
						{
							Curves--;
							if (CurrentType != BlockType::Curves)
								return false;
							BlockLayer->Pop();
						}
						else if (*Current == L'[')
						{
							Bracket++;
							BlockLayer->Add(BlockType::Bracket);
							CurrentType = BlockType::Bracket;
						}
						else if (*Current == L']')
						{
							Bracket--;
							if (CurrentType != BlockType::Bracket)
								return false;
							BlockLayer->Pop();
						}
						else if (*Current == L'{')
						{
							Brace++;
							BlockLayer->Add(BlockType::Brace);
							CurrentType = BlockType::Brace;
						}
						else if (*Current == L'}')
						{
							Brace--;
							if (CurrentType != BlockType::Brace)
								return false;
							BlockLayer->Pop();
						}
						if (Curves == 0 && Brace == 0 && Bracket == 0)
							return false;
						++Current;
						return true;
					}
				};
				//��������ģ����̽
				template<>
				class BasicBlockSniffer<1>
				{
				public:
					size_t FuzzyCount;//ģ������
					wchar_t const*Current;//��ǰָ��
					bool FuzzyNext() {
						if (*Current != 0)
						{
							if (*Current == L'(' || *Current == L'[' || *Current == L'{')
								FuzzyCount++;
							else if (*Current == L')' || *Current == L']' || *Current == L'}')
								FuzzyCount--;
							if (FuzzyCount == 0)
								return false;
							++Current;
							return true;
						}
						else
							return false;
					}
				};

				typedef BasicBlockSniffer<0> BlockSniffer;

				typedef	BasicBlockSniffer<1> BlockFuzzySniffer;


				//��Ƿ���������
				class TokenAnalysisDomain
				{
				public:
					UArray<uptr<KeyWord>> KeyWords;//����Ĺؼ���
					UArray<uptr<TypeIdentifier>> Types;//����
				};

				class TokenAnalysisResult
				{
				public:
					AnalysisResult Result;
					UArray<uptr<IIdentifier>> Prefix;
					uptr<TypeIdentifier> Type;
					UString Name;
				};

				static bool TypeValidate(AnalysisResult const&r, AnalysisResult const&l)
				{
					if (r == AnalysisResult::Null)
						return false;
					else
					{
						if (r == l)
							return false;
					}
					return true;
				}

				//�����������
				static uptr<TokenAnalysisResult> SingleDefineAnalysis(TokenAnalysisDomain*domain_ptr, TokenSniffer*sniffer_ptr) {
					uptr<TokenAnalysisResult> reter = GC::newgc<TokenAnalysisResult>();
					UArray<uptr<IIdentifier>> prefix = GC::newgc<Generic::Array<uptr<IIdentifier>>>();
					UString name = GC::newgc<String>((size_t)0);
					AnalysisResult result = AnalysisResult::Null;
					for(;;)
					{
						//�����ؼ���
						sniffer_ptr->NextToken();
						index_t index = domain_ptr->KeyWords->IndexOfWithComparator(nullptr, [&](uptr<KeyWord> const&l, uptr<KeyWord> const&r) {
							return l->HashForName == sniffer_ptr->TokenHash;
						});
						if (index != -1)
						{
							result = domain_ptr->KeyWords[index]->SelfDedution;//��ֵ�ƶ�
							prefix->Add(domain_ptr->KeyWords[index]);
						}
						else//����
						{
							index = domain_ptr->Types->IndexOfWithComparator(nullptr, [&](uptr<TypeIdentifier> const&l, uptr<TypeIdentifier> const&r) {
								return l->HashForName == sniffer_ptr->TokenHash;
							});
							if (index != -1)
								reter->Type = domain_ptr->Types[index];
							else//����
							{
								name->Append(sniffer_ptr->StartPosition, 0, sniffer_ptr->EndPosition - sniffer_ptr->StartPosition);
								//��ʼ��֤�϶�
								if (*sniffer_ptr->FinalPosition == L'(')//һ���Ǻ���
								{
									if (!TypeValidate(result, AnalysisResult::Function))
									{
										result = AnalysisResult::Function;
										goto Out;
									}
								}
								else if (*sniffer_ptr->FinalPosition == L'{')//���������������߼Ĵ���
								{
									//����һ���ж�
									BlockFuzzySniffer fuz_sniffer;
									fuz_sniffer.Current = sniffer_ptr->FinalPosition;
									while (fuz_sniffer.FuzzyNext())
									{
										if (*fuz_sniffer.Current == L'g'|| *fuz_sniffer.Current == L's')
										{
											bool ret = false;
											if (*fuz_sniffer.Current == L'g')
												ret = Internal::BasicStringEquals(fuz_sniffer.Current, L"get", 3, 3);
											else
												ret = Internal::BasicStringEquals(fuz_sniffer.Current, L"set", 3, 3);
											if (ret)//����ҵ���
											{
												fuz_sniffer.Current += 3;//����get��set
												fuz_sniffer.Current = _SF::GoNotUntil(fuz_sniffer.Current, L' ');//�����ո�
												if (*fuz_sniffer.Current == L';' || *fuz_sniffer.Current == L'{')//�϶�ΪRegister
												{
													if (!TypeValidate(result, AnalysisResult::Register))
													{
														result = AnalysisResult::Register;
														goto Out;
													}
													else
														Exception::Throw<Exception::ParserException>(L"�ƶ�ʧ��");
												}
												else
												{
													if (!TypeValidate(result, AnalysisResult::Class))
													{
														result = AnalysisResult::Class;
														goto Out;
													}
												}
											}
										}
									}

								}
								else if (*sniffer_ptr->FinalPosition == L';')
								{
									if (!TypeValidate(result, AnalysisResult::Variable))
									{
										result = AnalysisResult::Variable;
										goto Out;
									}
								}
								if (result == AnalysisResult::Null)//�ƶ�ʧ��
								{
									Exception::Throw<Exception::ParserException>(L"�ƶ�ʧ��");
								}
							}
						}
						sniffer_ptr->Update();
					}

					Out:
					reter->Name = name;
					reter->Prefix = prefix;
					reter->Result = result;
					return reter;
				}

				//������������
				static uptr<FunctionImplement> FunctionCompile(uptr<TokenAnalysisResult> const&ptr) {

				}
				//�Ĵ�����������
				static uptr<RegisterImplement> RegisterCompile(uptr<TokenAnalysisResult> const&ptr) {

				}

				//���������
				static uptr<TypeIdentifier> ClassDefineCompile(TokenAnalysisDomain*domain_ptr, TokenSniffer*sniffer_ptr) {
					uptr<TypeIdentifier> ret = GC::newgc<TypeIdentifier>();
					ret->Impl = GC::newgc<TypeImplement>();
					ret->Impl->MemLayout = GC::newgc<MemoryLayout>();
					ret->Impl->Functions = GC::newgc<Generic::Array<uptr<FunctionImplement>>>();
					ret->Impl->MemLayout->VariableInfo = GC::newgc<Generic::Array<uptr<VariableImplement>>>();

					uptr<TokenAnalysisResult>result = SingleDefineAnalysis(domain_ptr, sniffer_ptr);
					if (result->Result == AnalysisResult::Class)
					{
						ret->IdentifierName = result->Name;
						ret->HashForName = ret->IdentifierName->GetHashCode();
						ret->Type = TypeOfType::Normal;
					}
					else if (result->Result==AnalysisResult::GenericClass) 
					{
						ret->IdentifierName = result->Name;
						ret->HashForName = ret->IdentifierName->GetHashCode();
						ret->Type = TypeOfType::Generic;
					}
					else
					{
						Exception::Throw<Exception::ParserException>(L"����ʧ��");
					}
					for (;;) 
					{
						result = SingleDefineAnalysis(domain_ptr, sniffer_ptr);
						if (result->Result == AnalysisResult::Variable)
						{
							uptr<VariableImplement> impl = GC::newgc<VariableImplement>();
							impl->Type = result->Type;
							ret->Impl->MemLayout->VariableInfo->Add(impl);
						}
						else if (result->Result == AnalysisResult::Register)
						{
							ret->Impl->MemLayout->RegisterInfo->Add(RegisterCompile(result));
						}
						else if (result->Result == AnalysisResult::Function)
						{
							ret->Impl->Functions->Add(FunctionCompile(result));
						}
						else
						{
							Exception::Throw<Exception::ParserException>(L"��֧�ֵĶ���");
						}
					}

				}


				
				static uptr<MemoryLayout> LayoutCompile() {
					
				}
			}
		}
	}
}