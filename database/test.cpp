#include <iostream>
#include <string>
#include <stdio.h>
#include <unistd.h>
#include "ice_database.h"
#include "Printer.h"
#include "Ice/Ice.h"

using namespace std;
using namespace demo;

int main(int argc, char* argv[])
{
	cout<<"app start!\n";
	Ice::PropertiesPtr props = Ice::createProperties(argc,argv);
	props->setProperty("Ice::Override.ConnectTimeout", "10000");
	props->setProperty("Ice::Override.Timeout", "30000");
	props->setProperty("Ice::RetryIntervals", "-1");
	props->setProperty("Ice::Default.EncodingVersion", "1.0");
	Ice::InitializationData id;
	id.properties = props;
	//Ice::CommunicatorPtr ic = Ice::initialize(argc, argv);
	sleep(5);
#if 0
	try
	{
		Ice::ObjectPrx base = ic->stringToProxy("SimplePrinter:default -p 19993");
		PrinterPrx printer = PrinterPrx::checkedCast(base);
		if(!printer)
			throw "invalid proxy";
		printer->printString("hello world!");
	}
	catch(const Ice::Exception&e)
	{
		cerr<<e<<endl;
	}
#else
	string sql = "select * from faultinfo.faultinfo.t_mainstation";
	Json::Value jroot;
	if(execQuery(sql.c_str(), jroot))
	{
		Json::UInt num = jroot.size();
		double nid = 0;
		string ip = "";
		int ddid = 0;
		for(Json::UInt i =0;i<num;i++)
		{
			nid = jroot[i]["ID"].isNull()?0.0 : jroot[i]["ID"].asDouble();
			ddid = jroot[i]["N_DD_MAINSTATION_ID"].isNull()? 0 : jroot[i]["N_DD_MAINSTATION_ID"].asInt();
		       	ip = jroot[i]["C_TASK_SEND_DIR"].isNull()?"null" : jroot[i]["C_TASK_SEND_DIR"].asString();
			cout<<nid<<" | "<<ip<<" | "<<ddid<<endl;
		}
	}
	else
		cout<<"exec sql err\n";
#endif
	return 0;
}

