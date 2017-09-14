#ifndef LIVEOBJECTREPORTER_H
#define LIVEOBJECTREPORTER_H

#include <d3d11.h>			// We require the d3d11 header for Direct3D functions
#include <d3dcompiler.h>	// We also need the D3DCompiler header to compile shader
#include <DirectXMath.h>
#include <string>



namespace SE {

	namespace Graphics {

		/**
		* @brief
		* reportLiveObjects checks for DirectX memory leaks
		*
		* @details	
		* reportLiveObjects is used to report live objects when exiting the application
		*
		* @warning Make sure not to release the graphic device before reporting
		*
		* Example code:
		* @code
		* ifdef _DEBUG
		*
		*	reportLiveObjects(gDevice.Get());
		*
		* #endif
		*
		* gDevice.Reset();
		* @endcode
		*/
		void reportLiveObjects(ID3D11Device* gDevice);

		/**
		* @brief
		* reportLiveObjects is used to set specific names for DirectX componenets for debugging with reportLiveObjects()
		*
		* @details
		* reportLiveObjects is used to report live objects when exiting the application
		*
		* @warning Make sure to use a unique name or a warning might arise that several DirectX components share the same private name
		*
		* Example code:
		* @code
		* setDebugName(gBackBuffer.Get(), "STANDARD_BACK_BUFFER_TEXTURE2D");
		* @endcode
		*/
		void setDebugName(ID3D11DeviceChild* child, const std::string &name);
	}
}

#endif
