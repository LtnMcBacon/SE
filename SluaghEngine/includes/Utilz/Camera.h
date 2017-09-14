#ifndef CAMERA_H
#define CAMERA_H

#include <windows.h>
#include <d3d11.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include <iostream>

namespace SE {

	namespace Utilz {

		class Camera {
		
		public:
		
			// Constructor and destructor
			Camera();
			~Camera();
		
			void Update(float deltaTime);
			void UpdateBuffer();
		
			void mouseEvent(float delta);
			void moveEvent(float deltaTime, float speed);
		
			POINT mLastMousePos;
		
			// Get/Set Camera Properties
			DirectX::XMVECTOR GetPositionXM()const;
			DirectX::XMFLOAT3 GetPosition()const;
			void SetPosition(float x, float y, float z);
			void SetPosition(const DirectX::XMFLOAT3& v);
		
			// Get Camera Vectors
			DirectX::XMVECTOR GetRightXM()const;
			DirectX::XMFLOAT3 GetRight()const;
			DirectX::XMVECTOR GetUpXM()const;
			DirectX::XMFLOAT3 GetUp()const;
			DirectX::XMVECTOR GetLookXM()const;
			DirectX::XMFLOAT3 GetLook()const;
		
			// Get Frustum Properties
			float GetNearZ()const;
			float GetFarZ()const;
			float GetAspect()const;
			float GetFovY()const;
			float GetFovX()const;
		
			// Get Near and Far Plane dimensions in View Space coordinates
			float GetNearWindowWidth()const;
			float GetNearWindowHeight()const;
			float GetFarWindowWidth()const;
			float GetFarWindowHeight()const;
		
			// Set frustum
			void SetLens(float fovY, float aspect, float zn, float zf);
			void SetFovY(float fovY);
		
			// Define Camera Space via Look At parameters
			void LookAt(DirectX::XMVECTOR pos, DirectX::XMVECTOR target, DirectX::XMVECTOR worldUp);
			void LookAt(const DirectX::XMFLOAT3& pos, const DirectX::XMFLOAT3& target, const DirectX::XMFLOAT3& up);
		
			// Get View / Proj matrices
			DirectX::XMMATRIX View()const;
			DirectX::XMMATRIX Proj()const;
			DirectX::XMMATRIX ViewProj()const;
		
			// Strafe / Walk the camera a distance d
			void Strafe(float d);
			void Walk(float d);
		
			// Rotate the camera
			void Pitch(float angle);
			void RotateY(float angle);
		
			// Update View Matrix after every frame 
			void UpdateViewMatrix();
		
			float GetX()const; 
			float GetZ()const; 
		
			DirectX::XMFLOAT4X4 inverseView;
			
			DirectX::XMFLOAT3 eyePosF;
			DirectX::XMFLOAT3 lookAtF;
			DirectX::XMFLOAT3 upF;
		
			DirectX::XMVECTOR eyePos;
			DirectX::XMVECTOR lookAt;
			DirectX::XMVECTOR up;
		
			DirectX::XMMATRIX viewMatrix;
			DirectX::XMMATRIX projectionMatrix;
			DirectX::XMMATRIX viewProj;
			DirectX::XMMATRIX InvViewPoj;
			DirectX::XMVECTOR detViewProj;
			DirectX::XMVECTOR projDet;
		
			DirectX::XMMATRIX ProjInv;
			DirectX::XMVECTOR viewDet;
			DirectX::XMMATRIX ViewInv;
		
		private:
		
			D3D11_MAPPED_SUBRESOURCE mappedResource;
		
			// Camera coordinate system with coordinates relative to World Space
			DirectX::XMFLOAT3 mPosition;	// View Space Origin
			DirectX::XMFLOAT3 mRight;		// View Space X-Axis
			DirectX::XMFLOAT3 mUp;			// View Space Y-Axis
			DirectX::XMFLOAT3 mLook;		// ViewSpace Z-Axis
		
			// Cache Frustum Properties
			float mNearZ;
			float mFarZ;
			float mAspect;
			float mFovY;
			float mNearWindowHeight;
			float mFarWindowHeight;
		
			float speed;
			float rotAngle;
		
			// Cache View/Proj Matrices 
			DirectX::XMFLOAT4X4 mView;
			DirectX::XMFLOAT4X4 mProj;
			
		
		};

	}

}

#endif CAMERA_H