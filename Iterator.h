#pragma once
namespace HL
{
#undef Yield
	namespace System
	{
		namespace Iteration
		{
			//枚举结果
			enum class EnumerationResult
			{
				//枚举结束
				EOE,
				//成功
				Successful,
				//失败
				Failed,
				//不支持的操作
				NotSupported
			};
			//枚举器
			template<class T>
			class IEnumerator:public Interface::ICloneable
			{
			protected:
				T*CurrentObject;
				IEnumerator(T*Current) :CurrentObject(Current) {}
				IEnumerator() {}
			public:
				virtual EnumerationResult MoveNext() = 0;
				inline T& Current() {
					return *CurrentObject;
				}
				inline T const& Current()const {
					return *CurrentObject;
				}
				virtual IEnumerator<T>& operator=(IEnumerator<T>const&) = 0;
				virtual void* ClonePtr()const = 0;
			};
			//迭代器
			template<class T>
			class Iterator:public Interface::ICloneable
			{
				uptr<IEnumerator<T>> m_iter;
				bool m_eoe = false;
			public:
				Iterator(uptr<IEnumerator<T>> const&target) :m_iter(target) {}
				Iterator(Iterator const&rhs) :m_iter(rhs.m_iter.Clone()), m_eoe(rhs.m_eoe) {}
				Iterator(){}
				Iterator<T>& operator++() {
					EnumerationResult result = this->m_iter->MoveNext();
					if (result == EnumerationResult::EOE)
						m_eoe = true;
					else if (result == EnumerationResult::Failed)
						HL::Exception::Throw<HL::Exception::UnHandledException>();
					else if (result == EnumerationResult::NotSupported)
						HL::Exception::Throw<HL::Exception::InterfaceNoImplementException>();
					return *this;
				}
				Iterator<T>& operator++(int) {
					return ++(*this);
				}
				T&operator*() {
					return m_iter->Current();
				}
				T const&operator*()const {
					return m_iter->Current();
				}
				bool operator==(Iterator<T> const&rhs)const {
					return m_eoe;
				}
				bool operator!=(Iterator<T> const&rhs)const {
					return !m_eoe;
				}
				Iterator<T>&operator=(Iterator<T> const&rhs) {
					if (rhs.m_iter.IsNull())
						this->m_iter = nullptr;
					if (this->m_iter.IsNull())
						this->m_iter = rhs.m_iter.Clone();
					else
						this->m_iter->SetValue(rhs.m_iter);
					return *this;
				}
				virtual void* ClonePtr()const {
					return new Iterator(*this);
				}
				static uptr<Iterator<T>>& End() {
					static uptr<Iterator<T>> end = Reference::newptr<Iterator<T>>();
					return end;
				}
			};

			template<class T>
			class IEnumerable
			{
			public:
				virtual uptr<IEnumerator<T>> GetEnumerator()const = 0;
#if	_FOR_LOOP_SYNTACTIC_SUGAR //for语法糖支持
				inline uptr<Iterator<T>> begin()const {
					return newptr<Iteration::Iterator<T>>(this->GetEnumerator());
				}
				inline uptr<Iterator<T>>& end()const {
					return Iterator<T>::End();
				}
#endif
			};

			template<class T>
			struct Interface::IteratorSupportInterface<Iterator<T>>
			{
				typedef T UnReferenceType;
			};
		}
	}
}