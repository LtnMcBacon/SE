#include "ThreadPoolTest.h"
#include <Utilz/ThreadPool.h>

SE::Test::ThreadPoolTest::ThreadPoolTest()
{
}

SE::Test::ThreadPoolTest::~ThreadPoolTest()
{
}

static int Chungus(int a, int b)
{
	return a + b;
}

struct X
{
	X(int i_) : i(i_) {  };
	int i;
	int Foo(int x)const
	{
		return i + x;
	}
};

bool SE::Test::ThreadPoolTest::Run(DevConsole::IConsole* console)
{
	Utilz::ThreadPool tp(4);
	{
		std::mutex lock;
		int someInt = 0;
		std::vector<std::future<void>> results;
		results.reserve(1000);
		for (int i = 0; i < 1000; ++i)
		{
			results.emplace_back(tp.Enqueue([&someInt, &lock]()
			{
				lock.lock();
				++someInt;
				lock.unlock();
			}));
		}

		for (int i = 0; i < 1000; ++i)
		{
			results[i].get();
		}
		if (someInt != 1000)
			return false;
	}
	//Testing adding a lambda
	{
		auto someLambda = []() {return 5; };
		auto future = tp.Enqueue(someLambda);
		if (future.get() != 5)
			return false;
	}

	//Testing a regular function.
	{
		auto future = tp.Enqueue(Chungus, 10, 15);
		if (future.get() != Chungus(10, 15))
			return false;
	}

	// Testing methods
	{
		X x(10);
		auto ret = tp.Enqueue(&x, &X::Foo, 20);
		if (ret.get() != 20 + x.i)
			return false;
	}
	
	

	
	return true;
}
