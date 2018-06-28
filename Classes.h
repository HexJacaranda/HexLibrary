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
			template<class TKey, class TValue>
			class KeyValuePair
			{
				TKey* m_key = nullptr;
				TValue* m_value = nullptr;
			public:
				KeyValuePair() = default;
				TKey &Key() {
					return *m_key;
				}
				TValue &Value() {
					return *m_value;
				}
				TKey const&Key()const {
					return *m_key;
				}
				TValue const&Value()const {
					return *m_value;
				}
				void Set(TKey &Key, TValue&Value) {
					this->m_key = &Key;
					this->m_value = &Value;
				}
				KeyValuePair(TKey&Key, TValue&Value) :m_key(&Key), m_value(&Value) {}
				KeyValuePair&operator=(KeyValuePair const&rhs) {
					this->m_key = rhs.m_key;
					this->m_value = rhs.m_value;
					return *this;
				}
			};
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