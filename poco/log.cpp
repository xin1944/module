#include <stdio.h>
//#include "sddl_log.h"
#include <iostream>
#include <string>
#include "Poco/FileChannel.h"
#include "Poco/ConsoleChannel.h"
#include "Poco/FormattingChannel.h"
#include "Poco/PatternFormatter.h"
#include "Poco/Logger.h"
#include "Poco/AutoPtr.h"

using std::string;
#define SDDLLOG "sddllog"
static bool init_flag = false;
void sddl_log_init()
{
#ifdef SDDLLOG
	if(init_flag)
		return;
	std::cout<<"log init at logs/isolation.log\n";
	Poco::AutoPtr<Poco::FileChannel> pChannel(new Poco::FileChannel);
	pChannel->setProperty("path", "logs/isolation.log");
	//char clogsize[20] = {0};
	//snprintf(clogsize, sizeof(clogsize), "%d M", CEnvironment::get_env()->get_log_filesize());
	pChannel->setProperty("rotation", "1K");
	//pChannel->setProperty("rotation", "monthly");
	//pChannel->setProperty("archive", "number");
	pChannel->setProperty("archive", "timestamp");
	//pChannel->setProperty("purgeCount", "10");
	pChannel->setProperty("purgeAge", "1 days");
	Poco::AutoPtr<Poco::PatternFormatter> pPF(new Poco::PatternFormatter);
	pPF->setProperty("pattern", "%Y-%m-%d %H:%M:%S %i %s %p [%U-%u]: %t");
	Poco::AutoPtr<Poco::FormattingChannel> pFC(new Poco::FormattingChannel(pPF, pChannel));
	Poco::Logger::root().setChannel(pFC);
	Poco::Logger& logger = Poco::Logger::get(SDDLLOG);
	//logger.setLevel(CEnvironment::get_env()->get_log_level());//7 PRIO_DEBUG
	init_flag = true;
#endif
}
void log_fatal(const std::string& msg, const char* file, const int line)
{
	sddl_log_init();
	Poco::Logger * pLog = Poco::Logger::has(SDDLLOG);
	if(pLog && pLog->fatal())
		pLog->fatal(msg, file, line);
}
int main()
{
	for(int i = 0;i<100;i++)
	{
		char cid[10] = {0};
		snprintf(cid, sizeof(cid), "count:%d", i);
		string a(cid);
		log_fatal(a + " my log", __FILE__, __LINE__);
	}
	return 0;
}