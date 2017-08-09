#pragma once
namespace HL
{
	namespace System
	{
		namespace Container
		{
			namespace Internal
			{
				template<unsigned inx, class T, class...Rest>
				class TupleElement :public TupleElement<inx - 1, Rest...> {};

				template<class T, class...Rest>
				class TupleElement<0, T, Rest...>
				{
				public:
					typedef Tuple<T, Rest...> ThisT;
				};


				template<class TT, class...Args>class TupleTypeFind;

				template<class TT>
				class TupleTypeFind<TT> {};

				template<class TT, class T, class...Rest>
				class TupleTypeFind<TT, T, Rest...> :public Template::If<Template::IsSame<T, TT>::R, TupleTypeFind<TT>, TupleTypeFind<TT, Rest...>>::T, public Template::EnableIf<Template::IsSame<TT, T>::R, Tuple<T, Rest...>>
				{

				};
				template<class...Ts>
				class TupleMaker;

				template<class T, class...Rest>
				class TupleMaker<T, Rest...>
				{
				public:
					static inline void Make(Tuple<T, Rest...>&tuple, T const& first, Rest const&...rest) {
						tuple.Value = first;
						TupleMaker<Rest...>::Make(tuple.GetRest(), rest...);
					}
				};

				template<>
				class TupleMaker<>
				{
				public:
					static inline void Make(Tuple<>&tuple)
					{
						return;
					}
				};
			}

			template<class...Ts>
			static Tuple<Ts...> MakeTuple(Ts const&...rest)
			{
				Tuple<Ts...> Ret;
				Internal::TupleMaker<Ts...>::Make(Ret, rest...);
				return static_cast<Tuple<Ts...>&&>(Ret);
			}

			template<>
			class Tuple<>
			{
			public:
				Tuple() {}
				~Tuple() {}
			};
			template<>
			class Tuple<void>
			{
			public:
				Tuple() {}
				~Tuple() {}
			};

			//ิชื้
			template<class T, class...Rest>
			class Tuple<T, Rest...> :private Tuple<Rest...>
			{
			public:
				T Value;
				Tuple() {}
				template<class FirstT, class...Others>
				constexpr Tuple(FirstT const& first, Others const&...rest) :Value(const_cast<FirstT&>(first)), Tuple<Rest...>(rest...)
				{
				}
				template<class...Others>
				constexpr Tuple(Tuple<Others...>const&tuple) : Value(tuple.Value), Tuple<Rest...>(tuple.GetRest())
				{
				}

				inline Tuple<Rest...>&GetRest()noexcept
				{
					return (Tuple<Rest...>&)(*this);
				}
				constexpr inline Tuple<Rest...> const&GetRest()const noexcept
				{
					return (Tuple<Rest...>&)(*this);
				}

				inline auto GetTargetRest(index_t index) {
					auto ptr = this;
					for (index_t i = 0; i < index; ++i) {
						ptr = this->GetRest();
					}
					return ptr;
				}
				template<unsigned inx>
				inline auto&GetAt()noexcept
				{
					return ((typename Internal::TupleElement<inx, T, Rest...>::ThisT*)(this))->Value;
				}
				template<unsigned inx>
				constexpr inline auto const&GetAt()const noexcept
				{
					return ((typename Internal::TupleElement<inx, T, Rest...>::ThisT*)(this))->Value;
				}
				template<class AnyT>
				inline AnyT&GetBy()
				{
					return ((typename Internal::TupleTypeFind<AnyT, T, Rest...>::T*)(this))->Value;
				}
				template<class AnyT>
				constexpr inline AnyT const&GetBy()const
				{
					return ((typename Internal::TupleTypeFind<AnyT, T, Rest...>::T*)(this))->Value;
				}
				~Tuple()
				{
				}
			};

			class GetArgsAt {
				template<class T, class...Args>
				static auto const& Get(index_t i, index_t target, T const&current, Args const&...rest) {
					if (i == target)
						return current;
					else
						return Get(++i, target, rest...);
				}
				template<class T>
				static T const& Get(index_t i, index_t target, T const&currnet) {
					return currnet;
				}
			public:
				template<class...Args>
				inline static auto const& GetAt(index_t index, Args const&...args) {
					return Get(0, index, args...);
				}
			};
		}
	}
}