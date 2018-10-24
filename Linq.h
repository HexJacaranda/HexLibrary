#pragma once
namespace HL
{
	namespace System
	{
		namespace Linq
		{
			template<class T>
			using UEnumerator = uptr<Iteration::IEnumerator<T>>;

			//Where枚举器
			template<class T,class Functor>
			class WhereEnumerator :public Iteration::IEnumerator<T>
			{
				UEnumerator<T> m_iter;
				Functor m_predicate;
				typedef Iteration::IEnumerator<T> Base;
			public:
				WhereEnumerator(WhereEnumerator const&rhs) :m_iter(rhs.m_iter.Clone()), Base(rhs.CurrentObject), m_predicate(rhs.m_predicate) {}
				WhereEnumerator(UEnumerator<T> const&target, Functor const& predicate) :m_iter(target), m_predicate(predicate)
				{
					this->CurrentObject = &m_iter->Current();
					if(!m_predicate(*this->CurrentObject))
						this->MoveNext();
				}
				virtual Iteration::EnumerationResult MoveNext() final {
					Iteration::EnumerationResult result;
					for (;;)
					{
						result = m_iter->MoveNext();
						if (result == Iteration::EnumerationResult::EOE)
							break;
						this->CurrentObject = &m_iter->Current();
						if (m_predicate(*this->CurrentObject))
							return Iteration::EnumerationResult::Successful;
					}
					return Iteration::EnumerationResult::EOE;
				}
				virtual WhereEnumerator& operator=(Iteration::IEnumerator<T>const&rhs)final {
					m_iter.SetValue(static_cast<WhereEnumerator const&>(rhs).m_iter);
					this->CurrentObject = static_cast<WhereEnumerator const&>(rhs).CurrentObject;
					return *this;
				}
				virtual void* ClonePtr()const final {
					return new WhereEnumerator(*this);
				}
				virtual ~WhereEnumerator() {}
			};

			//选择迭代器
			template<class FromT, class ToT, class Functor>
			class SelectEnumerator :public Iteration::IEnumerator<ToT>
			{
			public:
				typedef Iteration::IEnumerator<ToT> Base;
				typedef ToT T;
				Functor m_cast;
				UEnumerator<FromT> m_iter;
			public:
				SelectEnumerator(UEnumerator<FromT> const& begin, Functor const& target) :m_cast(target), m_iter(begin), Base(&target(const_cast<FromT&>(begin->Current()))){}
				SelectEnumerator(SelectEnumerator const&rhs) :m_cast(rhs.m_cast), m_iter(rhs.m_iter.Clone()), Base(rhs.CurrentObject) {}
				virtual Iteration::EnumerationResult MoveNext() final {
					Iteration::EnumerationResult ret = m_iter->MoveNext();
					this->CurrentObject = &m_cast(m_iter->Current());
					return ret;
				}
				virtual SelectEnumerator& operator=(Iteration::IEnumerator<T>const&rhs) final {
					this->m_iter.SetValue(static_cast<SelectEnumerator const&>(rhs).m_iter);
					return *this;
				}
				virtual void* ClonePtr()const final {
					return new SelectEnumerator(*this);
				}
				virtual ~SelectEnumerator() {}
			};

			//连接迭代器
			template<class FirstT, class SecondT, class Functor>
			class JoinEnumerator :public Iteration::IEnumerator<Generic::KeyValuePair<FirstT, SecondT>>
			{
			public:
				typedef Generic::KeyValuePair<FirstT, SecondT> T;
				typedef Iteration::IEnumerator<T> Base;
			private:
				T m_current;
				UEnumerator<FirstT> m_first;
				Iteration::EnumerationResult m_sign = Iteration::EnumerationResult::Successful;
				UEnumerator<SecondT> m_second;
				UEnumerator<SecondT> m_second_beg;
				Functor m_predicator;
			public:
				JoinEnumerator(UEnumerator<FirstT> const& first, UEnumerator<SecondT> const& second, Functor const& target) :m_first(first), m_second(second), m_second_beg(second.Clone()), m_predicator(target), Base(&this->m_current) {
					this->MoveNext();
				}
				JoinEnumerator(JoinEnumerator const& rhs) :m_first(rhs.m_first.Clone()), m_second(rhs.m_second.Clone()), m_second_beg(rhs.m_second_beg.Clone()), m_predicator(rhs.m_predicator),m_current(rhs.m_current),m_sign(rhs.m_sign),Base(&this->m_current) {}
				virtual Iteration::EnumerationResult MoveNext() final {
					if (m_sign == Iteration::EnumerationResult::EOE)
						return m_sign;
					do
					{
						do 
						{
							if (m_predicator(this->m_first->Current(), this->m_second->Current()))
							{
								this->m_current.Set(this->m_first->Current(), this->m_second->Current());
								if (this->m_second->MoveNext() == Iteration::EnumerationResult::EOE)
								{
									m_sign = this->m_first->MoveNext();
									this->m_second.SetValue(this->m_second_beg);
								}
								return Iteration::EnumerationResult::Successful;
							}
						} 
						while (this->m_second->MoveNext() != Iteration::EnumerationResult::EOE);
						this->m_second.SetValue(this->m_second_beg);
					} 
					while (this->m_first->MoveNext() != Iteration::EnumerationResult::EOE);
					return Iteration::EnumerationResult::EOE;
				}
				virtual JoinEnumerator& operator=(Iteration::IEnumerator<T>const&rhs) final {
					JoinEnumerator const& another = static_cast<JoinEnumerator const&>(rhs);
					this->m_current = another.m_current;
					this->CurrentObject = another.CurrentObject;
					this->m_first.SetValue(another.m_first);
					this->m_second.SetValue(another.m_second);
					this->m_second_beg.SetValue(another.m_second_beg);
					return *this;
				}
				virtual void* ClonePtr()const final {
					return new JoinEnumerator(*this);
				}
				virtual ~JoinEnumerator() {}
			};
			
			//排序顺序
			enum class SortOrder
			{
				//升序
				Ascending,
				//降序
				Descending
			};

			//排序迭代器
			template<class T,class Functor>
			class OrderEnumerator :public Iteration::IEnumerator<T>
			{
				typedef Iteration::IEnumerator<T> Base;
				Functor m_compare;
				Memory::MemoryManager<Reference::Ref<T>> m_ordered_objects;
				Reference::Ref<T>*end_ptr;
				Reference::Ref<T>*beg_ptr;
			public:
				OrderEnumerator(OrderEnumerator const& rhs) :m_compare(rhs.m_compare), beg_ptr(rhs.beg_ptr), end_ptr(rhs.end_ptr), Base(rhs.CurrentObject) {}
				OrderEnumerator(UEnumerator<T> const&target, Functor const& compare, SortOrder order) :m_compare(compare), Base(nullptr)
				{
					UEnumerator<T> iter = target.Clone();
					do
					{
						m_ordered_objects.Append(Reference::Ref<T>(iter->Current()));
					} while (iter->MoveNext() == Iteration::EnumerationResult::Successful);
					if (order == SortOrder::Ascending)
						Algorithm::Sort(m_ordered_objects.GetMemoryBlock(), 0, m_ordered_objects.GetUsedSize() - 1, [this](Reference::Ref<T>&left, Reference::Ref<T>&right) { return m_compare(left, right); });
					else
						Algorithm::Sort(m_ordered_objects.GetMemoryBlock(), 0, m_ordered_objects.GetUsedSize() - 1, [this](Reference::Ref<T>&left, Reference::Ref<T>&right) { return (-1)*m_compare(left, right); });
					this->beg_ptr = m_ordered_objects.GetMemoryBlock();
					this->end_ptr = this->beg_ptr + m_ordered_objects.GetUsedSize();
					this->CurrentObject = &beg_ptr->operator T &();
				}
				virtual Iteration::EnumerationResult MoveNext() final {
					this->beg_ptr++;
					this->CurrentObject = &beg_ptr->operator T &();
					if (this->beg_ptr == this->end_ptr)
						return Iteration::EnumerationResult::EOE;
					return Iteration::EnumerationResult::Successful;
				}
				virtual OrderEnumerator& operator=(Iteration::IEnumerator<T>const&rhs) final {
					OrderEnumerator const& another = static_cast<OrderEnumerator const&>(rhs);
					this->CurrentObject = another.CurrentObject;
					this->m_ordered_objects = another.m_ordered_objects;
					return *this;
				}
				virtual void* ClonePtr()const final {
					return new OrderEnumerator(*this);
				}
				virtual ~OrderEnumerator() {}
			};

			//Linq方法拓展(基枚举器)
			template<typename T>
			class LinqBase :public Iteration::IEnumerable<T>
			{
			public:
				//根据条件选择
				template<class Functor>
				uptr<LinqQueryResult<T>> Where(Functor const& Target)const {
					return newptr<LinqQueryResult<T>>(
						newptr<WhereEnumerator<T, Functor>>(this->GetEnumerator(), Target)
						);
				}
				//根据成员选择
				template<class Functor>
				uptr<LinqQueryResult<typename Template::RemoveAllExtents<typename Template::GetFunctionInfo<Functor>::R>::T>> Select(Functor const& Target)const {
					typedef typename Template::RemoveAllExtents<typename Template::GetFunctionInfo<Functor>::R>::T ToT;
					return newptr<LinqQueryResult<ToT>>(
						newptr<SelectEnumerator<T, ToT, Functor>>(this->GetEnumerator(), Target)
						);
				}
				//联合
				template<class SecondT, class Functor>
				uptr<LinqQueryResult<Generic::KeyValuePair<T, SecondT>>> Join(Iteration::IEnumerable<SecondT> const&Another, Functor const&Target)const {
					return newptr<LinqQueryResult<Generic::KeyValuePair<T, SecondT>>>(
						newptr<JoinEnumerator<T, SecondT, Functor>>(this->GetEnumerator(), Another.GetEnumerator(), Target)
						);
				}
				//排序
				template<class Functor> 
				uptr<LinqQueryResult<T>> OrderBy(Functor const& compare, SortOrder order = SortOrder::Ascending)const
				{
					return newptr<LinqQueryResult<T>>(newptr<OrderEnumerator<T, Functor>>(this->GetEnumerator(), compare, order));
				}
				//排序
				uptr<LinqQueryResult<T>> OrderBy(SortOrder order = SortOrder::Ascending)const
				{
					auto sort = [](T&l, T&r) {return l >= r ? 1 : -1; };
					return newptr<LinqQueryResult<T>>(newptr<OrderEnumerator<T, decltype(sort)>>(this->GetEnumerator(), sort, order));
				}
			};

			//Linq查询结果
			template<typename T>
			class LinqQueryResult:public LinqBase<T>
			{
				UEnumerator<T> m_iter;
			public:
				LinqQueryResult(UEnumerator<T> const&target) : m_iter(target) {}
				virtual uptr<Iteration::IEnumerator<T>> GetEnumerator()const final
				{
					return m_iter.Clone();
				}
				virtual ~LinqQueryResult() {}
			};

			template<class T>
			struct Interface::EnumerableSupportInterface<LinqQueryResult<T>>
			{
				typedef uptr<Iteration::Iterator<T>> IteratorType;
				typedef uptr<Iteration::Iterator<T>> ConstIteratorType;
			};
		}
	}
}