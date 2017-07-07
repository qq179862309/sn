#include "stdafx.h"
#include "Main.h"

#include  <dbghelp.h> 
#pragma comment(lib,  "dbghelp.lib")
#include <WinSock2.h>
#pragma comment(lib,"Ws2_32.lib")


LONG WINAPI	UnhandledExceptionFilter(struct _EXCEPTION_POINTERS* ExceptionInfo)
{
	TCHAR szBuf[MAX_PATH] = { 0 };
	GetModuleFileName((HMODULE)CMain::getInstance()->getHandle(), szBuf, MAX_PATH);
	CString   strDumpFile;
	strDumpFile.Format(_T("%s.dmp"), szBuf, CTime::GetCurrentTime().GetTickCount());
	HANDLE   hFile = CreateFile(strDumpFile, GENERIC_WRITE, FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile != INVALID_HANDLE_VALUE)
	{
		MINIDUMP_EXCEPTION_INFORMATION   ExInfo;

		ExInfo.ThreadId = ::GetCurrentThreadId();
		ExInfo.ExceptionPointers = ExceptionInfo;
		ExInfo.ClientPointers = NULL;

		//   write   the   dump
		BOOL   bOK = MiniDumpWriteDump(GetCurrentProcess(), GetCurrentProcessId(), hFile, MiniDumpNormal, &ExInfo, NULL, NULL);
		CloseHandle(hFile);

	}
	return EXCEPTION_EXECUTE_HANDLER;
}

CMain::CMain()
{
	m_bSocket = false;
	m_bCOM = false;
	m_bType = 0;
}


CMain::~CMain()
{
	if (m_bSocket)
		WSACleanup();
	if (m_bCOM)
		CoUninitialize();
}

CMain*	CMain::getInstance()
{
	static CMain	mainGlobal;
	return &mainGlobal;
}

HANDLE	CMain::getHandle()
{
	HANDLE hRetHandle = NULL;
	switch (m_bType)
	{
	case 0:
		break;
	case 1:
		hRetHandle = (HANDLE)m_hInhstance;
		break;
	case 2:
		hRetHandle = (HANDLE)m_hModule;
		break;
	default:
		break;
	}
	return hRetHandle;
}

int		CMain::appMain(HINSTANCE hInhstance, LPTSTR lpCmdLine, int nShow)
{
	int nRet = 0;
	m_hInhstance = hInhstance;
	m_nShow = nShow;
	m_bType = 1;

	nRet = initSocket();
	retIntError(nRet);
	nRet = initCOM();
	retIntError(nRet);
	return 0;
}

int		CMain::dllMain(HMODULE	hModule)
{
	int nRet = 0;
	m_hModule = hModule;
	m_bType = 2;

	nRet = initSocket();
	retIntError(nRet);
	nRet = initCOM();
	retIntError(nRet);

	return 0;
}

int		CMain::initSocket()
{
	WORD wVersionRequested;
	WSADATA wsaData;
	int nRet = 0;

	wVersionRequested = MAKEWORD(2, 2);

	nRet = WSAStartup(wVersionRequested, &wsaData);
	retIntError(nRet);

	if (LOBYTE(wsaData.wVersion) != 2 || HIBYTE(wsaData.wVersion) != 2) 
	{
		WSACleanup();
		nRet = 2;
		retIntError(nRet);
	}
	m_bSocket = true;
	return 0;
}

int		CMain::initCOM()
{
	int nRet = 0;
	nRet = ::CoInitialize(NULL);
	retIntError(nRet);

	m_bCOM = true;

	return 0;
}

int		CMain::initDUMP(LPTOP_LEVEL_EXCEPTION_FILTER lpTopLevelExceptionFilter)
{
	if (lpTopLevelExceptionFilter != NULL)
	{
		SetUnhandledExceptionFilter(lpTopLevelExceptionFilter);
	}
	else
	{
		SetUnhandledExceptionFilter(UnhandledExceptionFilter);
	}
	return 0;
}

int		CMain::releaseResources(tstring strPath, tstring strType, WORD wResID)
{
	int nRet = 0;
	// 资源大小  
	DWORD   dwWrite = 0;

	// 创建文件  
	HANDLE  hFile = CreateFile(strPath.c_str(), GENERIC_WRITE, FILE_SHARE_WRITE, NULL,
		CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		nRet = 1;
		retIntError(nRet);
	}

	// 查找资源文件中、加载资源到内存、得到资源大小  
	HRSRC   hRsc = FindResource(NULL, MAKEINTRESOURCE(wResID), strType.c_str());
	if (hRsc == NULL)
	{
		CloseHandle(hFile);
		nRet = 2;
		retIntError(nRet);
	}
	HGLOBAL hG = LoadResource(NULL, hRsc);
	if (hG == NULL)
	{
		CloseHandle(hFile);
		nRet = 3;
		retIntError(nRet);
	}
	DWORD   dwSize = SizeofResource(NULL, hRsc);
	if (dwSize == 0)
	{
		CloseHandle(hFile);
		nRet = 4;
		retIntError(nRet);
	}
	// 写入文件  
	WriteFile(hFile, hG, dwSize, &dwWrite, NULL);
	CloseHandle(hFile);
	return 0;
}

int		CMain::getCurrentPath(tstring& strPath)
{
	int nRet = 0;
	TCHAR szBuf[MAX_PATH] = { 0 };
	int nLength = GetModuleFileName((HMODULE)getHandle(), szBuf, MAX_PATH * sizeof(TCHAR));
	while (nLength)
	{
		if (szBuf[nLength] == '\\')
		{
			szBuf[nLength + 1] = '\0';
			strPath = szBuf;
			break;
		}
		nLength--;
	}
	return nRet;
}