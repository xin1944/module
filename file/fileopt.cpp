#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <list>
#include <vector>

using namespace std;

typedef struct _Channel_Data
{
	int chNo;
	string chName;
	string chUnit;
	double second;
	double rms;
}Channel_t;

typedef list<Channel_t> channelList;
typedef string::size_type (string::*find_t)(const string& delim,string::size_type offset) const;

vector<string> strsplit(const string&s, const string& match,bool removeEmpty=true,bool fullMatch=true)
{
	vector<string> result;
	string::size_type start = 0, skip=1;
	//find_t pfind = &string::find_first_of;
	if(fullMatch)
	{
		skip = match.length();
		//pfind == &string::find;
	}
	while(start!= string::npos)
	{
		string::size_type end = s.find(match,start);
		if(skip == 0) end = string::npos;
		string token = s.substr(start,end-start);
		if(!(removeEmpty && token.empty()))
			result.push_back(token);
		if((start = end)!=string::npos) start += skip;
	}
	return result;
}

int main()
{
	char channel[1024] = {0};
	FILE *fp;
	fp = fopen("roo.cfg", "r");
	if(fp == NULL)
	{
		cout<<"file open err\n";
		return 1;
	}
	fgets(channel, sizeof(channel), fp);
	int num = atoi(channel);
	channelList channels;
	Channel_t c;
	char* tmp = NULL;
	if(num > 0)
	//while(!feof(fp))
	for(int i=0;i<num;i++)
	{
		fgets(channel, sizeof(channel), fp);
		if(channel[strlen(channel)-1]=='\n')
		{
			if(channel[strlen(channel)-2]=='\r')
			{
				channel[strlen(channel)-2]='\0';
			}
			else
			{
				channel[strlen(channel)-1]='\0';
			}
		}
		cout<<channel<<endl;
	vector<string> result;
	result = strsplit(channel, ",");
	vector<string>::iterator iter = result.begin();
	if(iter != result.end())
		c.chNo = atoi((*iter).c_str());
	iter++;
	if(iter != result.end())
		c.chName = *iter;
	iter++;
	if(iter != result.end())
		c.chUnit = *iter;
	iter++;
	if(iter != result.end())
		c.second = atof((*iter).c_str());
	iter++;
	if(iter != result.end())
		c.rms = atof((*iter).c_str());

		cout<<c.chNo<<c.chName<<c.chUnit<<c.second<<c.rms<<endl;
	}
	else
		cout<<"channel line error:"<<channel;
	fclose(fp);
	return 0;
}

