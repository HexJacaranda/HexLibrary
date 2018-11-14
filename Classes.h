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
			template<class U>
			class uptr;
			class uobject;
		}
		template<class T>
		using uptr = UPointer::uptr<T>;
		using uobject = UPointer::uobject;
		namespace Generic
		{
			template<class TKey, class TValue>class ObservePair;
			template<class T>
			class ICollection;
			template<class TKey, class TValue>
			class IDictionary;
			template<class TT>
			class Array;
			template<class TKey, class TValue>
			class Dictionary;
			template<class T>
			using UArray = UPointer::uptr<Array<T>>;
			template<class T>
			using UICollection = UPointer::uptr<ICollection<T>>;
		}
		namespace Functional
		{
			class Auto;
			template<class R,class...Args>
			class Delegate;
			template<class R, class...Args>
			using UDelegate = UPointer::uptr<Delegate<R, Args...>>;
		}
		namespace Reference
		{
			template<class T>class Ref;
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
		namespace Linq
		{
			template<typename T>
			class LinqQueryResult;
			template<typename T>
			class LinqBase;

		}
		template<class CharT>class BasicString;
		typedef BasicString<wchar_t> String;
		typedef UPointer::uptr<String> UString;
		namespace Json
		{
			class IJsonValue;
			class JsonObject;
			class JsonArray;
			class JsonValue;
			typedef UPointer::uptr<IJsonValue> UIJsonValue;
			typedef UPointer::uptr<JsonObject> UJsonObject;
			typedef UPointer::uptr<JsonArray> UJsonArray;
			typedef UPointer::uptr<JsonValue> UJsonValue;
		}
		namespace Concurrency
		{
			template<class T>
			class ConcurrentNode;
			template<class T>
			class ConcurrentNodeWrapper;
			template<class T>
			class ConcurrentStack;
			template<class T>
			class ConcurrentQueue;
		}
		namespace RegularExpression
		{
			class Regex;
			class MatchResult;

			class Edge;
			class Status;
			class ContentBase;
			class NFA;
			class Resource;
		}
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