#pragma once
namespace HL
{
	namespace System
	{
		//反射
		namespace Reflection
		{
			//类型
			class Type;
			//成员
			class Member
			{
				String m_name;
				UPointer::weakuptr<Type> m_owner_type;
				UPointer::weakuptr<Type> m_self_type;
			public:
				String const&GetName()const {
					return m_name;
				}
				uptr<Type> GetType()const {
					return m_self_type.ToUPtr();
				}
			};
		}
	}
}