#include "LiveObjectReporter.h"
#include <Profiler.h>

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxguid.lib")


void SE::Graphics::reportLiveObjects(ID3D11Device* gDevice) {

	ID3D11Debug* DebugDevice = nullptr;
	HRESULT result = gDevice->QueryInterface(__uuidof(ID3D11Debug), reinterpret_cast<void**>(&DebugDevice));

	result = DebugDevice->ReportLiveDeviceObjects(D3D11_RLDO_DETAIL);

	DebugDevice->Release();
}

void SE::Graphics::setDebugName(ID3D11DeviceChild* child, const std::string &name) {

	if (child != nullptr) {

		child->SetPrivateData(WKPDID_D3DDebugObjectName, (UINT)name.size(), name.c_str());
	}
}