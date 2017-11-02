#include "CameraManager.h"
#include <Profiler.h>

using namespace DirectX;


SE::Core::CameraManager::CameraManager(const InitializationInfo & initInfo) : initInfo(initInfo)
{
	_ASSERT(initInfo.renderer);
	_ASSERT(initInfo.transformManager);

	initInfo.transformManager->RegisterSetDirty({ this, &CameraManager::SetDirty });

	auto result = initInfo.renderer->GetPipelineHandler()->CreateConstantBuffer("OncePerFrame", sizeof(XMFLOAT4X4), nullptr);
	if (result < 0)
		throw std::exception("Could not create OncePerFrame buffer");

	result = initInfo.renderer->GetPipelineHandler()->CreateConstantBuffer("CameraPos", sizeof(XMFLOAT4), nullptr);
	if (result < 0)
		throw std::exception("Could not create CameraPos buffer");

	currentActive.activeCamera = ~0u;

	Allocate(2);
}

SE::Core::CameraManager::~CameraManager()
{
	operator delete(cameraData.data);
}

void SE::Core::CameraManager::Create(const Entity & entity, CreateInfo & info)
{
	StartProfile;
	auto find = entityToIndex.find(entity);
	if (find != entityToIndex.end())
		ProfileReturnVoid;

	// Check if the entity is alive
	if (!initInfo.entityManager->Alive(entity))
		ProfileReturnVoid;

	// Make sure we have enough memory.
	if (cameraData.used + 1 > cameraData.allocated)
		Allocate(cameraData.allocated * 2);


	auto index = cameraData.used++;
	entityToIndex[entity] = index;

	cameraData.entity[index] = entity;
	cameraData.dirty[index] = ~0u;
	cameraData.fov[index] = info.fov;
	cameraData.aspectRatio[index] = info.aspectRatio;
	cameraData.nearPlane[index] = info.nearPlane;
	cameraData.farPlane[index] = info.farPlance;
	XMStoreFloat4x4(&cameraData.view[index], XMMatrixIdentity());

	initInfo.transformManager->Create(entity, info.posistion, info.rotation);

	StopProfile;
}

void SE::Core::CameraManager::UpdateCamera(const Entity & entity, const CreateInfo & info)
{
	StartProfile;
	auto find = entityToIndex.find(entity);
	if (find == entityToIndex.end())
		ProfileReturnVoid;

	cameraData.fov[find->second] = info.fov;
	cameraData.aspectRatio[find->second] = info.aspectRatio;
	cameraData.nearPlane[find->second] = info.nearPlane;
	cameraData.farPlane[find->second] = info.farPlance;

	initInfo.transformManager->SetAsDirty(entity);
	StopProfile;
}

void SE::Core::CameraManager::UpdateCamera(const CreateInfo & info)
{
	StartProfile;
	if (currentActive.activeCamera != ~0)
	{
		cameraData.fov[currentActive.activeCamera] = info.fov;
		cameraData.aspectRatio[currentActive.activeCamera] = info.aspectRatio;
		cameraData.nearPlane[currentActive.activeCamera] = info.nearPlane;
		cameraData.farPlane[currentActive.activeCamera] = info.farPlance;

		initInfo.transformManager->SetAsDirty(currentActive.activeCamera);
	}
	StopProfile;
}


DirectX::XMFLOAT4X4 SE::Core::CameraManager::GetView(const Entity & entity)
{
	StartProfile;
	XMFLOAT4X4 retMat;
	const auto find = entityToIndex.find(entity);
	if (find != entityToIndex.end())
	{
		retMat = cameraData.view[find->second];
	}
	else
	{
		XMStoreFloat4x4(&retMat, XMMatrixIdentity());
	}
	ProfileReturnConst(retMat);
}

DirectX::XMFLOAT4X4 SE::Core::CameraManager::GetViewInv(const Entity & entity)
{
	StartProfile;
	XMFLOAT4X4 retMat;
	const auto find = entityToIndex.find(entity);
	if (find != entityToIndex.end())
	{
		XMMATRIX view = XMLoadFloat4x4(&cameraData.view[find->second]);
		view = XMMatrixInverse(nullptr, view);
		XMStoreFloat4x4(&retMat, view);
	}
	else
	{
		XMStoreFloat4x4(&retMat, XMMatrixIdentity());
	}
	ProfileReturnConst(retMat);
}

DirectX::XMFLOAT4X4 SE::Core::CameraManager::GetProjection(const Entity& entity)
{
	StartProfile;
	XMFLOAT4X4 retMat;
	const auto f = entityToIndex.find(entity);
	if(f != entityToIndex.end())
	{
		XMMATRIX m = XMMatrixPerspectiveFovLH(cameraData.fov[f->second], cameraData.aspectRatio[f->second], cameraData.nearPlane[f->second], cameraData.farPlane[f->second]);
		XMStoreFloat4x4(&retMat, m);
	}
	else
	{
		XMStoreFloat4x4(&retMat, XMMatrixIdentity());
	}
	ProfileReturnConst(retMat);

}

DirectX::XMFLOAT4X4 SE::Core::CameraManager::GetViewProjection(const Entity& entity)
{
	StartProfile;
	XMFLOAT4X4 retMat;
	const auto f = entityToIndex.find(entity);
	if (f != entityToIndex.end())
	{
		XMMATRIX proj = XMMatrixPerspectiveFovLH(cameraData.fov[f->second], cameraData.aspectRatio[f->second], cameraData.nearPlane[f->second], cameraData.farPlane[f->second]);
		XMMATRIX view = XMLoadFloat4x4(&cameraData.view[f->second]);
		XMStoreFloat4x4(&retMat, view * proj);
	}
	else
	{
		XMStoreFloat4x4(&retMat, XMMatrixIdentity());
	}
	ProfileReturnConst(retMat);
}

void SE::Core::CameraManager::WorldSpaceRayFromScreenPos(int x, int y, int screenWidth, int screenHeight, DirectX::XMVECTOR& origin, DirectX::XMVECTOR& direction) const
{
	const size_t index = currentActive.activeCamera;
	XMMATRIX proj = XMMatrixPerspectiveFovLH(cameraData.fov[index], cameraData.aspectRatio[index], cameraData.nearPlane[index], cameraData.farPlane[index]);
	XMFLOAT4X4 projF;
	XMStoreFloat4x4(&projF, proj);
	XMMATRIX view = XMLoadFloat4x4(&cameraData.view[index]);
	XMMATRIX invView = XMMatrixInverse(nullptr, view);

	float xNDC = ((2.0f * x) / screenWidth) - 1;
	float yNDC = -(((2.0f * y) / screenHeight) - 1);

	float xView = xNDC / projF._11;
	float yView = yNDC / projF._22;
	direction = XMVector3Normalize(XMVector4Transform(XMVectorSet(xView, yView, 1.0f, 0.0f), invView));
	XMFLOAT3 pos = initInfo.transformManager->GetPosition(currentActive.entity);
	origin = XMVectorSet(pos.x, pos.y, pos.z, 1.0f);
	

}

void SE::Core::CameraManager::SetActive(const Entity & entity)
{
	StartProfile;
	auto find = entityToIndex.find(entity);
	if (find == entityToIndex.end())
		ProfileReturnVoid;

	currentActive.entity = entity;
	currentActive.activeCamera = find->second;

	StopProfile;
}

void SE::Core::CameraManager::Frame(Utilz::TimeCluster * timer)
{
	_ASSERT(timer);
	StartProfile;
	timer->Start(CREATE_ID_HASH("CameraManager"));
	GarbageCollection();
	if (currentActive.activeCamera != ~0u)
	{
		if (cameraData.dirty[currentActive.activeCamera] != ~0u)// Update the transform
		{
			XMMATRIX transform = XMLoadFloat4x4(&initInfo.transformManager->GetCleanedTransforms()[cameraData.dirty[currentActive.activeCamera]]);
			XMVECTOR pos = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);// XMLoadFloat3(&transformManager->positions[tindex]);
															   //	auto rotation = XMMatrixRotationRollPitchYawFromVector(XMLoadFloat3(&transformManager->rotations[tindex]));
			XMVECTOR forward = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
			XMVECTOR up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
			pos = XMVector3TransformCoord(pos, transform);
			forward = XMVector3TransformNormal(forward, transform);
			up = XMVector3TransformNormal(up, transform);
			XMVECTOR lookAt = pos + forward;

			XMMATRIX view = XMMatrixLookAtLH(pos, lookAt, up);
			XMStoreFloat4x4(&cameraData.view[currentActive.activeCamera], view);
			XMMATRIX viewproj = view * XMMatrixPerspectiveFovLH(cameraData.fov[currentActive.activeCamera], cameraData.aspectRatio[currentActive.activeCamera], cameraData.nearPlane[currentActive.activeCamera], cameraData.farPlane[currentActive.activeCamera]);

			XMFLOAT4X4 viewProjMatrix;
			XMStoreFloat4x4(&viewProjMatrix, XMMatrixTranspose(viewproj));
			initInfo.renderer->GetPipelineHandler()->UpdateConstantBuffer("OncePerFrame", &viewProjMatrix, sizeof(XMFLOAT4X4));

			XMFLOAT3 fpos;
			XMStoreFloat3(&fpos, pos);
			initInfo.renderer->GetPipelineHandler()->UpdateConstantBuffer("CameraPos", &fpos, sizeof(XMFLOAT3));


			cameraData.dirty[currentActive.activeCamera] = ~0u;
		}



	}
	timer->Stop(CREATE_ID_HASH("CameraManager"));
	StopProfile;
}

void SE::Core::CameraManager::SetDirty(const Entity & entity, size_t index)
{
	StartProfile;
	auto find = entityToIndex.find(entity);
	if (find == entityToIndex.end())
		ProfileReturnVoid;

	cameraData.dirty[find->second] = index;

	StopProfile;
}

void SE::Core::CameraManager::Allocate(size_t size)
{
	StartProfile;
	_ASSERT(size > cameraData.allocated);

	// Allocate new memory
	CameraData newData;
	newData.allocated = size;
	newData.data = operator new(size * CameraData::size);
	newData.used = cameraData.used;

	// Setup the new pointers
	newData.entity = (Entity*)newData.data;
	newData.dirty = (size_t*)(newData.entity + newData.allocated);
	newData.fov = reinterpret_cast<float*>(newData.dirty + newData.allocated);
	newData.aspectRatio = (float*)(newData.fov + newData.allocated);
	newData.nearPlane = (float*)(newData.aspectRatio + newData.allocated);
	newData.farPlane = (float*)(newData.nearPlane + newData.allocated);
	newData.view = (XMFLOAT4X4*)(newData.farPlane + newData.allocated);

	// Copy data
	memcpy(newData.entity, cameraData.entity, cameraData.used * sizeof(Entity));
	memcpy(newData.dirty, cameraData.dirty, cameraData.used * sizeof(size_t));
	memcpy(newData.fov, cameraData.fov, cameraData.used * sizeof(float));
	memcpy(newData.aspectRatio, cameraData.aspectRatio, cameraData.used * sizeof(float));
	memcpy(newData.nearPlane, cameraData.nearPlane, cameraData.used * sizeof(float));
	memcpy(newData.farPlane, cameraData.farPlane, cameraData.used * sizeof(float));
	memcpy(newData.view, cameraData.view, cameraData.used * sizeof(XMFLOAT4X4));

	// Delete old data;
	operator delete(cameraData.data);
	cameraData = newData;

	StopProfile;
}

void SE::Core::CameraManager::Destroy(size_t index)
{
	StartProfile;

	// Temp variables
	size_t last = cameraData.used - 1;
	const Entity entity = cameraData.entity[index];
	const Entity last_entity = cameraData.entity[last];

	// Copy the data
	cameraData.entity[index] = last_entity;
	cameraData.dirty[index] = cameraData.dirty[last];
	cameraData.fov[index] = cameraData.fov[last];
	cameraData.aspectRatio[index] = cameraData.aspectRatio[last];
	cameraData.nearPlane[index] = cameraData.nearPlane[last];
	cameraData.farPlane[index] = cameraData.farPlane[last];
	cameraData.view[index] = cameraData.view[last];

	// Replace the index for the last_entity 
	entityToIndex[last_entity] = index;
	entityToIndex.erase(entity);
	if (currentActive.activeCamera == last)
		currentActive.activeCamera = index;
	else if (currentActive.activeCamera == index)
		currentActive.activeCamera = ~0u;
	cameraData.used--;
	StopProfile;
}

void SE::Core::CameraManager::Destroy(const Entity & entity)
{
}

void SE::Core::CameraManager::GarbageCollection()
{
	StartProfile;
	uint32_t alive_in_row = 0;
	while (cameraData.used > 0 && alive_in_row < 4U)
	{
		std::uniform_int_distribution<uint32_t> distribution(0U, cameraData.used - 1U);
		uint32_t i = distribution(generator);
		if (initInfo.entityManager->Alive(cameraData.entity[i]))
		{
			alive_in_row++;
			continue;
		}
		alive_in_row = 0;
		Destroy(i);
	}
	StopProfile;
}
