#pragma once

//���Ͷ���
#ifdef _UNICODE
#define tstring		std::wstring
#else
#define tstring		std::string
#endif // _UNICODE



//�궨��
#define retIntError(code) if (code != 0){ return code; }