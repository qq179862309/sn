#include "stdafx.h"
#include "hook\Hook.h"
#include "SN.h"


CHook::CHook()
{
}


CHook::~CHook()
{
}

int	CHook::hookWindowAPI(tstring strModule, char* funcName, DWORD dwFunction)
{
	int nRet = 0;
	HMODULE hLibModule = GetModuleHandle(strModule.c_str());
	if (hLibModule == NULL)
	{
		hLibModule = LoadLibrary(strModule.c_str());
		if (hLibModule == NULL)
		{
			nRet = 1;
			retIntError(nRet);
		}
	}
	FARPROC pFunction = GetProcAddress(hLibModule, funcName);
	if (pFunction == NULL)
	{
		nRet = 2;
		retIntError(nRet);
	}
	DWORD dwCode = (DWORD)pFunction - dwFunction - 5;
	byte bCode[5] = { 0x90 };
	bCode[0] = 0xE9;
	memcpy(&bCode[1], (LPVOID)dwCode, sizeof(DWORD));

	DWORD dwProtect = 0;
	if (!VirtualProtect((LPVOID)pFunction, 5, PAGE_EXECUTE_READWRITE, &dwProtect))
	{
		nRet = 3;
		retIntError(nRet);
	}
	memcpy((LPVOID)pFunction, (const void*)bCode, sizeof(bCode));
	return 0;
}