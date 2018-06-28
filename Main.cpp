#include "Include.h"
#include <typeinfo>
#define _CRTDBG_MAP_ALLOC
inline void EnableMemLeakCheck()
{
	_CrtSetDbgFlag(_CrtSetDbgFlag(_CRTDBG_REPORT_FLAG) | _CRTDBG_LEAK_CHECK_DF);
}
using namespace HL::System;
using namespace HL::Exception;
using namespace HL::System::Generic;
using namespace HL::System::Reference;
using namespace HL::System::Functional;
using namespace HL::GC;
using namespace HL::System::Threading;
using namespace HL::System::Iteration;



int main()
{
	EnableMemLeakCheck();
	std::wcout.imbue(std::locale("chs"));
	Console::Pause();
	return 0;
}



