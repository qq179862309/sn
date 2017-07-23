#pragma once
#include <vector>

using namespace std;

namespace SN
{
	class SNAPI CCharacter
	{
	public:
		CCharacter();
		virtual ~CCharacter();
	public:
		static string format(const char *fmt, ...);
		static wstring format(const wchar_t *fmt, ...);

		static vector<string> split(const string str, const string seperator);
		static vector<wstring> split(const wstring str, const wstring seperator);

		static int		unicodeToAscii(wstring strUnicode, string& strAscii);
		static int		asciiToUnicode(string strAscii, wstring& strUnicode);
	};

}