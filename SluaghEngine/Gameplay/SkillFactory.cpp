#include "SkillFactory.h"
#include <Core\IEngine.h>
#include <time.h>
#include <Profiler.h>
#include "CoreInit.h"

SE::Gameplay::SkillFactory::SkillFactory()
{
	auto rm = CoreInit::subSystems.resourceHandler;
	rm->LoadResource("testSkill.si", [this](const Utilz::GUID& guid, void* filePointer, size_t fileSize)
	{
		this->skillAmounts = *((char*)(filePointer));

		if (this->skillAmounts < 0)
		{
			this->skillAmounts = 0;
		}

		return ResourceHandler::InvokeReturn::SUCCESS | ResourceHandler::InvokeReturn::DEC_RAM;
	});
}

unsigned int SE::Gameplay::SkillFactory::readSkillInfo(std::string& name, unsigned short int* typeList)
{
	StartProfile;
	auto rm = CoreInit::subSystems.resourceHandler;

	SkillInfo* tempSkill;
	tempSkill = new SkillInfo;
	
	unsigned int index;
	rm->LoadResource("testSkill.si", [this, &index, &tempSkill, name](const Utilz::GUID& guid, void* filePointer, size_t fileSize)
	{
		index = CoreInit::subSystems.window->GetRand() % this->skillAmounts;
		int offset = sizeof(int);
		int usiSize = sizeof(unsigned short int);
		int uiSize = sizeof(unsigned int);
		for (int i = 0; i < index; i++)
		{
			unsigned int stringSize;
			memcpy(&stringSize, (char*)filePointer + offset, uiSize);
			offset += stringSize + uiSize;
			
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
		memcpy(&stringSize, (char*)filePointer + offset, uiSize);
		offset += uiSize;
		auto thing = ((char*)(filePointer)+offset);
		tempSkill->skillName = ((std::string)(thing)).substr(0, stringSize);
		offset += stringSize;

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

		return ResourceHandler::InvokeReturn::SUCCESS | ResourceHandler::InvokeReturn::DEC_RAM;
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
void SE::Gameplay::SkillFactory::readAttributesFromFile(unsigned int index, SE::Utilz::GUID &projectileReference, float* attributes)
{
	StartProfile;
	auto rm = CoreInit::subSystems.resourceHandler;

	SkillAttributes* tempSkill;
	tempSkill = new SkillAttributes;

	int res = rm->LoadResource("testSkill.sa", [this, index, &tempSkill](const Utilz::GUID& guid, void* filePointer, size_t fileSize) 
	{
		int offset = 0;
		unsigned int stringSize;
		int usiSize = sizeof(unsigned short int);
		int uiSize  = sizeof(unsigned int);
		int fSize   = sizeof(float);
		for (int i = 0; i < index; i++)
		{
			memcpy(&stringSize, (char*)filePointer + offset, uiSize);
			offset += stringSize + uiSize * (fSize * 8);
		}
		memcpy(&stringSize, (char*)filePointer + offset, uiSize);
		offset += uiSize;
		auto thing = ((char*)(filePointer)+offset);
		tempSkill->projectileReference = ((std::string)(thing)).substr(0, stringSize);
		offset += stringSize;

		memcpy(&tempSkill->skillDamage, (char*)filePointer + offset, fSize);
		offset += fSize;
		memcpy(&tempSkill->boonEffectValue, (char*)filePointer + offset, fSize);
		offset += fSize;
		memcpy(&tempSkill->boonRange, (char*)filePointer + offset, fSize);
		offset += fSize;
		memcpy(&tempSkill->boonDuration, (char*)filePointer + offset, fSize);
		offset += fSize;
		memcpy(&tempSkill->baneEffectValue, (char*)filePointer + offset, fSize);
		offset += fSize;
		memcpy(&tempSkill->baneRange, (char*)filePointer + offset, fSize);
		offset += fSize;
		memcpy(&tempSkill->baneDuration, (char*)filePointer + offset, fSize);
		offset += fSize;
		memcpy(&tempSkill->cooldown, (char*)filePointer + offset, fSize);
		offset += fSize;

		return ResourceHandler::InvokeReturn::SUCCESS | ResourceHandler::InvokeReturn::DEC_RAM;
	});

	projectileReference = tempSkill->projectileReference + ".SEP";
 	attributes[0] = tempSkill->skillDamage;
	attributes[1] = tempSkill->boonEffectValue;
	attributes[2] = tempSkill->boonRange;
	attributes[3] = tempSkill->boonDuration;
	attributes[4] = tempSkill->baneEffectValue;
	attributes[5] = tempSkill->baneRange;
	attributes[6] = tempSkill->baneDuration;
	attributes[7] = tempSkill->cooldown;

	delete tempSkill;
	StopProfile;
}

