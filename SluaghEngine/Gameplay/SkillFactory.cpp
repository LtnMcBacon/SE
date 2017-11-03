#include "SkillFactory.h"
#include <Core\IEngine.h>
#include <time.h>
#include <Profiler.h>
#include "CoreInit.h"

SE::Gameplay::SkillFactory::SkillFactory()
{
	auto rm = CoreInit::subSystems.resourceHandler;
	rm->LoadResource("test.si", [this](const Utilz::GUID& guid, void* filePointer, size_t fileSize)
	{
		this->skillAmounts = *((char*)(filePointer));

		if (this->skillAmounts < 0)
		{
			this->skillAmounts = 0;
		}

		return ResourceHandler::InvokeReturn::DecreaseRefcount;;
	});
}

unsigned int SE::Gameplay::SkillFactory::readSkillInfo(std::string& name, unsigned short int* typeList)
{
	StartProfile;
	auto rm = CoreInit::subSystems.resourceHandler;

	SkillInfo* tempSkill;
	tempSkill = new SkillInfo;
	
	unsigned int index;
	rm->LoadResource("test.si", [this, &index, &tempSkill, name](const Utilz::GUID& guid, void* filePointer, size_t fileSize)
	{
		index = CoreInit::subSystems.window->GetRand() % this->skillAmounts;
		int offset = sizeof(int);
		for (int i = 0; i < index; i++)
		{
			unsigned int stringSize;
			memcpy(&stringSize, (char*)filePointer + offset, sizeof(unsigned int));
			offset += stringSize + sizeof(unsigned int);
			

			int usiSize = sizeof(unsigned short int);
			tempSkill->AtkType = *((char*)(filePointer)+offset);
			offset += usiSize;
			tempSkill->Element = *((char*)(filePointer)+offset);
			offset += usiSize;
			tempSkill->Boon = *((char*)(filePointer)+offset);
			offset += usiSize;
			tempSkill->Bane = *((char*)(filePointer)+offset);
			offset += usiSize;
			tempSkill->Animation = *((char*)(filePointer)+offset);
			offset += usiSize;
			tempSkill->Particle = *((char*)(filePointer)+offset);
			offset += usiSize;
		}
		unsigned int stringSize;
		memcpy(&stringSize, (char*)filePointer + offset, sizeof(unsigned int));
		offset += sizeof(unsigned int);
		auto thing = ((char*)(filePointer)+offset);
		tempSkill->skillName = ((std::string)(thing)).substr(0, stringSize);
		offset += stringSize;

		int usiSize = sizeof(unsigned short int);
		tempSkill->AtkType		= *((char*)(filePointer)+offset);
		offset += usiSize;
		tempSkill->Element		= *((char*)(filePointer)+offset);
		offset += usiSize;
		tempSkill->Boon			= *((char*)(filePointer)+offset);
		offset += usiSize;
		tempSkill->Bane			= *((char*)(filePointer)+offset);
		offset += usiSize;
		tempSkill->Animation	= *((char*)(filePointer)+offset);
		offset += usiSize;
		tempSkill->Particle		= *((char*)(filePointer)+offset);

		return ResourceHandler::InvokeReturn::DecreaseRefcount;
	});

	name = tempSkill->skillName;
	typeList[0] = tempSkill->AtkType;
	typeList[1] = tempSkill->Element;
	typeList[2] = tempSkill->Boon;
	typeList[3] = tempSkill->Bane;
	typeList[4] = tempSkill->Animation;
	typeList[5] = tempSkill->Particle;

	delete tempSkill;
	StopProfile;

	return index;
}
void SE::Gameplay::SkillFactory::readAttributesFromFile(unsigned int index, float* attributes)
{
	StartProfile;
	auto rm = CoreInit::subSystems.resourceHandler;

	SkillAttributes* tempSkill;
	tempSkill = new SkillAttributes;

	rm->LoadResource("test.sa", [this, index, &tempSkill](const Utilz::GUID& guid, void* filePointer, size_t fileSize) 
	{
		int offset = index * (sizeof(float) * 7);
		float db = *((char*)(filePointer)+offset);
		memcpy(&tempSkill->skillDamage, (char*)filePointer + offset, sizeof(float));
		offset += sizeof(float);
		memcpy(&tempSkill->boonEffectValue, (char*)filePointer + offset, sizeof(float));
		offset += sizeof(float);
		memcpy(&tempSkill->boonRange, (char*)filePointer + offset, sizeof(float));
		offset += sizeof(float);
		memcpy(&tempSkill->boonDuration, (char*)filePointer + offset, sizeof(float));
		offset += sizeof(float);
		memcpy(&tempSkill->baneEffectValue, (char*)filePointer + offset, sizeof(float));
		offset += sizeof(float);
		memcpy(&tempSkill->baneRange, (char*)filePointer + offset, sizeof(float));
		offset += sizeof(float);
		memcpy(&tempSkill->baneDuration, (char*)filePointer + offset, sizeof(float));
		offset += sizeof(float);

		return ResourceHandler::InvokeReturn::DecreaseRefcount;
	});

 	attributes[0] = tempSkill->skillDamage;
	attributes[1] = tempSkill->boonEffectValue;
	attributes[2] = tempSkill->boonRange;
	attributes[3] = tempSkill->boonDuration;
	attributes[4] = tempSkill->baneEffectValue;
	attributes[5] = tempSkill->baneRange;
	attributes[6] = tempSkill->baneDuration;
	delete[] tempSkill;
	StopProfile;
}

