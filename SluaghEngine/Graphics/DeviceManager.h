#ifndef DEVICEMANAGER_H
#define DEVICEMANAGER_H

#include <d3d11.h>
#include <d3dcompiler.h>	// We also need the D3DCompiler header to compile shaders
#include <DirectXMath.h>
#include <Windows.h>
#include <iostream>

#include "LiveObjectReporter.h"

namespace SE {

	namespace Graphics {

		/**
		*
		* @brief 
		* DeviceManager is responsible for managing and initializing the DirectX core components
		*
		* @details DeviceManager requires a window handle for its initialization, mainly for the swap chain
		*
		* @warning Error handling is taken care of internally from DeviceManager. If any of the core components fail to initialize, the application will be closed
		*
		* @sa ”See Also”,
		* Renderer
		**/
		class DeviceManager {

		public:

			DeviceManager();
			~DeviceManager();

			/**
			* @brief Init initializes the DeviceManager
			* @details Calls for all necessary functions for all components to be initialized
			* @retval return_value_n Returns a HRESULT indicating if the DirectX core was successfully created or not
			* @endcode
			*/
			HRESULT Init(HWND windowHandle);

			/**
			* @brief Shutdown releases the DirectX components
			* @details Releases the pointers
			* @endcode
			*/
			void Shutdown();

			/**
			* @brief CreateDeviceResources creates the graphic device and Graphic device context
			* @details
			* CreateDeviceResources first specifies the feature level and setups the graphic device debug flag.
			* Finally, it calls upon D3D11CreateDevice to create both the graphic device and the graphic device context
			* @retval return_value_n Returns a HRESULT indicating if the Graphic Device and Graphic Context was successfully created or not
			* @warning Make sure to interpret the HRESULT returned from CreateDeviceResources and stop the application from proceeding if HRESULT is S_FALSE == FAILED(hr)
			* Example code:
			* @code
			*	HRESULT hr = S_OK;
			*
			*	hr = CreateDeviceResources();
			*
			*	if (FAILED(hr)) {
			*
			*		Error handling
			*	}
			* @endcode
			*/
			HRESULT CreateDeviceResources();

			/**
			* @brief	
			* CreateSwapChain creates the swap chain
			* @details CreateSwapChain first defines a swap chain description. It proceeds to create an adapter that calls upon the parent factory. From the factory, a swap chain is created
			* @param[in] windowHandle A window handle 
			* @retval return_value_n Returns a HRESULT indicating if the swap chain was successfully created or not
			* @warning Make sure to interpret the HRESULT returned from CreateSwapChain and stop the application from proceeding if HRESULT is S_FALSE == FAILED(hr)
			* Example code:
			* @code
			*	HRESULT hr = S_OK;
			*
			*	hr = CreateSwapChain(HWND windowHandle);
			*
			*	if (FAILED(hr)) {
			*
			*		Error handling
			*	}
			* @endcode
			*/
			HRESULT CreateSwapChain(HWND windowHandle);

			/**
			* @brief	
			* CreateBackBufferRTV creates the back buffer render target view
			* @details
			* CreateBackBufferRTV gets the Texture2D from the swap chain and puts in in gBackBuffer. The texture in gBackBuffer is then used to create the gBackBufferRTV
			* @retval return_value_n Returns a HRESULT indicating if the render target view was successfully created or not
			* @warning Make sure to interpret the HRESULT returned from CreateBackBufferRTV and stop the application from proceeding if HRESULT is S_FALSE == FAILED(hr)
			* Example code:
			* @code
			*	HRESULT hr = S_OK;
			*
			*	hr = CreateBackBufferRTV();
			*
			*	if (FAILED(hr)) {
			*
			*		Error handling
			*	}
			* @endcode
			*/
			HRESULT CreateBackBufferRTV();

			/**
			* @brief	
			* CreateDepthStencil creates the default depth stencil view
			* @details
			* CreateDepthStencil defines a texture 2D description and creates a texture from it. The texture is then used to create the depth stencil view
			* @retval return_value_n Returns a HRESULT indicating if the depth stencil view was successfully created or not
			* @warning Make sure to interpret the HRESULT returned from CreateDepthStencil and stop the application from proceeding if HRESULT is S_FALSE == FAILED(hr)
			* Example code:
			* @code
			*	HRESULT hr = S_OK;
			*
			*	hr = CreateDepthStencil();
			*
			*	if (FAILED(hr)) {
			*
			*		Error handling
			*	}
			* @endcode
			*/
			HRESULT CreateDepthStencil();

			/**
			* @brief SetViewport initializes the default viewport
			* @details From the back buffer texture, width and height is used to set the size of the viewport
			* @endcode
			*/
			void SetViewport();


			/**
			* @brief Present displays the rendered image to the user
			* @endcode
			*/
			void Present();

			/**
			* @brief Resizes the swapchain
			* @param[in] windowHandle A window handle.
			* @endcode
			*/
			void ResizeSwapChain(HWND windowHandle);

			/**
			* @brief Creates a blend state
			* @endcode
			*/
			void CreateBlendState();

			inline ID3D11Device*           GetDevice() { return gDevice; };
			inline ID3D11DeviceContext*    GetDeviceContext() { return gDeviceContext; };
			inline ID3D11DeviceContext*    GetSecondaryDeviceContext() { return gSecDeviceContext; };
			inline ID3D11RenderTargetView* GetRTV() const { return gBackbufferRTV; };
			inline ID3D11DepthStencilView* GetDepthStencil() { return gDepthStencilView; };
			inline ID3D11BlendState*	   GetBlendState() { return blendTransState; };
			
			inline void SetDepthStencilStateAndRS()
			{
				gDeviceContext->RSSetState(rasterSolidState);
				gDeviceContext->OMSetDepthStencilState(pDSState, 1);
			}

			inline void SetRasterStateFill(uint8_t fillSolid)
			{
				if (fillSolid == 1)
				{
					gDeviceContext->RSSetState(rasterSolidState);
				}
				else
				{
					gDeviceContext->RSSetState(rasterWireState);
				}
			}

			inline void SetBlendTransparencyState(uint8_t transparency)
			{
				if (transparency == 1)
				{
					UINT sampleM = 0xFF;
					gDeviceContext->OMSetBlendState(blendTransState, NULL, sampleM);
				}
				else
				{
					UINT sampleM = 0xFF;
					gDeviceContext->OMSetBlendState(blendSolidState, NULL, sampleM);
				}
			}

			inline D3D11_TEXTURE2D_DESC GetTexDesc() {
				return gBB_Desc;
			}

			inline ID3D11ShaderResourceView* GetDepthStencilSRV()
			{
				return gDepthStencilSRV;
			}

			ID3D11Texture2D* GetBackBufferTexture();
		private:

			ID3D11Device*			gDevice;
			ID3D11DeviceContext*	gDeviceContext;
			ID3D11DeviceContext*	gSecDeviceContext;
			IDXGISwapChain*			gSwapChain;
			
			ID3D11Texture2D*		gBackBuffer;
			ID3D11RenderTargetView*	gBackbufferRTV;
			
			ID3D11Texture2D*		gDepthStencil;
			ID3D11DepthStencilView*	gDepthStencilView;
			ID3D11ShaderResourceView* gDepthStencilSRV;
			ID3D11DepthStencilState * pDSState;

			ID3D11BlendState*		blendSolidState;
			ID3D11BlendState*		blendTransState;
			ID3D11RasterizerState * rasterSolidState;
			ID3D11RasterizerState * rasterWireState;

			D3D11_TEXTURE2D_DESC	gBB_Desc;
			D3D_FEATURE_LEVEL		gFeatureLevel;
			D3D11_VIEWPORT			gViewportDefault;

		};
	}
}

#endif 
