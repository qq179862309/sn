#pragma once

#ifdef DEBUG
#define		SN_DEBUG_VERTION	"_D"
#else
#define		SN_DEBUG_VERTION	
#endif // DEBUG

#ifdef _WIN64
#define SN_LIB_VERTION		"x64"
#else
#define SN_LIB_VERTION		"x86"
#endif // _WIN64


#ifdef SN_STATIC
#define SNAPI
#pragma comment(lib,"SN" SN_DEBUG_VERTION SN_LIB_VERTION ".lib")
#else
#ifdef SN_EXPORTS
#define SNAPI _declspec(dllexport)
#else
#define SNAPI _declspec(dllimport)
#pragma comment(lib,"SN" SN_DEBUG_VERTION SN_LIB_VERTION ".lib")
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
#include "log/Log.h"
#include "core\Ini.h"
#include "core\Character.h"

using namespace SN;