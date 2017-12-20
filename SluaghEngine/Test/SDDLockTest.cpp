#include "SDDLockTest.h"
#include "../ResourceHandler/RawLoader.h"
#include <future>

namespace SE::Test
{
	SDDLockTest::SDDLockTest()
	{
	}


	SDDLockTest::~SDDLockTest()
	{
	}

	bool SE::Test::SDDLockTest::Run(SE::DevConsole::IConsole * console)
	{
		std::mutex lock;
		ResourceHandler::RawLoader l;

		l.Initialize();
		void* data;
		auto r = l.LoadResource("Candlestick_texture_metal.jpg", &data);
		if (r == 0)
			console->Print("Suc");
		r = l.LoadResource("Candlestick_texture_metal1.jpg", &data);
		if (r == 0)
			console->Print("Suc");
		r = l.LoadResource("Candlestick_texture_metal2.jpg", &data);
		if (r == 0)
			console->Print("Suc");
		r = l.LoadResource("Candlestick_texture_metal3.jpg", &data);
		if (r == 0)
		
			console->Print("Suc");
		double avgDiff = 0.0;
		for(int t = 0; t < 1000; t++)
		{
			std::promise<void> startP;
			std::shared_future<void> sf(startP.get_future());
			std::promise<void> readyP
				[4];
			std::promise<void> doneP[4];
			std::thread([&lock, &l, &sf, &readyP, &doneP] {
				readyP[0].set_value();
				sf.wait();
				void* data;
				std::lock_guard<std::mutex> ml(lock);
				l.LoadResource("Candlestick_texture_metal.jpg", &data);
				doneP[0].set_value();

				operator delete(data);
			}).detach();
			std::thread([&lock, &l, &sf, &readyP, &doneP] {
				readyP[1].set_value();
				sf.wait();
				void* data;
				std::lock_guard<std::mutex> ml(lock);
				l.LoadResource("Candlestick_texture_metal1.jpg", &data);
				doneP[1].set_value();
				operator delete(data);
			}).detach();
			std::thread([&lock, &l, &sf, &readyP, &doneP] {
				readyP[2].set_value();
				sf.wait();
				std::lock_guard<std::mutex> ml(lock);
				void* data;
				l.LoadResource("Candlestick_texture_metal2.jpg", &data);
				doneP[2].set_value();
				operator delete(data);
			}).detach();
			std::thread([&lock, &l, &sf, &readyP, &doneP] {
				readyP[3].set_value();
				sf.wait();
				void* data;
				std::lock_guard<std::mutex> ml(lock);
				l.LoadResource("Candlestick_texture_metal3.jpg", &data);
				doneP[3].set_value();
				operator delete(data);
			}).detach();

			for (int i = 0; i < 4; i++)
				readyP[i].get_future().wait();

			auto start = std::chrono::high_resolution_clock::now();
			startP.set_value();
			for (int i = 0; i < 4; i++)
				doneP[i].get_future().wait();

			auto end = std::chrono::high_resolution_clock::now();
			auto diff = std::chrono::duration<double, std::milli>(end - start).count();
			//console->Print("%d - %f", t, diff);
			avgDiff += diff;
		}


		console->Print("Avg Locked: %f", avgDiff / 1000.0);




		avgDiff = 0.0;
		for (int t = 0; t < 1000; t++)
		{
			std::promise<void> startP;
			std::shared_future<void> sf(startP.get_future());
			std::promise<void> readyP
				[4];
			std::promise<void> doneP[4];
			std::thread([&lock, &l, &sf, &readyP, &doneP] {
				readyP[0].set_value();
				sf.wait();
				void* data;
				l.LoadResource("Candlestick_texture_metal.jpg", &data);
				doneP[0].set_value();

				operator delete(data);
			}).detach();
			std::thread([&lock, &l, &sf, &readyP, &doneP] {
				readyP[1].set_value();
				sf.wait();
				void* data;
				l.LoadResource("Candlestick_texture_metal1.jpg", &data);
				doneP[1].set_value();
				operator delete(data);
			}).detach();
			std::thread([&lock, &l, &sf, &readyP, &doneP] {
				readyP[2].set_value();
				sf.wait();
				void* data;
				l.LoadResource("Candlestick_texture_metal2.jpg", &data);
				doneP[2].set_value();
				operator delete(data);
			}).detach();
			std::thread([&lock, &l, &sf, &readyP, &doneP] {
				readyP[3].set_value();
				sf.wait();
				void* data;
				l.LoadResource("Candlestick_texture_metal3.jpg", &data);
				doneP[3].set_value();
				operator delete(data);
			}).detach();

			for (int i = 0; i < 4; i++)
				readyP[i].get_future().wait();

			auto start = std::chrono::high_resolution_clock::now();
			startP.set_value();
			for (int i = 0; i < 4; i++)
				doneP[i].get_future().wait();

			auto end = std::chrono::high_resolution_clock::now();
			auto diff = std::chrono::duration<double, std::milli>(end - start).count();
			//console->Print("%d - %f", t, diff);
			avgDiff += diff;
		}
		console->Print("Avg no lock: %f", avgDiff / 1000.0);


		return false;
	}
}