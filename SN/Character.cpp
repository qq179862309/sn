#include "stdafx.h"
#include "core\Character.h"


CCharacter::CCharacter()
{
}


CCharacter::~CCharacter()
{
}

std::string CCharacter::format(const char *fmt, ...)
{
	std::string strResult = "";
	if (NULL != fmt)
	{
		va_list marker = NULL;
		va_start(marker, fmt);                            //��ʼ���������� 
		size_t nLength = _vscprintf(fmt, marker) + 1;    //��ȡ��ʽ���ַ�������
		std::vector<char> vBuffer(nLength, '\0');        //�������ڴ洢��ʽ���ַ������ַ�����
		int nWritten = _vsnprintf_s(&vBuffer[0], vBuffer.size(), nLength, fmt, marker);
		if (nWritten > 0)
		{
			strResult = &vBuffer[0];
		}
		va_end(marker);                                    //���ñ�������
	}
	return strResult;
}

std::wstring CCharacter::format(const wchar_t *fmt, ...)
{
	std::wstring strResult = L"";
	if (NULL != fmt)
	{
		va_list marker = NULL;
		va_start(marker, fmt);                            //��ʼ����������
		size_t nLength = _vscwprintf(fmt, marker) + 1;    //��ȡ��ʽ���ַ�������
		std::vector<wchar_t> vBuffer(nLength, L'\0');    //�������ڴ洢��ʽ���ַ������ַ�����
		int nWritten = _vsnwprintf_s(&vBuffer[0], vBuffer.size(), nLength, fmt, marker);
		if (nWritten > 0)
		{
			strResult = &vBuffer[0];
		}
		va_end(marker);                                    //���ñ�������
	}
	return strResult;
}

vector<wstring> CCharacter::split(const wstring str, const wstring seperator)
{
	vector<wstring> result;
	using wstring_size = wstring::size_type;
	wstring_size i = 0;

	while (i != str.size()){
		//�ҵ��ַ������׸������ڷָ�������ĸ��
		int flag = 0;
		while (i != str.size() && flag == 0){
			flag = 1;
			for (wstring_size x = 0; x < seperator.size(); ++x)
				if (str[i] == seperator[x]){
					++i;
					flag = 0;
					break;
				}
		}

		//�ҵ���һ���ָ������������ָ���֮����ַ���ȡ����
		flag = 0;
		wstring_size j = i;
		while (j != str.size() && flag == 0){
			for (wstring_size x = 0; x < seperator.size(); ++x)
				if (str[j] == seperator[x]){
					flag = 1;
					break;
				}
			if (flag == 0)
				++j;
		}
		if (i != j){
			result.push_back(str.substr(i, j - i));
			i = j;
		}
	}
	return result;
}

vector<string> CCharacter::split(const string str, const string seperator)
{
	vector<string> result;
	using string_size = string::size_type;
	string_size i = 0;

	while (i != str.size()){
		//�ҵ��ַ������׸������ڷָ�������ĸ��
		int flag = 0;
		while (i != str.size() && flag == 0){
			flag = 1;
			for (string_size x = 0; x < seperator.size(); ++x)
				if (str[i] == seperator[x]){
					++i;
					flag = 0;
					break;
				}
		}

		//�ҵ���һ���ָ������������ָ���֮����ַ���ȡ����
		flag = 0;
		string_size j = i;
		while (j != str.size() && flag == 0){
			for (string_size x = 0; x < seperator.size(); ++x)
				if (str[j] == seperator[x]){
					flag = 1;
					break;
				}
			if (flag == 0)
				++j;
		}
		if (i != j){
			result.push_back(str.substr(i, j - i));
			i = j;
		}
	}
	return result;
}

int		CCharacter::unicodeToAscii(wstring strUnicode, string& strAscii)
{
	int nLength = WideCharToMultiByte(CP_ACP, 0, strUnicode.c_str(), strUnicode.length(), NULL, 0, NULL, NULL);
	char* pData = new char[nLength + 1];
	memset(pData, 0, nLength + 1);
	WideCharToMultiByte(CP_ACP, 0, strUnicode.c_str(), strUnicode.length(), pData, nLength, NULL, NULL);
	strAscii = pData;
	delete pData;
	return 0;
}

int		CCharacter::asciiToUnicode(string strAscii, wstring& strUnicode)
{
	int nLength = MultiByteToWideChar(CP_ACP, 0, strAscii.c_str(), strAscii.length(), NULL, 0);
	wchar_t* pData = new wchar_t[nLength + 1];
	memset(pData, 0, (nLength + 1) * sizeof(wchar_t));
	MultiByteToWideChar(CP_ACP, 0, strAscii.c_str(), strAscii.length(), pData, nLength);
	strUnicode = pData;
	delete pData;
	return 0;
}