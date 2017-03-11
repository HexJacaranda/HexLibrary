#pragma once
namespace HL
{
	namespace System
	{
		namespace Memory
		{
			class Allocator;
			template<class T>class MemoryManager;
		}	
		namespace UPointer
		{
			template<class U>class uptr;
			class uobject;
		}
		namespace Generic
		{
			template<class TT>class Array;
			template<class T>
			using UArray = UPointer::uptr<Array<T>>;
		}
		namespace Functional
		{
			template<class R,class...Args>
			class Delegate;
			template<class R, class...Args>
			using UDelegate = UPointer::uptr<Delegate<R, Args...>>;
		}
		namespace Reference
		{
			class IntPtr;
			template<class T>class COMPtr;
		}
		namespace Threading
		{
			template<class T>class AsyncResult;
			class AtomicCounter;
			class RWLock;
			class ConditionVariable;
			class Thread;
			class Mutex;

			typedef UPointer::uptr<Thread> UThread;
			
			typedef UPointer::uptr<RWLock> URWLock;
			typedef UPointer::uptr<ConditionVariable> UConditionVariable;
		}
		namespace Property
		{
			template<class T>class property;
		}
		namespace Container {
			template<class...Args>class Tuple;
		}
		template<class CharT>class BasicString;
		typedef BasicString<wchar_t> String;
		typedef UPointer::uptr<String> UString;
	}
	namespace Cast
	{
		template<class InT, class OutT>class PSafeCast;
	}
	namespace Exception
	{
		template<class T>
		class ExceptionHandle;
	}
}