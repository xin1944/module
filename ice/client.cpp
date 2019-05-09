#include <Ice/Ice.h>
#include <Printer.h>

using namespace std;
using namespace demo;


int main(int argc, char* argv[])
{
	int status = 0;
	Ice::CommunicatorPtr ic;
	try
	{
		ic = Ice::initialize(argc, argv);
		Ice::ObjectPrx base = ic->stringToProxy("CCSimplePrinter:default -p 19993");
		PrinterPrx printer = PrinterPrx::checkedCast(base);
		if(!printer)
			throw "invalid proxy";
		printer->printString("hello world!");

	}
	catch(const Ice::Exception&e)
	{
		cerr<<e<<endl;
		status = 1;
	}
	catch (const char* msg)
	{
		cerr<<msg<<endl;
		status = 1;
	}
	if (ic) ic->destroy();
	return status;
}

