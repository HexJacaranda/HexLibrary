#pragma once
namespace HL
{
	namespace Meta
	{
		//��ʾһ������
		template<class...Args>
		using Sequence = Template::TypeList<Args...>;
		
	}
}