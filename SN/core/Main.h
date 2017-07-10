#pragma once
#include "define.h"

namespace SN
{
	class SNAPI CMain
	{
	public:
		CMain();
		~CMain();
		static	CMain* getInstance();
	public:
		int		appMain(HINSTANCE hInhstance, LPTSTR lpCmdLine, int nShow);
		int		dllMain(HMODULE	hModule);
		int		releaseResources(tstring strPath, tstring strType, WORD wResID);
		int		initDUMP(LPTOP_LEVEL_EXCEPTION_FILTER lpTopLevelExceptionFilter = NULL);
		int		getCurrentPath(tstring& strPath);

		HANDLE	getHandle();
	protected:
		int		initSocket();
		int		initCOM();
	public:
		HINSTANCE	m_hInhstance;
		int			m_nShow;
		HMODULE		m_hModule;
	private:
		bool		m_bSocket;
		bool		m_bCOM;
		byte		m_bType;
	};
}