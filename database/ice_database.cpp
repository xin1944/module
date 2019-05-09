#ifdef _WIN32
#	pragma warning(disable:4244 4018 4251 4275)
#	include <winsock2.h>
#endif

#include <sstream>
#include "ice_database.h"
#include "databaseCommon.h"
#include "Ice/Ice.h"
//#include "environment.h"
#include "my_string_opt.h"

using std::ostringstream;
using namespace sduept::database::common;

bool isDbConnect()
{
	Json::Value jvalue;
	bool ret = execQuery("select \'test\'", jvalue);
	return ret;
}

///////////////////////////////////////////////////////////////
//execQuery()
//chsql - [in] sql string with gbk encoding
//jvalue - [out] json value with utf8 encoding
bool execQuery(const char* chsql, Json::Value& jvalue)
{
	string strUri = "CommonJsonSql:default -h ";
	strUri += "192.168.220.1";
	//strUri += "127.0.0.1";
	strUri += " -p 10060";

	try
	{
		std::cout<<"get prx:"<<strUri<<std::endl;
		Ice::ObjectPrx objPrx = Ice::Application::communicator()->stringToProxy(strUri);
		JsonSqlPrx jsqlPrx = JsonSqlPrx::checkedCast(objPrx);
		if (jsqlPrx == NULL)
		{
			std::cout<<"DATABASE [QUERY FAIL]: Proxy not available\n";
			return false;
		}
		std::string utf8sql = g2u(std::string(chsql));
		std::string resultUTF8 = jsqlPrx->query(utf8sql);
		Json::Reader reader;
		bool ret = reader.parse(resultUTF8, jvalue);
		if (!ret)
		{
			std::string strEvent = "parse json error:";
			std::cout<<strEvent;
			return false;
		}
	}
	catch (DatabaseError& e)
	{
		printf("DatabaseError occured!\r\n");
		std::string strEvent = "DATABASE [QUERY FAIL]: \r\n";
		strEvent += "sql=";
		strEvent += chsql;
		strEvent += "\r\n";
		strEvent += u2g(e.reason);
		std::cout<<strEvent;
		return false;
	}
	catch (Ice::LocalException& e)
	{
		std::string strEvent = "DATABASE [QUERY FAIL]: ";
		strEvent += u2g(e.what());
		std::cout<<strEvent;
		return false;
	}
	catch (Ice::Exception& e)
	{
		std::string strEvent = "DATABASE [QUERY FAIL]: ";
		strEvent += u2g(e.what());
		std::cout<<strEvent;
		return false;
	}
	catch (...)
	{
		std::cout<<"DATABASE [QUERY FAIL]: unknown exception\n";
		return false;
	}
	return true;
}

////////////////////////////////////////////////////////////////////////////
//execNoQuery()
//chsql - [in] sql string with gbk encoding
bool execNoQuery(const char* chsql)
{
	string strUri = "CommonJsonSql:default -h ";
	//strUri += "192.168.220.1";
	strUri += "127.0.0.1";
	strUri += " -p 10060";
	try
	{
		Ice::ObjectPrx objPrx = Ice::Application::communicator()->stringToProxy(strUri);
		JsonSqlPrx jsqlPrx = JsonSqlPrx::checkedCast(objPrx);
		if (jsqlPrx == NULL)
		{
			std::cout<<"DATABASE [EXEC FAIL]: Proxy not available\n";
			return false;
		}
		std::string utf8sql = g2u(std::string(chsql));
		bool ret = jsqlPrx->execute(utf8sql);
		if (!ret)
		{
			std::string strEvent = "DATABASE [EXEC FAIL]: ";
			strEvent += chsql;
			std::cout<<strEvent;
		}
		return ret;
	}
	catch (DatabaseError& e)
	{
		std::string strEvent = "DATABASE [EXEC FAIL]: \r\n";
		strEvent += "sql=";
		strEvent += chsql;
		strEvent += "\r\n";
		strEvent += u2g(e.reason);
		std::cout<<strEvent;
		return false;
	}
	catch (Ice::LocalException& e)
	{
		std::string strEvent = "DATABASE [EXEC FAIL]: ";
		strEvent += u2g(e.what());
		std::cout<<strEvent;
		return false;
	}
	catch (Ice::Exception& e)
	{
		std::string strEvent = "DATABASE [EXEC FAIL]: ";
		strEvent += u2g(e.what());
		std::cout<<strEvent;
		return false;
	}
	catch (...)
	{
		std::cout<<"DATABASE [EXEC FAIL]: unknown exception\n";
		return false;
	}
	return true;
}
