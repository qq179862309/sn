#include "stdafx.h"
#include "core\InjecLib.h"


CInjecLib::CInjecLib()
{
}


CInjecLib::~CInjecLib()
{
}

int		CInjecLib::EnableDebugPrivilege()
{
	HANDLE TokenHandle = NULL;
	TOKEN_PRIVILEGES TokenPrivilege;
	LUID uID;
	//打开权限令牌
	if (!OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &TokenHandle))
	{
		return 1;
	}
	if (!LookupPrivilegeValue(NULL, SE_DEBUG_NAME, &uID))
	{
		CloseHandle(TokenHandle);
		TokenHandle = INVALID_HANDLE_VALUE;
		return 2;
	}
	TokenPrivilege.PrivilegeCount = 1;
	TokenPrivilege.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
	TokenPrivilege.Privileges[0].Luid = uID;
	if (!AdjustTokenPrivileges(TokenHandle, FALSE, &TokenPrivilege, sizeof(TOKEN_PRIVILEGES), NULL, NULL))
		//调整权限
	{
		CloseHandle(TokenHandle);
		TokenHandle = INVALID_HANDLE_VALUE;
		return  3;
	}
	CloseHandle(TokenHandle);
	TokenHandle = INVALID_HANDLE_VALUE;
	return 0;
}

int		CInjecLib::injecLib(HANDLE hProcess, tstring strLib)
{
	WCHAR* VirtualAddress = NULL;
	ULONG32 ulDllLength = (ULONG32)strLib.length() + 1;
	VirtualAddress = (WCHAR*)VirtualAllocEx(hProcess, NULL, ulDllLength * sizeof(WCHAR), MEM_COMMIT, PAGE_READWRITE);
	if (NULL == VirtualAddress)
	{
		return 1;
	}
	if (FALSE == WriteProcessMemory(hProcess, VirtualAddress, (LPVOID)strLib.c_str(), ulDllLength * sizeof(WCHAR), NULL))
	{
		VirtualFreeEx(hProcess, VirtualAddress, ulDllLength, MEM_DECOMMIT);
		return 2;
	}
	LPTHREAD_START_ROUTINE FunctionAddress = NULL;
	FunctionAddress = (PTHREAD_START_ROUTINE)::GetProcAddress(::GetModuleHandle(_T("Kernel32")), "LoadLibraryW");
	HANDLE ThreadHandle = INVALID_HANDLE_VALUE;
	//start
	ThreadHandle = CreateRemoteThread(hProcess, NULL, 0, FunctionAddress, VirtualAddress, 0, NULL);
	if (NULL == ThreadHandle)
	{
		VirtualFreeEx(hProcess, VirtualAddress, ulDllLength, MEM_DECOMMIT);
		return 4;
	}
	WaitForSingleObject(ThreadHandle, INFINITE);
	VirtualFreeEx(hProcess, VirtualAddress, ulDllLength, MEM_DECOMMIT);			// 清理
	CloseHandle(ThreadHandle);
	return 0;
}

int		CInjecLib::injecLib(DWORD dwProcessId, tstring strLib)
{
	HANDLE  TargetProcessHandle = NULL;
	TargetProcessHandle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwProcessId);
	if (NULL == TargetProcessHandle)
	{
		return 1;
	}
	WCHAR* VirtualAddress = NULL;
	ULONG32 ulDllLength = (ULONG32)strLib.length() + 1;
	VirtualAddress = (WCHAR*)VirtualAllocEx(TargetProcessHandle, NULL, ulDllLength * sizeof(WCHAR), MEM_COMMIT, PAGE_READWRITE);
	if (NULL == VirtualAddress)
	{
		CloseHandle(TargetProcessHandle);
		return 2;
	}
	// write
	if (FALSE == WriteProcessMemory(TargetProcessHandle, VirtualAddress, (LPVOID)strLib.c_str(), ulDllLength * sizeof(WCHAR), NULL))
	{
		VirtualFreeEx(TargetProcessHandle, VirtualAddress, ulDllLength, MEM_DECOMMIT);
		CloseHandle(TargetProcessHandle);
		return 3;
	}
	LPTHREAD_START_ROUTINE FunctionAddress = NULL;
	FunctionAddress = (PTHREAD_START_ROUTINE)::GetProcAddress(::GetModuleHandle(_T("Kernel32")), "LoadLibraryW");
	HANDLE ThreadHandle = INVALID_HANDLE_VALUE;
	//start
	ThreadHandle = CreateRemoteThread(TargetProcessHandle, NULL, 0, FunctionAddress, VirtualAddress, 0, NULL);
	if (NULL == ThreadHandle)
	{
		VirtualFreeEx(TargetProcessHandle, VirtualAddress, ulDllLength, MEM_DECOMMIT);
		CloseHandle(TargetProcessHandle);
		return 4;
	}
	WaitForSingleObject(ThreadHandle, INFINITE);
	VirtualFreeEx(TargetProcessHandle, VirtualAddress, ulDllLength, MEM_DECOMMIT);			// 清理
	CloseHandle(ThreadHandle);
	CloseHandle(TargetProcessHandle);
	return 0;
}