#include "MemoryMeasuring.h"
#include <Profiler.h>
#include <string>

namespace SE
{
	namespace Graphics
	{
		MemoryMeasuring::MemoryMeasuring()
		{

		}

		MemoryMeasuring::~MemoryMeasuring()
		{
			if (dxgifactory != nullptr)
				dxgifactory->Release();
			if (dxgiAdapter3 != nullptr)
				dxgiAdapter3->Release();
		}

		int MemoryMeasuring::Init()
		{
			HRESULT ret_code = ::CreateDXGIFactory(
				__uuidof(IDXGIFactory),
				reinterpret_cast<void**>(&dxgifactory));

			if (SUCCEEDED(ret_code))
			{
				IDXGIAdapter* firstAdapter;
				ret_code = dxgifactory->EnumAdapters(0, &firstAdapter);

				if (SUCCEEDED(firstAdapter->QueryInterface(__uuidof(IDXGIAdapter3), (void**)&dxgiAdapter3)))
				{
					return 0;
				}
				return -1;
			}
			return -1;
		}

		int MemoryMeasuring::printUsage(SE::Utilz::IConsoleBackend* console)
		{
			DXGI_QUERY_VIDEO_MEMORY_INFO info;
			if (SUCCEEDED(dxgiAdapter3->QueryVideoMemoryInfo(0, DXGI_MEMORY_SEGMENT_GROUP_LOCAL, &info)))
			{
				auto memoryUsage = info.CurrentUsage / 1024 / 1024; //MiB
				std::string memString = "Memory usage " + std::to_string(memoryUsage) + "\n";
				console->Print(memString.c_str());
				return 0;
			}
			return -1;
		}

	}
}