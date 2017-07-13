#pragma once

class CLog
{
public:
	CLog(void);
	~CLog(void);

	static	CLog*			getInstance();

	int						setDirectory(LPTSTR lpDirectory = NULL);
	VOID					UpdateFileTime();

	VOID					DebugA(LPSTR lpstrFormat, ...);
	VOID					DebugW(LPWSTR lpstrFormat, ...);

	VOID					ErrorA(LPSTR lpstrFormat, ...);
	VOID					ErrorW(LPWSTR lpstrFormat, ...);

	VOID					Hex(byte* lpHex, int nLength);
	VOID					Write(byte* lpByte, int nLength);
public:
	TCHAR		m_szDirectory[MAX_PATH];
	int			m_nYear;
	int			m_nMonth;
	int			m_nDay;
};
