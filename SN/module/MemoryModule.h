#pragma once

namespace SN
{
	typedef BOOL(WINAPI *DllEntryProc)(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpReserved);
	typedef int (WINAPI *ExeEntryProc)(void);

	typedef void *HMEMORYRSRC;

	typedef void *HCUSTOMMODULE;

	typedef HCUSTOMMODULE(*CustomLoadLibraryFunc)(LPCSTR, void *);
	typedef FARPROC(*CustomGetProcAddressFunc)(HCUSTOMMODULE, LPCSTR, void *);
	typedef void(*CustomFreeLibraryFunc)(HCUSTOMMODULE, void *);

	typedef struct {
		PIMAGE_NT_HEADERS headers;
		unsigned char *codeBase;
		HCUSTOMMODULE *modules;
		int numModules;
		int initialized;
		int isDLL;
		int isRelocated;
		CustomLoadLibraryFunc loadLibrary;
		CustomGetProcAddressFunc getProcAddress;
		CustomFreeLibraryFunc freeLibrary;
		void *userdata;
		ExeEntryProc exeEntry;
	} MEMORYMODULE, *PMEMORYMODULE;

	typedef MEMORYMODULE *HMEMORYMODULE;

	class SNAPI CMemoryModule
	{
	public:
		CMemoryModule();
		~CMemoryModule();
		HMEMORYMODULE MemoryLoadLibrary(const void *);
		HMEMORYMODULE MemoryLoadLibraryEx(const void *, CustomLoadLibraryFunc, CustomGetProcAddressFunc, CustomFreeLibraryFunc, void *);
		FARPROC MemoryGetProcAddress(HMEMORYMODULE, LPCSTR);
		void MemoryFreeLibrary(HMEMORYMODULE);
		int MemoryCallEntryPoint(HMEMORYMODULE);
		HMEMORYRSRC MemoryFindResource(HMEMORYMODULE, LPCTSTR, LPCTSTR);
		HMEMORYRSRC MemoryFindResourceEx(HMEMORYMODULE, LPCTSTR, LPCTSTR, WORD);
		DWORD MemorySizeofResource(HMEMORYMODULE, HMEMORYRSRC);
		LPVOID MemoryLoadResource(HMEMORYMODULE, HMEMORYRSRC);
		int MemoryLoadString(HMEMORYMODULE, UINT, LPTSTR, int);
		int MemoryLoadStringEx(HMEMORYMODULE, UINT, LPTSTR, int, WORD);
	private:
		void CopySections(const unsigned char *data, PIMAGE_NT_HEADERS old_headers, PMEMORYMODULE module);
		void FinalizeSections(PMEMORYMODULE module);
		void ExecuteTLS(PMEMORYMODULE module);
		int PerformBaseRelocation(PMEMORYMODULE module, SIZE_T delta);
		int BuildImportTable(PMEMORYMODULE module);
	};
}