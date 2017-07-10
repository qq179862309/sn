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
	protected:
		MAP_HOOKINFO	m_MapInfo;
	};
}

