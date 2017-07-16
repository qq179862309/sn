#include "stdafx.h"
#include "hook\Hook.h"
#include "SN.h"



CHook::CHook()
{
	m_MapInfo.clear();
	m_MapHwndProc.clear();
}


CHook::~CHook()
{
}

CHook*	CHook::getInstance()
{
	static	CHook	hookGlobal;
	return &hookGlobal;
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
	int nLength = 0;
#ifdef _WIN64
	DWORD dwCode = (DWORD)dwFunction - (DWORD)pFunction - 9;
	byte bCode[9] = { 0x90 };
	nLength = 9;
#else
	DWORD dwCode = (DWORD)dwFunction - (DWORD)pFunction - 5;
	byte bCode[5] = { 0x90 };
	nLength = 5;
#endif
	bCode[0] = 0xE9;
	memcpy(&bCode[1], (LPVOID)&dwCode, sizeof(DWORD));

	DWORD dwProtect = 0;
	if (!VirtualProtect((LPVOID)pFunction, nLength, PAGE_EXECUTE_READWRITE, &dwProtect))
	{
		nRet = 3;
		retIntError(nRet);
	}
	memcpy((LPVOID)pFunction, (const void*)bCode, sizeof(bCode));
	return 0;
}

int	CHook::hookFunction(tstring strKey, DWORD dwFuncAddress, byte* lpNewCode, int nNewLength)
{
	int nRet = 0;
	DWORD dwProtect = 0;
	if (!VirtualProtect((LPVOID)dwFuncAddress, nNewLength, PAGE_EXECUTE_READWRITE, &dwProtect))
	{
		nRet = 1;
		retIntError(nRet);
	}
	PHOOKINFO pHookInfo = new HOOKINFO;
	pHookInfo->dwFuncAddress = dwFuncAddress;
	pHookInfo->pNewCode = new byte[nNewLength];
	memcpy(pHookInfo->pNewCode, lpNewCode, nNewLength);
	pHookInfo->pOldCode = new byte[nNewLength];

	memcpy(pHookInfo->pOldCode, (const void*)dwFuncAddress, nNewLength);
	memcpy((void*)dwFuncAddress, pHookInfo->pNewCode, nNewLength);

	m_MapInfo.insert(MAP_HOOKINFO::value_type(strKey, pHookInfo));

	return 0;
}

int		CHook::unHookFunction(tstring strKey)
{
	ITER_HOOKINFO iter = m_MapInfo.find(strKey);
	if (iter != m_MapInfo.end())
	{
		PHOOKINFO pHookInfo = iter->second;
		memcpy((void*)pHookInfo->dwFuncAddress, pHookInfo->pOldCode, pHookInfo->nNewLength);
		delete []pHookInfo->pNewCode;
		delete []pHookInfo->pOldCode;
		delete pHookInfo;

		m_MapInfo.erase(iter);
	}
	return 0;
}

int		CHook::hookWindowProc(HWND hWnd, DWORD dwNewProc, DWORD& dwOldProc)
{
	int nRet = 0;
	LONG lWndProc = GetWindowLong(hWnd, GWL_WNDPROC);
	if (lWndProc == NULL)
	{
		nRet = 1;
		retIntError(nRet);
	}
	dwOldProc = lWndProc;
	SetWindowLong(hWnd, GWL_WNDPROC, (LONG)dwNewProc);
	m_MapHwndProc.insert(MAP_HWNDPROC::value_type(hWnd, lWndProc));

	return 0;
}

int		CHook::unHookWindowProc(HWND hWnd)
{
	ITER_HWNDPROC iter = m_MapHwndProc.find(hWnd);
	if (iter != m_MapHwndProc.end())
	{
		SetWindowLong(hWnd, GWL_WNDPROC, (LONG)iter->second);
		m_MapHwndProc.erase(iter);
	}
	return 0;
}

int		CHook::windowProcCall(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, int& nResult)
{
	int nRet = -1;
	ITER_HWNDPROC iter = m_MapHwndProc.find(hWnd);
	if (iter != m_MapHwndProc.end())
	{
		nResult = CallWindowProc((WNDPROC)iter->second, hWnd, uMsg, wParam, lParam);
		nRet = 0;
	}
	return nRet;
}