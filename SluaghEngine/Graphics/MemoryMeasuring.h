#pragma once
#ifndef SE_UTILZ_MEMORYMEASURING_H_
#define SE_UTILZ_MEMORYMEASURING_H_
#include <d3d11_3.h>
#include <dxgi1_4.h>
#include <Utilz\Console.h>


#pragma comment(lib, "dxgi.lib")

namespace SE
{
	namespace Graphics
	{
		class MemoryMeasuring
		{
		public:
			MemoryMeasuring();
			~MemoryMeasuring();

			/**
			* @brief Init MM
			*
			* @retval 0	 When successful
			* @retval -1 When unsuccessful
			*
			*/
			int Init();
			/**
			* @brief Print vRam to console
			*
			* @param[in] console To be able to printing the data
			*
			* @retval 0	 When successful
			* @retval -1 When unsuccessful
			*
			*/
			int printUsage(SE::Utilz::IConsoleBackend* console);

			inline size_t GetVRam() {
				DXGI_QUERY_VIDEO_MEMORY_INFO info;
				if (SUCCEEDED(dxgiAdapter3->QueryVideoMemoryInfo(0, DXGI_MEMORY_SEGMENT_GROUP_LOCAL, &info)))
				{
					return info.CurrentUsage;
				}
				return 0;
			};
		private:
			IDXGIFactory* dxgifactory = nullptr;
			IDXGIAdapter3* dxgiAdapter3 = nullptr;
		};
	}
}

#endif // SE_UTILZ_MEMORYMEASURING_H_