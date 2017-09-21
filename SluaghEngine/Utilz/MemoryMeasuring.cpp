#include <Utilz\MemoryMeasuring.h>
#include <Profiler.h>
#include <string>

namespace SE
{
	namespace Utilz
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
			StartProfile;
			HRESULT ret_code = ::CreateDXGIFactory(
				__uuidof(IDXGIFactory),
				reinterpret_cast<void**>(&dxgifactory));

			if (SUCCEEDED(ret_code))
			{
				IDXGIAdapter* firstAdapter;
				ret_code = dxgifactory->EnumAdapters(0, &firstAdapter);

				if (SUCCEEDED(firstAdapter->QueryInterface(__uuidof(IDXGIAdapter3), (void**)&dxgiAdapter3)))
				{
					ProfileReturnConst(0);
				}
				ProfileReturnConst(-1);
			}
			ProfileReturnConst(-1);
		}

		int MemoryMeasuring::printUsage(SE::Utilz::IConsoleBackend* console)
		{
			StartProfile;
			DXGI_QUERY_VIDEO_MEMORY_INFO info;
			if (SUCCEEDED(dxgiAdapter3->QueryVideoMemoryInfo(0, DXGI_MEMORY_SEGMENT_GROUP_LOCAL, &info)))
			{
				int memoryUsage = info.CurrentUsage / 1024 / 1024; //MiB
				std::string memString = "Memory usage " + std::to_string(memoryUsage) + "\n";
				console->Print(memString.c_str());
				/*char msg[100];
				sprintf_s(msg, "%d", memoryUsage);
				MessageBoxA(0, msg, "", 0);*/
				ProfileReturnConst(0);
			}
			ProfileReturnConst(-1);
		}

	}
}