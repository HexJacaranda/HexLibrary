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

		EXCEPTIONCLASS(InvalidInvokeException, "����ʧ��");
		EXCEPTIONCLASS(NullReferenceException, "��Ч����");
		EXCEPTIONCLASS(SetOnlyException, "������Ϊֻд");
		EXCEPTIONCLASS(GetOnlyException, "������Ϊֻ��");
		EXCEPTIONCLASS(ArgumentNullException, "����Ϊnull");
		EXCEPTIONCLASS(UnHandledException, "δ�����쳣");
		EXCEPTIONCLASS(KeyNotFoundException, "�Ҳ�����");
		EXCEPTIONCLASS(InterfaceNoImplementException, "�ӿ�δʵ���쳣");

		class IndexOutOfException;

		class BadAllocateException;

		class ParserException;


		template<class T,class...Args>
		void Throw(Args&&...args);
	}
}