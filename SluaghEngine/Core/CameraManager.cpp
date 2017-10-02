#include <CameraManager.h>
#include <Profiler.h>
#include <DirectXMath.h>
using namespace DirectX;


SE::Core::CameraManager::CameraManager(Graphics::IRenderer* renderer, const EntityManager& entityManager, TransformManager* transformManager) 
	: entityManager(entityManager), 
	transformManager(transformManager),
	renderer(renderer),
	activeCamera(~0u)
{
	_ASSERT(renderer);
	_ASSERT(transformManager);

	transformManager->SetDirty.Add<CameraManager, &CameraManager::SetDirty>(this);


	Allocate(2);
}


SE::Core::CameraManager::~CameraManager()
{
	operator delete(cameraData.data);
}

void SE::Core::CameraManager::Bind(const Entity & entity, float fov, float aspectRatio, float nearP, float farP, const DirectX::XMFLOAT3& pos,
	const DirectX::XMFLOAT3& rotation)
{
	StartProfile;
	auto& find = entityToIndex.find(entity);
	if (find != entityToIndex.end())
		ProfileReturnVoid;
	
	// Check if the entity is alive
	if (!entityManager.Alive(entity))
		ProfileReturnVoid;

	// Make sure we have enough memory.
	if (cameraData.used + 1 > cameraData.allocated)
		Allocate(cameraData.allocated * 2);


	auto index = cameraData.used++;
	entityToIndex[entity] = index;

	cameraData.entity[index] = entity;
	cameraData.dirty[index] = ~0u;
	cameraData.fov[index] = fov;
	cameraData.aspectRatio[index] = aspectRatio;
	cameraData.nearPlane[index] = nearP;
	cameraData.farPlane[index] = farP;
	XMStoreFloat4x4(&cameraData.view[index], XMMatrixIdentity());

	transformManager->Create(entity, pos, rotation);

	StopProfile;
}

DirectX::XMFLOAT4X4 SE::Core::CameraManager::GetViewInv(const Entity & entity)
{
	StartProfile;
	const auto find = entityToIndex.find(entity);
	if (find == entityToIndex.end())
		throw std::exception("No camera bound to entity!");

	XMMATRIX view = XMLoadFloat4x4(&cameraData.view[find->second]);
	view = XMMatrixInverse(nullptr, view);
	XMFLOAT4X4 v;
	XMStoreFloat4x4(&v, view);
	ProfileReturnConst( v);
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

void SE::Core::CameraManager::SetActive(const Entity & entity)
{
	StartProfile;
	auto& find = entityToIndex.find(entity);
	if (find == entityToIndex.end())
		ProfileReturnVoid;

	activeCamera = find->second;

	StopProfile;
}

void SE::Core::CameraManager::Frame()
{
	StartProfile;
	if (activeCamera != ~0u)
	{
		if(cameraData.dirty[activeCamera] != ~0u)// Update the transform
		{
			XMMATRIX transform = XMLoadFloat4x4(&transformManager->dirtyTransforms[cameraData.dirty[activeCamera]]);
			XMVECTOR pos = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);// XMLoadFloat3(&transformManager->positions[tindex]);
		//	auto rotation = XMMatrixRotationRollPitchYawFromVector(XMLoadFloat3(&transformManager->rotations[tindex]));
			XMVECTOR forward = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
			XMVECTOR up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
			pos = XMVector3TransformCoord(pos, transform);
			forward = XMVector3TransformNormal(forward, transform);
			up = XMVector3TransformNormal(up, transform);
			XMVECTOR lookAt = pos + forward;

			XMMATRIX view = XMMatrixLookAtLH(pos, lookAt, up);
			XMStoreFloat4x4(&cameraData.view[activeCamera], view);
			XMMATRIX viewproj = view * XMMatrixPerspectiveFovLH(cameraData.fov[activeCamera], cameraData.aspectRatio[activeCamera], cameraData.nearPlane[activeCamera], cameraData.farPlane[activeCamera]);

			XMFLOAT4X4 viewProjMatrix;
			XMStoreFloat4x4(&viewProjMatrix, viewproj);
			renderer->UpdateView((float*)&viewProjMatrix);

			cameraData.dirty[activeCamera] = ~0u;
		} 

		

	}
	StopProfile;
}

void SE::Core::CameraManager::SetDirty(const Entity & entity, size_t index)
{
	StartProfile;
	auto& find = entityToIndex.find(entity);
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
	newData.fov = (float*)(newData.dirty + newData.allocated);
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
	const Entity& entity = cameraData.entity[index];
	const Entity& last_entity = cameraData.entity[last];

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
	if (activeCamera == last)
		activeCamera = index;

	cameraData.used--;
	StopProfile;
}

void SE::Core::CameraManager::GarbageCollection()
{
	StartProfile;
	uint32_t alive_in_row = 0;
	while (cameraData.used > 0 && alive_in_row < 4U)
	{
		std::uniform_int_distribution<uint32_t> distribution(0U, cameraData.used - 1U);
		uint32_t i = distribution(generator);
		if (entityManager.Alive(cameraData.entity[i]))
		{
			alive_in_row++;
			continue;
		}
		alive_in_row = 0;
		Destroy(i);
	}
	StopProfile;
}
