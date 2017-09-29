#include <CollisionManager.h>
#include <Profiler.h>
#include <Utilz\Console.h>
#include <OBJParser\Parsers.h>

using namespace DirectX;

SE::Core::CollisionManager::CollisionManager(ResourceHandler::IResourceHandler * resourceHandler, const EntityManager & entityManager, TransformManager * transformManager)
	: resourceHandler(resourceHandler), entityManager(entityManager), transformManager(transformManager)
{
	_ASSERT(resourceHandler);
	_ASSERT(transformManager);

	transformManager->SetDirty.Add<CollisionManager, &CollisionManager::SetDirty>(this);

	defaultHierarchy = 0;
	Allocate(128);
	AllocateBH(64);
}

SE::Core::CollisionManager::~CollisionManager()
{
	for(size_t i = 0; i < collisionData.used; i++)
		collisionData.collisionWithAnyCallback[i].~Delegate();
	delete collisionData.data;
	delete boundingHierarchy.data;
}


void SE::Core::CollisionManager::CreateBoundingHierarchy(const Entity & entity, const DirectX::XMFLOAT3 & p1, const DirectX::XMFLOAT3 & p2)
{
}

void SE::Core::CollisionManager::CreateBoundingHierarchy(const Entity & entity, const Utilz::GUID & mesh)
{
	StartProfile;
	auto& find = entityToCollisionData.find(entity);
	if (find == entityToCollisionData.end())
	{
		// Check if the entity is alive
		if (!entityManager.Alive(entity))
			ProfileReturnVoid;

		// Make sure we have enough memory.
		if (collisionData.used + 1 > collisionData.allocated)
			Allocate(collisionData.allocated * 2);

		// Register the entity
		size_t newEntry = collisionData.used;
		entityToCollisionData[entity] = newEntry;
		collisionData.entity[newEntry] = entity;
		collisionData.used++;

		// Load the mesh
		{
			auto& findHi = guidToBoundingIndex.find(mesh);
			auto& bIndex = guidToBoundingIndex[mesh];
			if (findHi == guidToBoundingIndex.end())
			{
				// Make sure we have enough memory.
				if (boundingHierarchy.used + 1 > boundingHierarchy.allocated)
					AllocateBH(boundingHierarchy.allocated + 10); // TODO: Make thread safe


				boundingIndex.push_back({ defaultHierarchy, 0 });
				bIndex = boundingIndex.size() - 1;

				// Register the new hierarchy
				size_t newHI = boundingHierarchy.used;
				bIndex = defaultHierarchy;

				auto res = resourceHandler->LoadResource(mesh, ResourceHandler::LoadResourceDelegate::Make<CollisionManager, &CollisionManager::LoadMesh>(this));

				if (res)
					Utilz::Console::Print("Could not load mesh for boundingdata. Using default instead.\n");
			
			}

			collisionData.boundingIndex[newEntry] = bIndex;
			boundingIndex[bIndex].refCount++;
			

			

		}


	}
	StopProfile;
}

void SE::Core::CollisionManager::BindOnCollideWithAny(const Entity & entity, const CollideCallbackDelegate & callback)
{
	auto& find = entityToCollisionData.find(entity);
	if (find != entityToCollisionData.end())
	{
		collisionData.collisionWithAny[find->second] = true;
		collisionData.collisionWithAnyCallback[find->second] = callback;
	}
}

bool SE::Core::CollisionManager::PickEntity(const Entity & entity, const DirectX::XMVECTOR & rayO, const DirectX::XMVECTOR & rayD, float * distance)
{
	StartProfile;
	DirectX::FXMVECTOR origin = rayO;
	DirectX::FXMVECTOR direction = rayD;

	auto& find = entityToCollisionData.find(entity);
	if (find != entityToCollisionData.end())
	{
		auto& sphere = collisionData.sphereWorld[find->second];
		auto sphereColCheck = sphere.Intersects(origin, direction, *distance);
		if (sphereColCheck)
		{
			auto& AABox = collisionData.AABBWorld[find->second];
			auto BBColCheck = AABox.Intersects(rayO, rayD, *distance);
			if (BBColCheck)
			{
				ProfileReturnConst(true)
			}
			else
			{
				ProfileReturnConst(false)
			}
		}
		else
		{
			ProfileReturnConst(false)
		}


	}	
	ProfileReturnConst(false)
}

void SE::Core::CollisionManager::Frame()
{
	StartProfile;
	// First update all bounding data
	for (auto& dirty : dirtyEntites)
	{
		// TODO: Multithread
		auto& mySphere = boundingHierarchy.sphere[boundingIndex[collisionData.boundingIndex[dirty.myIndex]].index];
		auto& myAABB = boundingHierarchy.AABB[boundingIndex[collisionData.boundingIndex[dirty.myIndex]].index];
		XMMATRIX myTransform = XMLoadFloat4x4(&transformManager->dirtyTransforms[dirty.transformIndex]);
		mySphere.Transform(collisionData.sphereWorld[dirty.myIndex], myTransform);	
		myAABB.Transform(collisionData.AABBWorld[dirty.myIndex], myTransform);
	}
	// Now check if any of the newly updated entities collided with anything
	for (auto& dirty : dirtyEntites)
	{
		if (collisionData.collisionWithAny[dirty.myIndex])
		{
			auto& mySphere = collisionData.sphereWorld[dirty.myIndex]; // Already transformed
			for (size_t i = 0; i < collisionData.used; i++)
			{
				auto& otherSphere = collisionData.sphereWorld[i]; // Already transformed
				if (dirty.myIndex != i  &&  mySphere.Intersects(otherSphere) )
				{
					collisionData.collisionWithAnyCallback[dirty.myIndex](collisionData.entity[dirty.myIndex], collisionData.entity[i]);
				}
			}


		}
	}
	dirtyEntites.clear();
	StopProfile;
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
	newData.collisionWithAnyCallback = (CollideCallbackDelegate*)(newData.collisionWithAny + newData.allocated);


	// Copy data
	memcpy(newData.entity, collisionData.entity, collisionData.used * sizeof(Entity));
	memcpy(newData.boundingIndex, collisionData.boundingIndex, collisionData.used * sizeof(size_t));
	memcpy(newData.sphereWorld, collisionData.sphereWorld, collisionData.used * sizeof(BoundingSphere));
	memcpy(newData.AABBWorld, collisionData.AABBWorld, collisionData.used * sizeof(BoundingBox));
	memcpy(newData.collisionWithAny, collisionData.collisionWithAny, collisionData.used * sizeof(uint8_t));
	memcpy(newData.collisionWithAnyCallback, collisionData.collisionWithAnyCallback, collisionData.used * sizeof(CollideCallbackDelegate));

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
	const Entity& entity = collisionData.entity[index];
	const Entity& last_entity = collisionData.entity[last];

	// Copy the data
	collisionData.entity[index] = last_entity;
	collisionData.boundingIndex[index] = collisionData.boundingIndex[last];
	collisionData.collisionWithAny[index] = collisionData.collisionWithAny[last];
	collisionData.collisionWithAnyCallback[index] = collisionData.collisionWithAnyCallback[last];
	// Replace the index for the last_entity 
	entityToCollisionData[last_entity] = index;
	entityToCollisionData.erase(entity);

	collisionData.used--;
	StopProfile;
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

int SE::Core::CollisionManager::LoadMesh(const Utilz::GUID & guid, void * data, size_t size)
{
	StartProfile;
	ArfData::Data arfData;
	ArfData::DataPointers arfp;
	auto r = Arf::ParseObj(data, size, &arfData, &arfp);
	if (r)
		ProfileReturnConst(r);
	Arf::Mesh::Data* parsedData;
	size_t parsedSize;
	r = Arf::Interleave(arfData, arfp, &parsedData, &parsedSize, Arf::Mesh::InterleaveOption::Position);
	if (r)
		ProfileReturnConst(r);

	delete arfp.buffer;

	auto newHI = guidToBoundingHierarchy[guid];

	CreateBoundingHierarchy(newHI, parsedData->vertices, parsedData->NumVertices, sizeof(XMFLOAT3));

	delete parsedData;

	auto bIndex = guidToBoundingIndex[guid];

	boundingIndex[bIndex].index = newHI;



	ProfileReturnConst(0);
}

void SE::Core::CollisionManager::CreateBoundingHierarchy(size_t index, void * data, size_t numVertices, size_t stride)
{
	BoundingBox::CreateFromPoints(boundingHierarchy.AABB[index], numVertices, (XMFLOAT3*)data, stride);
	BoundingSphere::CreateFromBoundingBox(boundingHierarchy.sphere[index], boundingHierarchy.AABB[index]);
	//BoundingOrientedBox::CreateFromPoints(boundingHierarchy.box[index], numVertices, (XMFLOAT3*)data, stride);
	
}
