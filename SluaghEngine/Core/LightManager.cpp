#include"LightManager.h"
#include <Profiler.h>

using namespace DirectX;



SE::Core::LightManager::LightManager(const InitializationInfo & initInfo) :initInfo(initInfo)
{
	_ASSERT(initInfo.renderer);
	_ASSERT(initInfo.entityManager);
	_ASSERT(initInfo.transformManager);
	_ASSERT(initInfo.eventManager);

	initInfo.eventManager->RegisterToToggleVisible({ this, &LightManager::ToggleLight });
	initInfo.transformManager->RegisterSetDirty({ this, &LightManager::UpdateDirtyPos });
	
	const auto result = initInfo.renderer->GetPipelineHandler()->CreateDepthStencilViewCube("DepthCube", 512, 512, true);
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
	if (fileLoaded == entityToLightData.end())
	{
		auto& light = entityToLightData[entity];
		light.visible = false;
		light.colour = { info.color.x, info.color.y, info.color.z, info.intensity };
		light.pos = { info.pos.x, info.pos.y, info.pos.z, info.radius };
		light.castShadow = false;

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
		anyTogglesThisFrame = true;
		initInfo.transformManager->SetAsDirty(entity);
		fileLoaded->second.visible = show;
	}
	ProfileReturnVoid;
}

void SE::Core::LightManager::Frame(Utilz::TimeCluster * timer)
{
	StartProfile;
	timer->Start(("LightManager"));
	GarbageCollection();

	if (dirtyEntites.size())
	{
		for (auto& l : dirtyEntites)
		{
			const auto fl = entityToLightData.find(l.entity);
			if (fl != entityToLightData.end())
			{
				const auto pos = initInfo.transformManager->GetPosition(l.entity);
				fl->second.pos.x = pos.x;
				fl->second.pos.y = pos.y;
				fl->second.pos.z = pos.z;
			}
		}
		anyTogglesThisFrame = true;
	}
	dirtyEntites.clear();

	
	if (anyTogglesThisFrame)
	{
		initInfo.renderer->GetPipelineHandler()->MapConstantBuffer("LightDataBuffer", [this](auto data) {
			auto& cb = *reinterpret_cast<LightDataBuffer*>(data);
			uint32_t count = 0;
			uint32_t shadowCasterIndex = MAX_LIGHTS;
			for (auto& l : entityToLightData)
			{
				if (l.second.visible)
				{
					cb.data[count].colour = l.second.colour;
					cb.data[count].pos = l.second.pos;
					if (hasShadowCaster && l.first == shadowCaster)
						shadowCasterIndex = count;
					count++;
				}
				if (count == MAX_LIGHTS)
					break;
			}
			cb.size[0] = count;
			cb.size[1] = shadowCasterIndex;

		});

		anyTogglesThisFrame = false;
	}

	if (hasShadowCaster) {
		const DirectX::XMMATRIX proj = DirectX::XMMatrixPerspectiveFovLH(DirectX::XM_PIDIV2, 1, 0.1f, entityToLightData[shadowCaster].pos.w);
		const DirectX::XMVECTOR looks[] = { 
		{ 1.0f, 0.0f, 0.0f, 0.0f },
		{ -1.0f, 0.0f, 0.0f, 0.0f },
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
		DirectX::XMFLOAT3 lpos = initInfo.transformManager->GetPosition(shadowCaster);
		const DirectX::XMVECTOR vpos = DirectX::XMLoadFloat3(&lpos);
		for (int i = 0; i < 6; ++i)
		{
			const DirectX::XMMATRIX view = DirectX::XMMatrixLookToLH(vpos, looks[i], ups[i]);
			DirectX::XMStoreFloat4x4(&lightVPBuffer.viewProjections[i], DirectX::XMMatrixTranspose(view * proj));
		}
		lightShadowDataBuffer.position = DirectX::XMFLOAT4(lpos.x, lpos.y, lpos.z, entityToLightData[shadowCaster].pos.w);
		lightShadowDataBuffer.range = { entityToLightData[shadowCaster].pos.w, 0.0f, 0.0f, 0.0f };

		initInfo.renderer->GetPipelineHandler()->UpdateConstantBuffer("LightVPBuffer", &lightVPBuffer, sizeof(lightVPBuffer));
		initInfo.renderer->GetPipelineHandler()->UpdateConstantBuffer("LightShadowDataBuffer", &lightShadowDataBuffer, sizeof(lightShadowDataBuffer));
	}
	timer->Stop(("LightManager"));
	StopProfile;
}
		


void SE::Core::LightManager::GarbageCollection()
{
	StartProfile;
	uint32_t alive_in_row = 0;

	while (entityToLightData.size() > 0 && alive_in_row < 4U)
	{
		const std::uniform_int_distribution<uint32_t> distribution(0U, entityToLightData.size() - 1U);
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

	hasShadowCaster = true;
	shadowCaster = entity;


}

void SE::Core::LightManager::SetColor(const Entity& entity, float rgb[3])
{
	const auto exists = entityToLightData.find(entity);
	if (exists == entityToLightData.end())
		return;

	exists->second.colour.x = rgb[0];
	exists->second.colour.y = rgb[1];
	exists->second.colour.z = rgb[2];
}

void SE::Core::LightManager::Destroy(size_t index)
{
	StartProfile;
	// Temp variables
	const size_t last = entityToLightData.size() - 1;
	const Entity entity = indexToEntity[index];
	const Entity last_entity = indexToEntity[last];

	if (entity == shadowCaster)
		hasShadowCaster = false;

	// Copy the data
	indexToEntity[index] = last_entity;

	// Remove last spot 
	entityToLightData.erase(entity);
	indexToEntity.pop_back();

	StopProfile;
}

void SE::Core::LightManager::Destroy(const Entity & entity)
{
	int32_t index = -1;
	for(int32_t i = 0; i < indexToEntity.size(); i++)
	{
		if (indexToEntity[i] == entity)
		{
			index = i;
			break;
		}
	}
	if (index != -1)
		Destroy(index);

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
