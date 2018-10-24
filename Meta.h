#pragma once
namespace HL
{
	namespace Meta
	{
		//表示一个序列
		template<class...Args>
		using Sequence = Template::TypeList<Args...>;
		
	}
}