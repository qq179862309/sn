#pragma once

//类型定义
#ifdef _UNICODE
#define tstring		std::wstring
#else
#define tstring		std::string
#endif // _UNICODE



//宏定义
#define retIntError(code) if (code != 0){ return code; }