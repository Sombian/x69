extern "C" int system(const char* inˀ);
static int WindowsConsoleSubSystem {[]
{
	#ifdef _WIN32//__{UTF8}__);
	system("chcp 65001 > nul");
	#endif//WIN32//__{UTF8}__);

	return 0; // dummy? return
}()};

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#ifndef DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN

#endif //CLOSE_CONFIG_IMPLEMENT_WITH_MAIN
#ifdef DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN

#include "doctest.h"

#endif //CLOSE_CONFIG_IMPLEMENT_WITH_MAIN
