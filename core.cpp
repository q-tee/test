#include <q-tee/common/common.h>

#ifdef Q_OS_WINDOWS
#define _CRT_SECURE_NO_WARNINGS

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#endif
#include <iomanip>

#include "units/crt.h"
#include "units/hash.h"

int main()
{
#ifdef Q_OS_WINDOWS
	const HANDLE hOutput = ::GetStdHandle(STD_OUTPUT_HANDLE);
	if (hOutput == INVALID_HANDLE_VALUE)
		return EXIT_FAILURE;

	// enable virtual terminal to support ANSI escape-codes
	if (DWORD dwConsoleMode = 0UL; !::GetConsoleMode(hOutput, &dwConsoleMode) || !::SetConsoleMode(hOutput, dwConsoleMode | ENABLE_VIRTUAL_TERMINAL_PROCESSING))
		return EXIT_FAILURE;

	::SetConsoleOutputCP(CP_UTF8);
#endif

	CUnitTest hash("HASH");
	hash.Add(UNIT::HASH::Assert(), true);
	hash.Add(UNIT::HASH::AssertConst(), true);
	hash.Report();

	CUnitTest crt("CRT");
	crt.Add(UNIT::CRT::Assert(), true);
	crt.Add(UNIT::CRT::AssertConst(), true);
	crt.Report();
	
	return EXIT_SUCCESS;
}
