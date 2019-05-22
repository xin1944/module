
#include <Ice/Ice.h>
#include "id_generator.h"
#include "IdGeneratorI.h"

using namespace std;

long IdGeneratorI::getNextId(const Ice::Current&)
{
	sduept::IdGenerator* idGen = sduept::IdGenerator::getIdGenerator();
	if (!idGen || !idGen->init("./idgenconfig.ini"))
		return 0;
	long nextid = idGen->nextId();
	return nextid;

}

record::recordIds IdGeneratorI::getNextIds(int idNum, const Ice::Current&)
{
	record::recordIds ids;
	sduept::IdGenerator* idGen = sduept::IdGenerator::getIdGenerator();
	if (!idGen || !idGen->init("./idgenconfig.ini"))
		return ids;
	long* pnextid = idGen->nextIds(idNum);
	if(pnextid == NULL)
		return ids;
	long *pid = pnextid;
	for(int i=0; i<idNum; i++)
	{
		ids.push_back(*pid);
		pid++;
	}
	delete pnextid;

	return ids;
}

class IdGeneratorApp:virtual public Ice::Application
{
public:
    virtual int run(int,char*[]);
};

int IdGeneratorApp::run(int,char*[])
{
	Ice::ObjectAdapterPtr adapter = communicator()->createObjectAdapterWithEndpoints("IdGeneratorPrx","default -p 17909");
	Ice::ObjectPtr object = new IdGeneratorI();
	adapter->add(object,communicator()->stringToIdentity("IdGeneratorServant"));
	adapter->activate();
	communicator()->waitForShutdown();

	if(interrupted())
		cerr << appName() << ":recv signal,shutting down" << endl;

	return 0;
}

int main(int argc,char* argv[])
{
	IdGeneratorApp app;
	return app.main(argc,argv);
}
