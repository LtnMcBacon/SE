#pragma once
#ifndef SE_UTILZ_MEMORYMEASURING_H_
#define SE_UTILZ_MEMORYMEASURING_H_
#include <d3d11_3.h>
#include <dxgi1_4.h>
#include <Utilz\Console.h>


#pragma comment(lib, "dxgi.lib")

namespace SE
{
	namespace Utilz
	{
		class MemoryMeasuring
		{
		public:
			MemoryMeasuring();
			~MemoryMeasuring();

			int Init();
			int printUsage(SE::Utilz::IConsoleBackend* console);
		private:
			IDXGIFactory* dxgifactory = nullptr;
			IDXGIAdapter3* dxgiAdapter3 = nullptr;
		};
	}
}

#endif // SE_UTILZ_MEMORYMEASURING_H_