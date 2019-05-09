#ifndef _MYSTRINGOPT_H_20091217
#define _MYSTRINGOPT_H_20091217

#include <string>
#include <vector>
#include "base_type.h"
using namespace std;

string g2u(const string& strGBK);
string u2g(const string& strUTF8);
int string_case_compare(const string& str1, const string& str2);
string string_toupper(const string& str);
string RightString(string& str, int nCount);
string LeftString(string& str, int nCount);
uint32_t StringToULong(char* str, int length=0);
uint32_t Hex2Decimal(const char* pHex, int length);
int trim(char* &szString);
int Char2Int(char c);
char Int2Char(int n);
void MakeByte(int num, char& c);
vector<string> Split(const string& s, const string& match, bool removeEmpty=true, bool fullMatch=true);
string int64_to_string(int64_t int_num);
string uint64_to_string(uint64_t uint_num);
int64_t string_to_int64(char* str);
int same_characters(string left, string right);

//2012_07_27
string AddSeparator(string& str);
void replace_string(string& srcStr, const string& findStr, const string& replaceStr);

#ifndef _WIN32

unsigned char LOBYTE(uint16_t word);
unsigned char HIBYTE(uint16_t word);
uint16_t LOWORD(uint32_t dword);
uint16_t HIWORD(uint32_t dword);

#endif	//_WIN32

#endif
