#include "ThreadPoolTest.h"
#include <Utilz/ThreadPool.h>

SE::Test::ThreadPoolTest::ThreadPoolTest()
{
}

SE::Test::ThreadPoolTest::~ThreadPoolTest()
{
}

bool SE::Test::ThreadPoolTest::Run(DevConsole::IConsole* console)
{
	Utilz::ThreadPool tp(4);
	std::mutex lock;
	int someInt = 0;
	std::vector<std::future<void>> results;
	results.reserve(1000);
	for(int i = 0; i < 1000; ++i)
	{
		results.emplace_back(tp.Enqueue([&someInt, &lock]()
		{
			lock.lock();
			++someInt;
			lock.unlock();
		}));
	}

	for(int i = 0; i < 1000; ++i)
	{
		results[i].get();
	}
	if (someInt != 1000)
		return false;
	return true;
}
