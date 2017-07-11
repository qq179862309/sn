#pragma once

#ifdef DEBUG
#define		SN_DEBUG_VERTION	"_D"
#else
#define		SN_DEBUG_VERTION	
#endif // DEBUG

#ifdef SN_STATIC
#define SNAPI
#else
#ifdef SN_EXPORTS
#define SNAPI _declspec(dllexport)
#else
#define SNAPI _declspec(dllimport)
#pragma comment(lib,"SN" SN_DEBUG_VERTION ".lib")
#pragma message("SN" SN_DEBUG_VERTION ".lib")
#endif
#endif


#include <string>
#include <atlstr.h>
#include <atltime.h>

#include <dbghelp.h>
#include <WinSock2.h>


#include "core/define.h"
#include "core/Main.h"
#include "module/MemoryModule.h"
#include "hook/Hook.h"

using namespace SN;