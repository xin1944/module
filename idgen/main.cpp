#include <Ice/Ice.h>
#include <idgenerator.h>

using namespace std;
using namespace record;

// 客户端只需要一个main函数，并且代码结构与服务端代码类似
int main(int argc, char* argv[])
{
    int status = 0;
    Ice::CommunicatorPtr ic;
    try {
		ic = Ice::initialize(argc, argv);

		Ice::ObjectPrx base = ic->stringToProxy("IdGeneratorServant:default -h 127.0.0.1 -p 17909");
		idGeneratorPrx idgenprx = idGeneratorPrx::checkedCast(base);
		if(!idgenprx)
			throw "Invalid proxy";
		// 调用idgenerator对象代理的getNextId方法。调用将会通过对象代理被发送到服务端
		long id = idgenprx->getNextId();
		cout<<id<<endl;
		record::recordIds ids = idgenprx->getNextIds(10);
		record::recordIds::iterator iter;
		for(iter = ids.begin();iter!=ids.end();iter++)
		{
			cout<<*iter<<endl;
		}
		
	} catch (const Ice::Exception&ex) {
		cerr << ex << endl;
		status = 1;
	} catch (const char* msg) {
		cerr << msg << endl;
		status = 1;
	}
	if (ic)
		ic->destroy();

	return status;
}

