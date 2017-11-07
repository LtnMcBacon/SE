#include"LightManager.h"
#include <Profiler.h>

using namespace DirectX;



SE::Core::LightManager::LightManager(const InitializationInfo & initInfo) :initInfo(initInfo)
{
	_ASSERT(initInfo.renderer);
	_ASSERT(initInfo.entityManager);
	_ASSERT(initInfo.transformManager);

	initInfo.transformManager->RegisterSetDirty({ this, &LightManager::UpdateDirtyPos });

	auto result = initInfo.renderer->GetPipelineHandler()->CreateDepthStencilViewCube("DepthCube", 512, 512, true);
	if (result < 0)
		throw std::exception("Failed to create depth stencil cube.");
}

SE::Core::LightManager::~LightManager()
{
}

void SE::Core::LightManager::Create(const Entity & entity, const CreateInfo & info)
{
	StartProfile;
	// Check if the entity is alive
	if (!initInfo.entityManager->Alive(entity))
		ProfileReturnVoid;

	// chexk if entity exist
	const auto fileLoaded = entityToLightData.find(entity);
	auto& data = entityToLightData[entity];
	if (fileLoaded == entityToLightData.end())
	{
		data.visible = false;
		data.colour = { info.color.x, info.color.y, info.color.z, 1.0f };
		data.pos = { info.pos.x, info.pos.y, info.pos.z, info.radius };
		data.dir = { info.dir.x, info.dir.y, info.dir.z, 1.0f };
		data.castShadow = info.castShadow;

		// If entity is shadow caster, push back to shadow casters
		if (data.castShadow == true) {

			DirectX::XMMATRIX proj = DirectX::XMMatrixPerspectiveFovLH(2.3, 1, 0.1, info.radius);

			shadowCasters.push_back({ entity, proj, info.dir});
		}

		initInfo.transformManager->Create(entity, info.pos);

		indexToEntity.push_back(entity);
	}
	ProfileReturnVoid;
}

void SE::Core::LightManager::ToggleLight(const Entity & entity, bool show)
{
	StartProfile;
	// Check if the entity is alive
	if (!initInfo.entityManager->Alive(entity))
		ProfileReturnVoid;

	// chexk if entity exist in text 
	const auto fileLoaded = entityToLightData.find(entity);
	if (fileLoaded != entityToLightData.end())
	{		
		if (show && !fileLoaded->second.visible)
		{
			anyTogglesThisFrame = true;
		}
		else if (!show && fileLoaded->second.visible)
		{
			anyTogglesThisFrame = true;
		}
		fileLoaded->second.visible = show;
	}
	ProfileReturnVoid;
}

void SE::Core::LightManager::Frame(Utilz::TimeCluster * timer)
{
	StartProfile;
	timer->Start(CREATE_ID_HASH("LightManger"));
	GarbageCollection();

	if (anyTogglesThisFrame)
	{
		initInfo.renderer->GetPipelineHandler()->MapConstantBuffer("LightDataBuffer", [this](auto data) {
			auto& cb = *reinterpret_cast<LightDataBuffer*>(data);
			uint32_t count = 0;
			for (auto& l : entityToLightData)
			{
				if (l.second.visible)
				{
					cb.data[count].colour = l.second.colour;
					cb.data[count].pos = l.second.pos;
					cb.data[count].castShadow[0] = l.second.castShadow;
					count++;
				}
				if (count == 20)
					break;
			}
			cb.size[0] = count;

		});

		anyTogglesThisFrame = false;
	}

	if (shadowCasters.size() > 0) {

		auto& k = shadowCasters[0];
		auto pos = initInfo.transformManager->GetPosition(k.entity);

		XMVECTOR newDir = { 0.0f, -1.0f, 1.0f, 1.0f };
		initInfo.transformManager->SetForward(k.entity, newDir);

		auto dir = initInfo.transformManager->GetForward(k.entity);
		auto right = initInfo.transformManager->GetRight(k.entity);

		auto vDir = XMLoadFloat3(&dir);
		auto vRight = XMLoadFloat3(&right);

		auto vUp = XMVector3Cross(vDir, vRight);

		XMMATRIX view = XMMatrixLookToLH(XMLoadFloat3(&pos), vDir, vUp);

		view = view * k.lProj;

		view = XMMatrixTranspose(view);

		XMFLOAT4X4 viewProj;
		XMStoreFloat4x4(&viewProj, view);

		initInfo.renderer->GetPipelineHandler()->UpdateConstantBuffer("LightViewProj", &viewProj, sizeof(XMFLOAT4X4));
	}
	initInfo.renderer->GetPipelineHandler()->UpdateConstantBuffer("LightVPBuffer", &lightVPBuffer, sizeof(lightVPBuffer));
	initInfo.renderer->GetPipelineHandler()->UpdateConstantBuffer("LightShadowDataBuffer", &lightShadowDataBuffer, sizeof(lightShadowDataBuffer));
	timer->Stop(CREATE_ID_HASH("LightManger"));
	StopProfile;
}
		


void SE::Core::LightManager::GarbageCollection()
{
	StartProfile;
	uint32_t alive_in_row = 0;

	while (entityToLightData.size() > 0 && alive_in_row < 4U)
	{
		std::uniform_int_distribution<uint32_t> distribution(0U, entityToLightData.size() - 1U);
		uint32_t i = distribution(generator);
		if (initInfo.entityManager->Alive(indexToEntity[i]))
		{
			alive_in_row++;
			continue;
		}
		alive_in_row = 0;	
		Destroy(i);
	}
	StopProfile;
}

void SE::Core::LightManager::SetShadowCaster(const Entity& entity)
{
	const auto exists = entityToLightData.find(entity);
	if (exists == entityToLightData.end())
		return;

	const DirectX::XMMATRIX proj = DirectX::XMMatrixPerspectiveFovLH(DirectX::XM_PIDIV2, 1, 0.1f, 20.0f);
	const DirectX::XMVECTOR looks[] = { { -1.0f, 0.0f, 0.0f, 0.0f },
										{ 1.0f, 0.0f, 0.0f, 0.0f },
										{ 0.0f, 1.0f, 0.0f, 0.0f },
										{ 0.0f, -1.0f, 0.0f, 0.0f },
										{ 0.0f, 0.0f, 1.0f, 0.0f },
										{ 0.0f, 0.0f, -1.0f, 0.0f } };
	const DirectX::XMVECTOR ups[] = { { 0.0f, 1.0f, 0.0f, 0.0f },
									{ 1.0f, 1.0f, 0.0f, 0.0f },
									{ 0.0f, 0.0f, -1.0f, 0.0f },
									{ 0.0f, 0.0f, 1.0f, 0.0f },
									{ 0.0f, 1.0f, 0.0f, 0.0f },
									{ 0.0f, 1.0f, 0.0f, 0.0f } };
	DirectX::XMFLOAT3 lpos = initInfo.transformManager->GetPosition(exists->first);
	const DirectX::XMVECTOR vpos = DirectX::XMLoadFloat3(&lpos);
	for(int i = 0; i < 6; ++i)
	{
		const DirectX::XMMATRIX view = DirectX::XMMatrixLookToLH(vpos, looks[i], ups[i]);
		DirectX::XMStoreFloat4x4(&lightVPBuffer.viewProjections[i], DirectX::XMMatrixTranspose(view * proj));
	}

	lightShadowDataBuffer.position = DirectX::XMFLOAT4(lpos.x, lpos.y, lpos.z, 1.0f);
	lightShadowDataBuffer.range = { 20.0f, 0.0f, 0.0f, 0.0f };

}

void SE::Core::LightManager::Destroy(size_t index)
{
	StartProfile;
	// Temp variables
	const size_t last = entityToLightData.size() - 1;
	const Entity entity = indexToEntity[index];
	const Entity last_entity = indexToEntity[last];

	// Copy the data
	indexToEntity[index] = last_entity;
	entityToLightData[last_entity] = entityToLightData[entity];

	// Remove last spot 
	entityToLightData.erase(entity);
	indexToEntity.pop_back();

	StopProfile;
}

void SE::Core::LightManager::Destroy(const Entity & entity)
{
}

void SE::Core::LightManager::UpdateDirtyPos(const Entity& entity, size_t index)
{
	StartProfile;
	const auto find = entityToLightData.find(entity);
	if (find != entityToLightData.end())
	{
		dirtyEntites.push_back({ index, entity });
	}
	ProfileReturnVoid;
}
