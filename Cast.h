#pragma once
namespace HL
{
	namespace Cast
	{
		namespace Internal
		{
			template<bool castable,class T,class U>
			struct PCast
			{
				static U Cast(T value) {
					return static_cast<U>(value);
				}
			};
			template<class T, class U>
			struct PCast<false,T,U>
			{
				static U Cast(T value) {
					return nullptr;
				}
			};
		}

		template<class InT,class OutT>
		class PSafeCast
		{
		public:
			static inline OutT* Cast(InT* value) {
				return Internal::PCast<Template::Or<Template::IsBaseOf<InT, OutT>::R, Template::IsBaseOf<OutT, InT>::R, true>::R, InT*, OutT*>::Cast(value);
			}
		};
	}
}