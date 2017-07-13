#include "StdAfx.h"
#include "log/Log.h"
#include <strsafe.h>

#pragma warning(disable:4995)

CLog*	CLog::getInstance()
{
	static	CLog	logGlobal;
	return &logGlobal;
}

CLog::CLog(void)
{
	memset(m_szDirectory, 0, MAX_PATH * sizeof(TCHAR));
	m_nYear = 0;
	m_nMonth = 0;
	m_nDay = 0;
	setDirectory();
}

CLog::~CLog(void)
{
}

int	CLog::setDirectory(LPTSTR lpDirectory)
{
	if (lpDirectory == NULL)
	{
		tstring strDirectory;
		CMain::getInstance()->getCurrentPath(strDirectory);
		strDirectory += _T("Log\\");
		lstrcpy(m_szDirectory, strDirectory.c_str());
	}
	else
	{
		lstrcpy(m_szDirectory, lpDirectory);
	}

	DWORD   dwAttr = GetFileAttributes(m_szDirectory);
	if (dwAttr == -1 || (dwAttr & FILE_ATTRIBUTE_DIRECTORY) == 0)
	{
		CreateDirectory(m_szDirectory, NULL);
	}
	UpdateFileTime();
	return 0;
}

VOID	CLog::UpdateFileTime()
{
	ATL::CTime	CurTime = ATL::CTime::GetTickCount();
	m_nYear = CurTime.GetYear();
	m_nMonth = CurTime.GetMonth();
	m_nDay = CurTime.GetDay();
}

VOID	CLog::DebugA(LPSTR lpstrFormat, ...)
{
	char szBuf[40960] = { 0 };

	va_list lpArgs;
	va_start(lpArgs, lpstrFormat);
	StringCbVPrintfA(szBuf, 40960, lpstrFormat, lpArgs);
	va_end(lpArgs);

	ATL::CTime CurTime = ATL::CTime::GetTickCount();
	char szOut[40960] = { 0 };
	int nLength = wsprintfA(szOut, "%d-%d-%d %d:%d:%d - [Debug] : %s\r\n", CurTime.GetYear(),
		CurTime.GetMonth(),
		CurTime.GetDay(),
		CurTime.GetHour(),
		CurTime.GetMinute(),
		CurTime.GetSecond(),
		szBuf);

	UpdateFileTime();

	TCHAR szFilePath[MAX_PATH] = { 0 };
	wsprintf(szFilePath, _T("%s\\%d%d%d.log"), m_szDirectory, m_nYear, m_nMonth, m_nDay);
	HANDLE hFile = CreateFile(szFilePath, GENERIC_ALL, FILE_SHARE_WRITE | FILE_SHARE_DELETE, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		return;
	}
	DWORD dwFileLength = GetFileSize(hFile, NULL);
	SetFilePointer(hFile, dwFileLength, NULL, FILE_BEGIN);

	DWORD dwWrite = 0;
	WriteFile(hFile, szOut, nLength, &dwFileLength, NULL);

	CloseHandle(hFile);
}

VOID	CLog::ErrorA(LPSTR lpstrFormat, ...)
{
	char szBuf[40960] = { 0 };

	va_list lpArgs;
	va_start(lpArgs, lpstrFormat);
	StringCbVPrintfA(szBuf, 40960, lpstrFormat, lpArgs);
	va_end(lpArgs);

	ATL::CTime CurTime = ATL::CTime::GetTickCount();
	char szOut[40960] = { 0 };
	int nLength = wsprintfA(szOut, "%d-%d-%d %d:%d:%d - [Error] : %s\r\n", CurTime.GetYear(),
		CurTime.GetMonth(),
		CurTime.GetDay(),
		CurTime.GetHour(),
		CurTime.GetMinute(),
		CurTime.GetSecond(),
		szBuf);

	UpdateFileTime();

	TCHAR szFilePath[MAX_PATH] = { 0 };
	wsprintf(szFilePath, _T("%s\\%d%d%d.log"), m_szDirectory, m_nYear, m_nMonth, m_nDay);
	HANDLE hFile = CreateFile(szFilePath, GENERIC_ALL, FILE_SHARE_WRITE | FILE_SHARE_DELETE, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		return;
	}
	DWORD dwFileLength = GetFileSize(hFile, NULL);
	SetFilePointer(hFile, dwFileLength, NULL, FILE_BEGIN);

	DWORD dwWrite = 0;
	WriteFile(hFile, szOut, nLength, &dwFileLength, NULL);

	CloseHandle(hFile);
}

VOID	CLog::DebugW(LPWSTR lpstrFormat, ...)
{
	TCHAR szBuf[40960] = { 0 };

	va_list lpArgs;
	va_start(lpArgs, lpstrFormat);
	StringCbVPrintfW(szBuf, 40960, lpstrFormat, lpArgs);
	va_end(lpArgs);

	USES_CONVERSION;

	ATL::CTime CurTime = ATL::CTime::GetTickCount();
	char szOut[40960] = { 0 };
	int nLength = wsprintfA(szOut, "%d-%d-%d %d:%d:%d - [Debug] : %s\r\n", CurTime.GetYear(),
		CurTime.GetMonth(),
		CurTime.GetDay(),
		CurTime.GetHour(),
		CurTime.GetMinute(),
		CurTime.GetSecond(),
		T2A(szBuf));

	UpdateFileTime();

	TCHAR szFilePath[MAX_PATH] = { 0 };
	wsprintf(szFilePath, _T("%s\\%d%d%d.log"), m_szDirectory, m_nYear, m_nMonth, m_nDay);
	HANDLE hFile = CreateFile(szFilePath, GENERIC_ALL, FILE_SHARE_WRITE | FILE_SHARE_DELETE, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		return;
	}
	DWORD dwFileLength = GetFileSize(hFile, NULL);
	SetFilePointer(hFile, dwFileLength, NULL, FILE_BEGIN);

	DWORD dwWrite = 0;
	WriteFile(hFile, szOut, nLength, &dwFileLength, NULL);

	CloseHandle(hFile);
}

VOID	CLog::ErrorW(LPWSTR lpstrFormat, ...)
{
	TCHAR szBuf[40960] = { 0 };

	va_list lpArgs;
	va_start(lpArgs, lpstrFormat);
	StringCbVPrintfW(szBuf, 40960, lpstrFormat, lpArgs);
	va_end(lpArgs);

	USES_CONVERSION;

	ATL::CTime CurTime = ATL::CTime::GetTickCount();
	char szOut[40960] = { 0 };
	int nLength = wsprintfA(szOut, "%d-%d-%d %d:%d:%d - [Error] : %s\r\n", CurTime.GetYear(),
		CurTime.GetMonth(),
		CurTime.GetDay(),
		CurTime.GetHour(),
		CurTime.GetMinute(),
		CurTime.GetSecond(),
		T2A(szBuf));

	UpdateFileTime();

	TCHAR szFilePath[MAX_PATH] = { 0 };
	wsprintf(szFilePath, _T("%s\\%d%d%d.log"), m_szDirectory, m_nYear, m_nMonth, m_nDay);
	HANDLE hFile = CreateFile(szFilePath, GENERIC_ALL, FILE_SHARE_WRITE | FILE_SHARE_DELETE, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		return;
	}
	DWORD dwFileLength = GetFileSize(hFile, NULL);
	SetFilePointer(hFile, dwFileLength, NULL, FILE_BEGIN);

	DWORD dwWrite = 0;
	WriteFile(hFile, szOut, nLength, &dwFileLength, NULL);

	CloseHandle(hFile);
}

VOID	CLog::Write(byte* lpByte, int nLength)
{
	ATL::CTime CurTime = ATL::CTime::GetTickCount();
	char szOut[40960] = { 0 };
	int nSize = wsprintfA(szOut, "%d-%d-%d %d:%d:%d - [Write] : \r\n", CurTime.GetYear(),
		CurTime.GetMonth(),
		CurTime.GetDay(),
		CurTime.GetHour(),
		CurTime.GetMinute(),
		CurTime.GetSecond());

	UpdateFileTime();

	TCHAR szFilePath[MAX_PATH] = { 0 };
	wsprintf(szFilePath, _T("%s\\%d%d%d.log"), m_szDirectory, m_nYear, m_nMonth, m_nDay);
	HANDLE hFile = CreateFile(szFilePath, GENERIC_ALL, FILE_SHARE_WRITE | FILE_SHARE_DELETE, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		return;
	}
	DWORD dwFileLength = GetFileSize(hFile, NULL);
	SetFilePointer(hFile, dwFileLength, NULL, FILE_BEGIN);

	DWORD dwWrite = 0;
	WriteFile(hFile, szOut, nSize, &dwFileLength, NULL);

	WriteFile(hFile, lpByte, nLength, &dwFileLength, NULL);

	int nSizeLength = strlen("\r\n");
	WriteFile(hFile, "\r\n", nSizeLength, &dwFileLength, NULL);

	CloseHandle(hFile);
}

VOID	CLog::Hex(byte* lpHex, int nLength)
{
	int nSize = (nLength * 3) + 1;

	char* lpData = (char*)GlobalAlloc(GPTR, nSize);

	for (int i = 0; i < nLength; i++)
	{
		char szBuf[6] = { 0 };
		wsprintfA(szBuf, "%02X ", lpHex[i]);
		lstrcatA(lpData, szBuf);
	}

	DebugA("%s", lpData);

	GlobalFree(lpData);
}