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

			//汇编JIT
			template<class PlatformType>
			class AssemblyJIT
			{
			public:
				typedef JITContent<PlatformType> Content;
			private:
				Content*source = nullptr;

				Content*PC = nullptr;//当前执行行

				PlatformType*stack = nullptr;//栈
				size_t stack_size = 0;//栈大小

				PlatformType*arguments[5];//指令参数,(实际用到的仅有前三个,另两个做保留)

				//寄存器
				PlatformType EAX, EBX, ECX, EDX, EDI, ESI;
				PlatformType*ESP = nullptr;
				PlatformType*EBP = nullptr;

				PlatformType CMP;//CMP结果保存

				//匹配参数
				int MatchArguments() {
					int i = 0;
					while (i < 5) {
						arguments[i] = nullptr;//置空参数
						PC++;//向后移动
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
					return i;//返回匹配的参数个数
				}
			public:
				//返回寄存器值(ebp,esp为指针值)
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
							order or = (order)PC->argument;//转换为order

							MatchArguments();//匹配参数

							if (or==order::mov)
								*this->arguments[0] = *this->arguments[1];//移动
							else if (or==order::jmp)
							{
								this->PC = (Content*)this->arguments[0];//无条件跳转
								goto Start;
							}
							else if (or==order::call)
							{
								*(ESP--) = (PlatformType)this->PC;//保存当前位置
								this->PC = (Content*)this->arguments[0];//跳转
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

				//标记分析结果
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
				//类型特性
				enum class TypeAttribute
				{
					Ref,
					Out,
					Null
				};
				//类型的类型
				enum class TypeOfType
				{
					Normal,
					Generic
				};
				//区块类型
				enum class BlockType
				{
					Brace,//大括号
					Bracket,//中括号
					Curves,//小括号
					Null
				};
				//嗅探类型
				enum class SnifferType :unsigned int
				{
					Precise = 0,
					Fuzzy = 1
				};

				class IIdentifier
				{
				public:
					UString IdentifierName;//标识符名称
					size_t HashForName;//标识符Hash
					
				};
				//关键字
				class KeyWord :public IIdentifier
				{
				public:
					AnalysisResult SelfDedution = AnalysisResult::Null;//自我推断类型
				};
				//类型
				class TypeIdentifier:public IIdentifier
				{
				public:
					TypeOfType Type = TypeOfType::Normal;//类型的类型
					uptr<TypeImplement> Impl;//类型实现
				};
				//变量实现
				class VariableImplement
				{
				public:
					uptr<TypeIdentifier> Type;//类型
					TypeAttribute TypeAttr = TypeAttribute::Null;//类型修饰符
					int Offset;//内存偏移值
				};

				//函数实现
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
					uptr<VariableImplement> Var;//如果可能的话
				};
				//类型内存排布
				class MemoryLayout
				{
				public:
					size_t TotalSize;//总大小
					UArray<uptr<VariableImplement>>VariableInfo;//变量具体信息
					UArray<uptr<RegisterImplement>>RegisterInfo;//寄存器信息(总是后于变量排布)
				};

				//类型实现
				class TypeImplement
				{
				public:
					UArray<uptr<FunctionImplement>> Functions;
					uptr<MemoryLayout> MemLayout;
				};

				//标记嗅探
				class TokenSniffer
				{
				public:
					wchar_t const*StartPosition;//起始位置
					wchar_t const*EndPosition;//终止位置
					wchar_t const*FinalPosition;//最终位置
					unsigned TokenHash;//标识符hash码
					//取下一个标记
					void NextToken() {
						wchar_t const*src = this->StartPosition;//开始地址
																 //预处理开头
						while (*src != 0) {
							if (*src == L' ' || *src == L'\n')//跳过空格或者换行符
								++src;
							else
							{
								this->StartPosition = src;
								break;
							}
						}
						//处理第一位(不能是数字)
						if (_SF::IsInt(*src))
						{
							this->TokenHash = -1;
							return;
						}
						//处理Token
						while (*src != 0) {
							if (_SF::IsChar(*src) || _SF::IsInt(*src) || *src == L'_')//是字符 
								++src;
							else
							{
								this->EndPosition = src;//更新信息
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

				//代码区块嗅探
				template<unsigned Type>
				class BasicBlockSniffer;

				//代码区块精准嗅探
				template<>
				class BasicBlockSniffer<0>
				{
				public:
					size_t Curves = 0;//小括号计数
					size_t Brace = 0;//大括号计数
					size_t Bracket = 0;//中括号计数
					UArray<BlockType> BlockLayer;
					BlockType CurrentType;//当前类型
					wchar_t const*Current;//当前字符串
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
				//代码区块模糊嗅探
				template<>
				class BasicBlockSniffer<1>
				{
				public:
					size_t FuzzyCount;//模糊计数
					wchar_t const*Current;//当前指向
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


				//标记分析作用域
				class TokenAnalysisDomain
				{
				public:
					UArray<uptr<KeyWord>> KeyWords;//允许的关键字
					UArray<uptr<TypeIdentifier>> Types;//类型
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

				//单个定义分析
				static uptr<TokenAnalysisResult> SingleDefineAnalysis(TokenAnalysisDomain*domain_ptr, TokenSniffer*sniffer_ptr) {
					uptr<TokenAnalysisResult> reter = GC::newgc<TokenAnalysisResult>();
					UArray<uptr<IIdentifier>> prefix = GC::newgc<Generic::Array<uptr<IIdentifier>>>();
					UString name = GC::newgc<String>((size_t)0);
					AnalysisResult result = AnalysisResult::Null;
					for(;;)
					{
						//检索关键字
						sniffer_ptr->NextToken();
						index_t index = domain_ptr->KeyWords->IndexOfWithComparator(nullptr, [&](uptr<KeyWord> const&l, uptr<KeyWord> const&r) {
							return l->HashForName == sniffer_ptr->TokenHash;
						});
						if (index != -1)
						{
							result = domain_ptr->KeyWords[index]->SelfDedution;//赋值推断
							prefix->Add(domain_ptr->KeyWords[index]);
						}
						else//类型
						{
							index = domain_ptr->Types->IndexOfWithComparator(nullptr, [&](uptr<TypeIdentifier> const&l, uptr<TypeIdentifier> const&r) {
								return l->HashForName == sniffer_ptr->TokenHash;
							});
							if (index != -1)
								reter->Type = domain_ptr->Types[index];
							else//名称
							{
								name->Append(sniffer_ptr->StartPosition, 0, sniffer_ptr->EndPosition - sniffer_ptr->StartPosition);
								//开始验证断定
								if (*sniffer_ptr->FinalPosition == L'(')//一定是函数
								{
									if (!TypeValidate(result, AnalysisResult::Function))
									{
										result = AnalysisResult::Function;
										goto Out;
									}
								}
								else if (*sniffer_ptr->FinalPosition == L'{')//可能是类声明或者寄存器
								{
									//做进一步判断
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
											if (ret)//如果找到了
											{
												fuz_sniffer.Current += 3;//跳过get或set
												fuz_sniffer.Current = _SF::GoNotUntil(fuz_sniffer.Current, L' ');//跳过空格
												if (*fuz_sniffer.Current == L';' || *fuz_sniffer.Current == L'{')//断定为Register
												{
													if (!TypeValidate(result, AnalysisResult::Register))
													{
														result = AnalysisResult::Register;
														goto Out;
													}
													else
														Exception::Throw<Exception::ParserException>(L"推断失败");
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
								if (result == AnalysisResult::Null)//推断失败
								{
									Exception::Throw<Exception::ParserException>(L"推断失败");
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

				//函数分析编译
				static uptr<FunctionImplement> FunctionCompile(uptr<TokenAnalysisResult> const&ptr) {

				}
				//寄存器分析编译
				static uptr<RegisterImplement> RegisterCompile(uptr<TokenAnalysisResult> const&ptr) {

				}

				//类分析编译
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
						Exception::Throw<Exception::ParserException>(L"解析失败");
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
							Exception::Throw<Exception::ParserException>(L"不支持的定义");
						}
					}

				}


				
				static uptr<MemoryLayout> LayoutCompile() {
					
				}
			}
		}
	}
}