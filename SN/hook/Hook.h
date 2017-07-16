#pragma once

#include <map>

namespace SN
{
	typedef struct _tagHOOKINFO
	{
		DWORD	dwFuncAddress;
		byte*	pNewCode;
		int		nNewLength;
		byte*	pOldCode;
	}HOOKINFO, *PHOOKINFO;

	typedef std::map<tstring, PHOOKINFO>	MAP_HOOKINFO;
	typedef MAP_HOOKINFO::iterator			ITER_HOOKINFO;

	typedef std::map<HWND, DWORD>			MAP_HWNDPROC;
	typedef MAP_HWNDPROC::iterator			ITER_HWNDPROC;

	class SNAPI CHook
	{
	public:
		CHook();
		virtual ~CHook();
	public:
		static	CHook*	getInstance();
		static  int		hookWindowAPI(tstring strModule, char* funcName, DWORD dwFunction);
	public:
		int				hookFunction(tstring strKey, DWORD dwFuncAddress, byte* lpNewCode, int nNewLength);
		int				unHookFunction(tstring strKey);
		int				hookWindowProc(HWND hWnd, DWORD dwNewProc, DWORD& dwOldProc);
		int				unHookWindowProc(HWND hWnd);
		int				windowProcCall(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, int& nResult);
	protected:
		MAP_HOOKINFO	m_MapInfo;
		MAP_HWNDPROC	m_MapHwndProc;
	};
}

