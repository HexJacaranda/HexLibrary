#pragma once
namespace HL
{
	namespace Exception
	{
		class IException
		{
			wchar_t * ExceptionMessage = nullptr;
		public:
			IException()throw(){}
			IException(wchar_t const*msg)throw()
			{
				ExceptionMessage = const_cast<wchar_t*>(msg);
			}
		 	virtual wchar_t const* Message()throw(){
				return ExceptionMessage;
			}
			virtual ~IException()throw() {
			}
		};
		template<class T>
		class ExceptionHandle;

#define EXCEPTIONCLASS(Name,Msg) class Name:public HL::Exception::IException{public: Name():IException(L#Msg){} }

		EXCEPTIONCLASS(InvalidInvokeException, "调用失败");
		EXCEPTIONCLASS(NullReferenceException, "无效引用");
		EXCEPTIONCLASS(SetOnlyException, "该属性为只写");
		EXCEPTIONCLASS(GetOnlyException, "该属性为只读");
		EXCEPTIONCLASS(ArgumentNullException, "参数为null");
		EXCEPTIONCLASS(UnHandledException, "未处理异常");
		EXCEPTIONCLASS(KeyNotFoundException, "找不到键");
		EXCEPTIONCLASS(InterfaceNoImplementException, "接口未实现异常");

		class IndexOutOfException;

		class BadAllocateException;

		class ParserException;


		template<class T,class...Args>
		void Throw(Args&&...args);
	}
}