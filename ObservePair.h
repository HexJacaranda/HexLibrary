#pragma once
namespace HL
{
	namespace System
	{
		namespace Generic
		{
			template<class TKey, class TValue>
			class ObservePair
			{
				TKey* m_key = nullptr;
				TValue* m_value = nullptr;
			public:
				ObservePair() = default;
				ObservePair(TKey const&Key, TValue const&Value) :m_key(const_cast<TKey*>(&Key)), m_value(const_cast<TValue*>(&Value)) {}
				inline TKey &Key() {
					return *m_key;
				}
				inline TKey const&Key()const {
					return *m_key;
				}
				inline void Key(TKey const&Value) {
					this->m_key = const_cast<TKey*>(&Value);
				}
				inline TValue &Value() {
					return *m_value;
				}
				inline TValue const&Value()const {
					return *m_value;
				}
				inline void Value(TValue const&Target) {
					this->m_value = const_cast<TValue*>(&Target);
				}
			};
		}
	}
}