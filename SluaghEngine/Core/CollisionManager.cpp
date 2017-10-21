#include "CollisionManager.h"
#include <Profiler.h>
#include <Utilz\Console.h>

#include <Graphics\FileHeaders.h>
#include <Graphics\VertexStructs.h>
using namespace DirectX;
#undef min

SE::Core::CollisionManager::CollisionManager(const InitializationInfo& initInfo) : initInfo(initInfo)
{
	_ASSERT(initInfo.resourceHandler);
	_ASSERT(initInfo.entityManager);
	_ASSERT(initInfo.transformManager);

	initInfo.transformManager->RegisterSetDirty({ this, &CollisionManager::SetDirty });

	Allocate(128);
	AllocateBH(64);


	defaultHierarchy = 0;
	boundingHierarchy.used++;
	initInfo.resourceHandler->LoadResource("Placeholder_Block.mesh", [this](const Utilz::GUID& mesh, void*data, size_t size){
		LoadMesh(defaultHierarchy, data, size);

		return ResourceHandler::InvokeReturn::DecreaseRefcount;
	});


}

SE::Core::CollisionManager::~CollisionManager()
{
	delete collisionData.data;
	delete boundingHierarchy.data;
}


void SE::Core::CollisionManager::CreateBoundingHierarchy(const Entity & entity, const DirectX::XMFLOAT3 & p1, const DirectX::XMFLOAT3 & p2)
{
}

void SE::Core::CollisionManager::CreateBoundingHierarchy(const Entity & entity, const Utilz::GUID & mesh, bool async, ResourceHandler::Behavior behavior)
{
	StartProfile;
	auto find = entityToCollisionData.find(entity);
	if (find == entityToCollisionData.end())
	{
		// Check if the entity is alive
		if (!entityManager.Alive(entity))
			ProfileReturnVoid;

		// Make sure we have enough memory.
		if (collisionData.used + 1 > collisionData.allocated)
			Allocate(collisionData.allocated * 2);

		// Register the entity
		size_t newEntry = collisionData.used++;
		entityToCollisionData[entity] = newEntry;
		collisionData.entity[newEntry] = entity;

		transformManager->Create(entity);

		// Load the mesh
		{
			auto findHi = guidToBoudningIndex.find(mesh); // Is the bounding hierarchy created for this mesh?
			auto& bIndex = guidToBoudningIndex[mesh];
			if (findHi == guidToBoudningIndex.end()) // If not created
			{
				// Make sure we have enough memory.
				if (boundingHierarchy.used + 1 > boundingHierarchy.allocated)
					AllocateBH(boundingHierarchy.allocated*2); // TODO: Make thread safe

				auto cp = bIndex = boundingInfo.size();
				boundingInfo.push_back({ defaultHierarchy }); // Setup the deafult info.
				

				// Register the new hierarchy
				auto newHI = boundingHierarchy.used++;
			//	std::function<int(size_t, void*, size_t)> asd = std::bind(&CollisionManager::LoadMesh, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
				auto res = resourceHandler->LoadResource(mesh, [this,newHI, cp,async](auto mesh, auto data, auto size) ->ResourceHandler::InvokeReturn {
			

					auto res = LoadMesh(newHI, data, size);
					if (res)
						return ResourceHandler::InvokeReturn::Fail;

					if (async)
						toUpdate.push({ newHI, cp });
					else
						boundingInfo[cp].index = newHI;


					return ResourceHandler::InvokeReturn::DecreaseRefcount;
				}, async, behavior);


				if (res)
					Utilz::Console::Print("Could not load mesh for boundingdata. Using default instead.\n");
			
			}

			collisionData.boundingIndex[newEntry] = bIndex;		
			boundingInfo[bIndex].entities.push_back(entity);

			if(!async)
				transformManager->SetAsDirty(entity);

		}


	}
	StopProfile;
}

void SE::Core::CollisionManager::BindOnCollideWithAny(const Entity & entity)
{
	auto& find = entityToCollisionData.find(entity);
	if (find != entityToCollisionData.end())
	{
		collisionData.collisionWithAny[find->second] = true;
	}
}

bool SE::Core::CollisionManager::Pick(const DirectX::XMVECTOR& rayO, const DirectX::XMVECTOR& rayD, Entity& collidedEntity) const
{
	StartProfile;
	DirectX::FXMVECTOR origin = rayO;
	DirectX::FXMVECTOR dir = rayD;
	float minDistance = D3D11_FLOAT32_MAX;
	int collisionWith = -1;
	for(int i = 0; i < collisionData.used; i++)
	{
		auto& sphere = collisionData.sphereWorld[i];
		float distance;
		if(sphere.Intersects(rayO,rayD,distance))
		{
			auto& AABB = collisionData.AABBWorld[i];
			float distBox;
			if(AABB.Intersects(origin, dir, distBox))
			{
				float d = std::min(distance, distBox);
				if(d < minDistance)
				{
					minDistance = d;
					collisionWith = i;
				}
			}
		}
	}
	if(collisionWith >= 0)
	{
		collidedEntity = collisionData.entity[collisionWith];
	}
	ProfileReturn(collisionWith >= 0);
}

void SE::Core::CollisionManager::Frame(Utilz::TimeCluster* timer)
{
	StartProfile;
	timer->Start("CollisionManager");
	GarbageCollection();

	{
		// First update all bounding data
		for (auto& dirty : dirtyEntites)
		{
			// TODO: Multithread
			auto& mySphere = boundingHierarchy.sphere[boundingInfo[collisionData.boundingIndex[dirty.myIndex]].index];
			auto& myAABB = boundingHierarchy.AABB[boundingInfo[collisionData.boundingIndex[dirty.myIndex]].index];
			XMMATRIX myTransform = XMLoadFloat4x4(&transformManager->GetCleanedTransforms()[dirty.transformIndex]);
			mySphere.Transform(collisionData.sphereWorld[dirty.myIndex], myTransform);
			myAABB.Transform(collisionData.AABBWorld[dirty.myIndex], myTransform);
		}
	}

	{
		// Now check if any of the newly updated entities collided with anything
		for (auto& dirty : dirtyEntites)
		{
			if (collisionData.collisionWithAny[dirty.myIndex])
			{
				auto& mySphere = collisionData.sphereWorld[dirty.myIndex]; // Already transformed
				for (size_t i = 0; i < collisionData.used; i++)
				{
					auto& otherSphere = collisionData.sphereWorld[i]; // Already transformed
					if (dirty.myIndex != i  &&  mySphere.Intersects(otherSphere))
					{
						auto& otherAABB = collisionData.AABBWorld[i]; // Already transformed
						auto& myAABB = collisionData.AABBWorld[dirty.myIndex]; // Already transformed
						if(myAABB.Intersects(otherAABB))
							collideWithAny(collisionData.entity[dirty.myIndex], collisionData.entity[i]);
					}
				}


			}
		}
	}
	dirtyEntites.clear();

	// See if any new boundingHierarchies have finished loading. (We do this last because the transform manager need to update the transform.
	while (!toUpdate.wasEmpty())
	{
		auto& t = toUpdate.top();

		boundingInfo[t.index].index = t.boundingHierarchyIndex;
		for (auto& e : boundingInfo[t.index].entities)
			transformManager->SetAsDirty(e);

		toUpdate.pop();
	}

	timer->Stop("CollisionManager");
	StopProfile;
}

bool SE::Core::CollisionManager::GetLocalBoundingBox(const Entity& entity, DirectX::BoundingBox* bb)
{
	StartProfile;
	if (!bb)
		ProfileReturnConst(false);
	auto find = entityToCollisionData.find(entity);
	if (find == entityToCollisionData.end())
		ProfileReturnConst(false);

	const size_t bIndex = collisionData.boundingIndex[find->second];
	const size_t aabbIndex = boundingInfo[bIndex].index;
	*bb = boundingHierarchy.AABB[aabbIndex];
	return true;

}

void SE::Core::CollisionManager::SetDirty(const Entity & entity, size_t index)
{
	StartProfile;
	auto& find = entityToCollisionData.find(entity);
	if (find != entityToCollisionData.end())
	{
		dirtyEntites.push_back({ index, find->second });
	}
	StopProfile;
}

void SE::Core::CollisionManager::Allocate(size_t size)
{
	StartProfile;
	_ASSERT(size > collisionData.allocated);

	// Allocate new memory
	CollisionData newData;
	newData.allocated = size;
	newData.data = operator new(size * CollisionData::size);
	memset(newData.data, 0, size * CollisionData::size);
	newData.used = collisionData.used;

	// Setup the new pointers
	newData.entity = (Entity*)newData.data;
	newData.boundingIndex = (size_t*)(newData.entity + newData.allocated);
	newData.sphereWorld = (BoundingSphere*)(newData.boundingIndex + newData.allocated);
	newData.AABBWorld = (BoundingBox*)(newData.sphereWorld + newData.allocated);
	newData.collisionWithAny = (uint8_t*)(newData.AABBWorld + newData.allocated);


	// Copy data
	memcpy(newData.entity, collisionData.entity, collisionData.used * sizeof(Entity));
	memcpy(newData.boundingIndex, collisionData.boundingIndex, collisionData.used * sizeof(size_t));
	memcpy(newData.sphereWorld, collisionData.sphereWorld, collisionData.used * sizeof(BoundingSphere));
	memcpy(newData.AABBWorld, collisionData.AABBWorld, collisionData.used * sizeof(BoundingBox));
	memcpy(newData.collisionWithAny, collisionData.collisionWithAny, collisionData.used * sizeof(uint8_t));

	// Delete old data;
	operator delete(collisionData.data);
	collisionData = newData;

	StopProfile;
}

void SE::Core::CollisionManager::Destroy(size_t index)
{
	StartProfile;

	// Temp variables
	size_t last = collisionData.used - 1;
	const Entity entity = collisionData.entity[index];
	const Entity last_entity = collisionData.entity[last];

	boundingInfo[collisionData.boundingIndex[index]].entities.remove(entity); // Refcount


	// Copy the data
	collisionData.entity[index] = last_entity;
	collisionData.boundingIndex[index] = collisionData.boundingIndex[last];
	collisionData.collisionWithAny[index] = collisionData.collisionWithAny[last];

	// Replace the index for the last_entity 
	entityToCollisionData[last_entity] = index;
	entityToCollisionData.erase(entity);

	collisionData.used--;
	StopProfile;
}

void SE::Core::CollisionManager::Destroy(const Entity & entity)
{
}

void SE::Core::CollisionManager::GarbageCollection()
{
	StartProfile;
	uint32_t alive_in_row = 0;
	while (collisionData.used > 0 && alive_in_row < 4U)
	{
		std::uniform_int_distribution<uint32_t> distribution(0U, collisionData.used - 1U);
		uint32_t i = distribution(generator);
		if (entityManager.Alive(collisionData.entity[i]))
		{
			alive_in_row++;
			continue;
		}
		alive_in_row = 0;
		Destroy(i);
	}
	StopProfile;
}

void SE::Core::CollisionManager::AllocateBH(size_t size)
{
	StartProfile;
	_ASSERT(size > boundingHierarchy.allocated);

	// Allocate new memory
	BoundingHierarchy newData;
	newData.allocated = size;
	newData.data = operator new(size * BoundingHierarchy::size);
	newData.used = collisionData.used;

	// Setup the new pointers
	//newData.box = (BoundingOrientedBox*)(newData.refCount + boundingHierarchy.allocated);
	newData.sphere = (BoundingSphere*)newData.data;
	newData.AABB = (BoundingBox*)(newData.sphere + newData.allocated);
	

	// Copy data
	//memcpy(newData.box, boundingHierarchy.box, boundingHierarchy.used * sizeof(BoundingOrientedBox));
	memcpy(newData.sphere, boundingHierarchy.sphere, boundingHierarchy.used * sizeof(BoundingSphere));
	memcpy(newData.AABB, boundingHierarchy.AABB, boundingHierarchy.used * sizeof(BoundingBox));

	// Delete old data;
	operator delete(boundingHierarchy.data);
	boundingHierarchy = newData;

	StopProfile;
}

void SE::Core::CollisionManager::DestroyBH(size_t index)
{
}


int SE::Core::CollisionManager::LoadMesh(size_t newHI, void * data, size_t size)
{
	StartProfile;

	auto meshHeader = (Graphics::Mesh_Header*)data;

	if (meshHeader->vertexLayout == 0) {

		Vertex* v = (Vertex*)(meshHeader + 1);
		CreateBoundingHierarchy(newHI, v, meshHeader->nrOfVertices, sizeof(Vertex));
	}

	else {

		VertexDeformer* v = (VertexDeformer*)(meshHeader + 1);
		CreateBoundingHierarchy(newHI, v, meshHeader->nrOfVertices, sizeof(VertexDeformer));
	}

	ProfileReturnConst(0);
}

void SE::Core::CollisionManager::CreateBoundingHierarchy(size_t index, void * data, size_t numVertices, size_t stride)
{
	BoundingBox::CreateFromPoints(boundingHierarchy.AABB[index], numVertices, (XMFLOAT3*)data, stride);
	BoundingSphere::CreateFromBoundingBox(boundingHierarchy.sphere[index], boundingHierarchy.AABB[index]);
	//BoundingOrientedBox::CreateFromPoints(boundingHierarchy.box[index], numVertices, (XMFLOAT3*)data, stride);
	
}
