#include <math.h>
#include "my_string_opt.h"
#include <algorithm>
#ifdef _WIN32
#	ifndef WIN32_LEAN_AND_MEAN
#		define WIN32_LEAN_AND_MEAN
#	endif
#	include <windows.h>
#else
#	include <iconv.h>
#   include <string.h>
#   include <stdlib.h>
#   include <stdio.h>
#endif

// #ifndef DATASYNC_LOCAL_GB2312
// #define DATASYNC_LOCAL_GB2312
// #endif

//在Linux下由于默认字符编码是UTF8，所以无需转换

/*string GBKToUTF8(const string& strGBK)
{
#ifdef _WIN32
	string strOutUTF8 = "";
	WCHAR * str1;
	int n = MultiByteToWideChar(CP_ACP, 0, strGBK.c_str(), -1, NULL, 0);
	str1 = new WCHAR[n];
	MultiByteToWideChar(CP_ACP, 0, strGBK.c_str(), -1, str1, n);
	n = WideCharToMultiByte(CP_UTF8, 0, str1, -1, NULL, 0, NULL, NULL);
	char * str2 = new char[n];
	WideCharToMultiByte(CP_UTF8, 0, str1, -1, str2, n, NULL, NULL);
	strOutUTF8 = str2;
	delete[]str1;
	str1 = NULL;
	delete[]str2;
	str2 = NULL;
	return strOutUTF8;
#else
#   ifdef DATASYNC_LOCAL_GB2312
        string::size_type lenGBK = strGBK.size();
        string::size_type lenUTF8 = lenGBK*2;
        char* bufGBK = new char[lenGBK];
        if(bufGBK == NULL)
            return "";
        char* bufUTF8 = new char[lenUTF8];
        if(bufUTF8 == NULL)
        {
            delete[] bufGBK;
            return "";
        }
        memset(bufGBK, 0, lenGBK);
        memset(bufUTF8, 0, lenUTF8);
        char* pointer1 = bufGBK;
        char* pointer2 = bufUTF8;
        char** pin = &pointer1;
        char** pout = &pointer2;
        iconv_t cd;

        strncpy(bufGBK, strGBK.c_str(), lenGBK);
        size_t ret = 0;
        cd = iconv_open("utf-8", "gb2312");
        if(cd == 0)
            goto end;
        //iconv()每转换一个字符＊pin的值就加1，所以pointer1的值后来会改变
        ret = iconv(cd, pin, &lenGBK, pout, &lenUTF8);
        if(ret == (size_t)-1)
        {
            iconv_close(cd);
            goto end;
        }
        iconv_close(cd);

    end:
        string strUTF8 = bufUTF8;
        delete[] bufGBK;
        delete[] bufUTF8;
        return strUTF8;

#   else
        return strGBK;
#   endif
#endif
}

string UTF8ToGBK(const string& strUTF8)
{
#ifdef _WIN32
	int len = MultiByteToWideChar(CP_UTF8, 0, strUTF8.c_str(), -1, NULL, 0);
	uint16_t * wszGBK = new uint16_t[len + 1];
	memset(wszGBK, 0, len * 2 + 2);
	MultiByteToWideChar(CP_UTF8, 0, (LPCSTR)strUTF8.c_str(), -1, (LPWSTR)wszGBK, len);

	len = WideCharToMultiByte(CP_ACP, 0, (LPWSTR)wszGBK, -1, NULL, 0, NULL, NULL);
	char *szGBK = new char[len + 1];
	memset(szGBK, 0, len + 1);
	WideCharToMultiByte(CP_ACP,0, (LPWSTR)wszGBK, -1, szGBK, len, NULL, NULL);
	std::string strTemp(szGBK);
	delete[]szGBK;
	delete[]wszGBK;
	return strTemp;
#else
#   ifndef DATASYNC_LOCAL_GB2312
        return strUTF8;
#   else
        string::size_type lenUTF8 = strUTF8.size();
        string::size_type lenGBK = lenUTF8*2;
        char* bufGBK = new char[lenGBK];
        if(bufGBK == NULL)
            return "";
        char* bufUTF8 = new char[lenUTF8];
        if(bufUTF8 == NULL)
        {
            delete[] bufGBK;
            return "";
        }
        memset(bufGBK, 0, lenGBK);
        memset(bufUTF8, 0, lenUTF8);
        char* pointer1 = bufUTF8;
        char* pointer2 = bufGBK;
        char** pin = &pointer1;
        char** pout = &pointer2;
        iconv_t cd;

        strncpy(bufUTF8, strUTF8.c_str(), lenUTF8);
        cd = iconv_open("gb2312", "utf-8");
        if(cd == 0)
            goto end;
        if(iconv(cd, pin, &lenUTF8, pout, &lenGBK) == (size_t)-1)
            goto end;
        iconv_close(cd);

    end:
        string strGBK = bufGBK;
        delete[] bufGBK;
        delete[] bufUTF8;
        return strGBK;
#   endif
#endif
}*/

string g2u(const string& strGBK)
{
#ifdef _WIN32
	string strOutUTF8 = "";
	WCHAR * str1;
	int32_t n = MultiByteToWideChar(CP_ACP, 0, strGBK.c_str(), -1, NULL, 0);
	str1 = new WCHAR[n + 1];
	MultiByteToWideChar(CP_ACP, 0, strGBK.c_str(), -1, str1, n);
	n = WideCharToMultiByte(CP_UTF8, 0, str1, -1, NULL, 0, NULL, NULL);
	char * str2 = new char[n + 1];
	WideCharToMultiByte(CP_UTF8, 0, str1, -1, str2, n, NULL, NULL);
	strOutUTF8 = str2;
	delete[]str1;
	str1 = NULL;
	delete[]str2;
	str2 = NULL;
	return strOutUTF8;
#else
	string::size_type lenGBK = strGBK.size();
	string::size_type lenUTF8 = lenGBK * 3;
	if (lenGBK <= 0)
		return "";
	char* bufGBK = new char[lenGBK];
	if (bufGBK == NULL)
		return "";
	char* bufUTF8 = new char[lenUTF8];
	if (bufUTF8 == NULL)
	{
		delete[] bufGBK;
		return "";
	}
	memset(bufGBK, 0, lenGBK);
	memset(bufUTF8, 0, lenUTF8);
	char* pointer1 = bufGBK;
	char* pointer2 = bufUTF8;
	char** pin = &pointer1;
	char** pout = &pointer2;
	iconv_t cd;

	strncpy(bufGBK, strGBK.c_str(), lenGBK);
	size_t ret = 0;
	cd = iconv_open("utf-8//IGNORE", "gb18030");
	if (cd == 0)
		goto end;
	//iconv()每转换一个字符＊pin的值就加1，所以pointer1的值后来会改变
	ret = iconv(cd, pin, &lenGBK, pout, &lenUTF8);
	if (ret == (size_t)-1)
	{
		iconv_close(cd);
		goto end;
	}
	iconv_close(cd);

end:
	string strUTF8 = bufUTF8;
	delete[] bufGBK;
	delete[] bufUTF8;
	return strUTF8;
#endif
}

string u2g(const string& strUTF8)
{
#ifdef _WIN32
	int32_t len = MultiByteToWideChar(CP_UTF8, 0, strUTF8.c_str(), -1, NULL, 0);
	uint16_t * wszGBK = new uint16_t[len * 2 + 2];
	memset(wszGBK, 0, len * 2 + 2);
	MultiByteToWideChar(CP_UTF8, 0, (LPCSTR)strUTF8.c_str(), -1, (LPWSTR)wszGBK, len);

	len = WideCharToMultiByte(CP_ACP, 0, (LPWSTR)wszGBK, -1, NULL, 0, NULL, NULL);
	char *szGBK = new char[len + 1];
	memset(szGBK, 0, len + 1);
	WideCharToMultiByte(CP_ACP, 0, (LPWSTR)wszGBK, -1, szGBK, len, NULL, NULL);
	std::string strTemp(szGBK);
	delete[]szGBK;
	delete[]wszGBK;
	return strTemp;
#else
	string::size_type lenUTF8 = strUTF8.size();
	string::size_type lenGBK = lenUTF8 * 2;
	if (lenUTF8 <= 0)
		return "";
	char* bufGBK = new char[lenGBK];
	if (bufGBK == NULL)
		return "";
	char* bufUTF8 = new char[lenUTF8];
	if (bufUTF8 == NULL)
	{
		delete[] bufGBK;
		return "";
	}
	memset(bufGBK, 0, lenGBK);
	memset(bufUTF8, 0, lenUTF8);
	char* pointer1 = bufUTF8;
	char* pointer2 = bufGBK;
	char** pin = &pointer1;
	char** pout = &pointer2;
	iconv_t cd;

	strncpy(bufUTF8, strUTF8.c_str(), lenUTF8);
	cd = iconv_open("gb18030//IGNORE", "utf-8");
	if (cd == 0)
		goto end;
	if (iconv(cd, pin, &lenUTF8, pout, &lenGBK) == (size_t)-1)
		goto end;
	iconv_close(cd);

end:
	string strGBK = bufGBK;
	delete[] bufGBK;
	delete[] bufUTF8;
	return strGBK;
#endif
}
//忽略大小写比较字符串
int string_case_compare(const string& str1, const string& str2)
{
#ifdef _WIN32
	return _stricmp(str1.c_str(), str2.c_str());
#else
	return strcasecmp(str1.c_str(), str2.c_str());
#endif
}

string string_toupper(const string& str)
{
	string temp_str = str;
	std::transform(temp_str.begin(), temp_str.end(), temp_str.begin(), (int(*)(int))toupper);
	return temp_str;
}
string RightString(string& str, int nCount)
{
	int nLength = str.length();
	if(nLength < nCount)
		nCount = nLength;
	return str.substr(nLength-nCount, nCount);
}

string LeftString(string& str, int nCount)
{
	int nLength = str.length();
	if(nLength < nCount)
		nCount = nLength;
	return str.substr(0, nCount);
}

uint32_t StringToULong(char* str, int length/*=0*/)
{
	if(length <= 0)
		return atoi(str);

	char* buf = new char[length+1];
	memcpy(buf, str, length);
	buf[length] = '\0';
	uint32_t ret = atoi(buf);
	delete []buf;
	return ret;
}


// uint32_t StringToULong(char* string, int length/*=0*/)
// {
// 	if(length == 0)
// 		length = strlen(string);
// 
// 	uint32_t dwRet = 0;
// 	char* p = string;
// 	int num = 0;
// 	double fPow = 0;
// 	char c;
// 	while(length--)
// 	{
// 		c = *(p++);
// 		num = atoi(&c);
// 		fPow = pow((double)10, length);
// 		dwRet += (uint32_t) (num*fPow);
// 	}
// 
// 	return dwRet;
// }

uint32_t Hex2Decimal(const char* pHex, int length)
{
	uint32_t dwDec = 0;
	int number = 0;
	for(int i=length; i>0; --i)
	{
		switch(pHex[i-1])
		{
		case 'x': case 'X':
		case '0': number = 0; break;
		case '1': number = 1; break;
		case '2': number = 2; break;
		case '3': number = 3; break;
		case '4': number = 4; break;
		case '5': number = 5; break;
		case '6': number = 6; break;
		case '7': number = 7; break;
		case '8': number = 8; break;
		case '9': number = 9; break;
		case 'a': case 'A': number = 10; break;
		case 'b': case 'B': number = 11; break;
		case 'c': case 'C': number = 12; break;
		case 'd': case 'D': number = 13; break;
		case 'e': case 'E': number = 14; break;
		case 'f': case 'F': number = 15; break;
		}

		dwDec += (uint32_t)( number*pow((double)16, (double)length-i) );
	}

	return dwDec;
}

/*
功能
删除前后的空格（' ','\t','\r','\n'）
参数
szString		in	传入的字符串
out 去除空格后的字符串
返回值
1	调用成功
*/
int trim(char* &szString)
{
	char* p = szString;
	while (*p == ' ' || *p == '\t')
	{
		p ++;
	}
	szString = p;

	p = szString + strlen(szString) - 1;
	while ( *p == ' ' || *p == '\t' || *p == '\r' || *p == '\n')
	{
		-- p;
	}
	*( p + 1 ) = 0;

	return 1;
}

int Char2Int(char c)
{
	int n=0;
	int num = (int)c;
	if(num>=48 && num<=57)
		n = num-48;
	else if(num>=65 && num<=90)
		n = num-55;
	return n;
}

char Int2Char(int n)
{
	char c = 48;
	if(n<10)
		c = n+48;
	else
		c = n+55;
	return c;
}

//////////////////////////////////////////////////////////////////////////
//MakeBytes() num - 两位整数
void MakeByte(int num, char& c)
{
	c=0;
	c |= (num%10);
	c |=((num/10)<<4);
}

typedef string::size_type (string::*find_t)(const string& delim, string::size_type offset) const;

/// <summary>
/// Splits the string s on the given delimiter(s) and
/// returns a list of tokens without the delimiter(s)
/// </summary>
/// <param name=s>The string being split</param>
/// <param name=match>The delimiter(s) for splitting</param>
/// <param name=removeEmpty>Removes empty tokens from the list</param>
/// <param name=fullMatch>
/// True if the whole match string is a match, false
/// if any character in the match string is a match
/// </param>
/// <returns>A list of tokens</returns>

vector<string> Split(const string& s, const string& match, bool removeEmpty/*=true*/, bool fullMatch/*=true*/)
{
	vector<string> result;                 // return container for tokens
	string::size_type start = 0,           // starting position for searches
		skip = 1;            // positions to skip after a match
	find_t pfind = &string::find_first_of; // search algorithm for matches

	if (fullMatch)
	{
		// use the whole match string as a key
		// instead of individual characters
		// skip might be 0. see search loop comments
		skip = match.length();
		pfind = &string::find;
	}

	while (start != string::npos)
	{
		// get a complete range [start..end)
		string::size_type end = (s.*pfind)(match, start);

		// null strings always match in string::find, but
		// a skip of 0 causes infinite loops. pretend that
		// no tokens were found and extract the whole string
		if (skip == 0) end = string::npos;

		string token = s.substr(start, end - start);

		if (!(removeEmpty && token.empty()))
		{
			// extract the token and add it to the result list
			result.push_back(token);
		}

		// start the next range
		if ((start = end) != string::npos) start += skip;
	}

	return result;
}

string int64_to_string(int64_t int_num)
{
	char buf[50];
#ifdef _WIN32
	sprintf(buf, "%I64d", int_num);
#else
	sprintf(buf, "%ld", int_num);
#endif
	return buf;
}

string uint64_to_string(uint64_t uint_num)
{
	char buf[50];
#ifdef _WIN32
	sprintf(buf, "%I64u", uint_num);
#else
	sprintf(buf, "%lu", uint_num);
#endif
	return buf;
}

int64_t string_to_int64(char* str)
{
#ifdef _WIN32
	return _atoi64(str);
#else
	return atoll(str);
#endif
}

//返回从左边开始相同的字符数
int same_characters(string left, string right)
{
	int leftLen = left.length();
	int rightLen = right.length();
	int length = (leftLen<rightLen)? leftLen:rightLen;
	int i = 0;
	for(; i<length; ++i)
	{
		if(left[i] != right[i])
			break;
	}
	return i;
}

void replace_string(string& srcStr, const string& findStr, const string& replaceStr)
{
	string::size_type pos = 0;
	while( (pos = srcStr.find(findStr, pos)) != string::npos ) 
	{
		srcStr.replace( pos, findStr.size(), replaceStr );
		pos += replaceStr.size();
	//	pos++;
	}
}

#ifndef _WIN32

unsigned char LOBYTE(uint16_t word)
{
	return (word & 0xFF);
}

unsigned char HIBYTE(uint16_t word)
{
	return ((word>>8) & 0xFF);
}

uint16_t LOWORD(uint32_t dword)
{
	return (dword & 0xFFFF);
}

uint16_t HIWORD(uint32_t dword)
{
	return ((dword>>16) & 0xFFFF);
}

#endif //_WIN32
