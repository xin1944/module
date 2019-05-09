#include "Poco/Thread.h"
#include "Poco/Runnable.h"
#include <assert.h>
#include "Poco/Event.h"
#include "iostream"

using namespace Poco;
using namespace std;

class MyRunnable : public Runnable
{
	public:
	MyRunnable(): _ran(false){}
	void run()
	{
		Thread* pThread = Thread::current();
		if (pThread)
		_threadName = pThread->name();
		_ran = true;
		//_event.wait();
		while(1)
		{
		cout<<_threadName+ " child running\n";
		Thread::sleep(10*1000);
		}
	}
	bool ran() const
	{
	return _ran;
	}
	void notify()
	{
	_event.set();
	}
	const std::string& threadName() const
	{
	return _threadName;
	}

	private:
	std::string _threadName;
	bool _ran;
	Event _event;
	
};

void testThreads()
{
	Thread thread1("Thread1");
	Thread thread2("Thread2");
	Thread thread3("Thread3");
	Thread thread4("Thread4");

	MyRunnable *r1 = new MyRunnable();
	MyRunnable *r2 = new MyRunnable();
	MyRunnable *r3 = new MyRunnable();
	MyRunnable *r4 = new MyRunnable();
	assert (!thread1.isRunning());
	assert (!thread2.isRunning());
	assert (!thread3.isRunning());
	assert (!thread4.isRunning());
	thread1.start(*r1);
	Thread::sleep(200);
	assert (thread1.isRunning());
	assert (!thread2.isRunning());
	assert (!thread3.isRunning());
	assert (!thread4.isRunning());
	thread2.start(*r2);
	thread3.start(*r3);
	thread4.start(*r4);
	Thread::sleep(200);
	assert (thread1.isRunning());
	assert (thread2.isRunning());
	assert (thread3.isRunning());
	assert (thread4.isRunning());
	/*
	r4.notify();Thread::sleep(200);
	//assert (!thread4.isRunning());
	//assert (thread1.isRunning());
	//assert (thread2.isRunning());
	//assert (thread3.isRunning());
	r3.notify();Thread::sleep(200);
	//assert (!thread3.isRunning());
	r2.notify();Thread::sleep(200);
	//thread4.join();
	//thread3.join();
	//thread2.join();
	//assert (!thread2.isRunning());
	r1.notify();
	/*
	//thread1.join();
	//assert (!thread1.isRunning());
	assert (r1.ran());
	assert (r1.threadName() == "Thread1");
	assert (r2.ran());
	assert (r2.threadName() == "Thread2");
	assert (r3.ran());
	assert (r3.threadName() == "Thread3");
	assert (r4.ran());
	assert (r4.threadName() == "Thread4");
	*/
}

int main()
{
	testThreads();
	int i=5;
	while(i--)
	{
		Thread::sleep(5*1000);
		cout<<i<<" main running\n";
	}
	return 0;
}
