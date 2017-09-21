#include <Utilz\MemoryMeasuring.h>
#include <Profiler.h>

namespace SE
{
	namespace Utilz
	{
		MemoryMeasuring::MemoryMeasuring()
		{

		}

		MemoryMeasuring::~MemoryMeasuring()
		{

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

				IDXGIAdapter3* dxgiAdapter3 = NULL;
				if (SUCCEEDED(firstAdapter->QueryInterface(__uuidof(IDXGIAdapter3), (void**)&dxgiAdapter3)))
				{
					ProfileReturnConst(0);
				}
				ProfileReturnConst(-1);
			}
			ProfileReturnConst(-1);
		}

		int MemoryMeasuring::printUsage()
		{
			StartProfile;
			DXGI_QUERY_VIDEO_MEMORY_INFO info;
			if (SUCCEEDED(dxgiAdapter3->QueryVideoMemoryInfo(0, DXGI_MEMORY_SEGMENT_GROUP_LOCAL, &info)))
			{
				int memoryUsage = info.CurrentUsage / 1024 / 1024; //MiB

				char msg[100];
				sprintf_s(msg, "%d", memoryUsage);
				MessageBoxA(0, msg, "", 0);
				ProfileReturnConst(0);
			}
			ProfileReturnConst(-1);
		}

	}
}