#pragma once
#ifndef SE_UTILZ_MEMORYMEASURING_H_
#define SE_UTILZ_MEMORYMEASURING_H_
#include <d3d11_3.h>
#include <dxgi1_4.h>

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
			int printUsage();
		private:
			IDXGIFactory* dxgifactory = nullptr;
			IDXGIAdapter3* dxgiAdapter3 = NULL;
		};
	}
}

#endif // SE_UTILZ_MEMORYMEASURING_H_