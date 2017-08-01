#pragma once

namespace SN
{
	class CInjecLib
	{
	public:
		CInjecLib();
		~CInjecLib();
		static		int		EnableDebugPrivilege();

		static		int		injecLib(HANDLE hProcess, tstring strLib);
		static		int		injecLib(DWORD dwProcessId, tstring strLib);
	};

}
