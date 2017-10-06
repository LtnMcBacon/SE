#include "SkillFactory.h"
#include <Core\Engine.h>
#include <time.h>

unsigned int SE::Gameplay::SkillFactory::characterCreationRetriveSkill(std::string name, unsigned short int* typeList)
{
	srand(time(NULL));
	unsigned int index = SKILL_AMOUNT - 1;//rand() & 3 + 1;

	readSkillInfo(index, name, typeList);

	return index;
}
void characterCreationRandomizeSkill(std::string name, unsigned short int* typeList)
{
	
}
void SE::Gameplay::SkillFactory::readSkillInfo(unsigned int index, std::string name, unsigned short int* typeList)
{
	auto rm = Core::Engine::GetInstance().GetResourceHandler();
	
	struct SkillInfo* tempSkill;
	
	rm->LoadResource("SkillInfo.CP", [this, index, &tempSkill](const Utilz::GUID& guid, void* filePointer, size_t fileSize) { 
		tempSkill = ((SkillInfo*)(filePointer) + index);
		return 0;
	});

	typeList[0] = tempSkill->AtkType;
	typeList[1] = tempSkill->Element;
	typeList[2] = tempSkill->Boon;
	typeList[3] = tempSkill->Bane;
	typeList[4] = tempSkill->Animation;
	typeList[5] = tempSkill->Particle;
}
void SE::Gameplay::SkillFactory::readAttributesFromFile(unsigned int index, float* attributes)
{
	auto rm = Core::Engine::GetInstance().GetResourceHandler();

	struct SkillAttributes* tempSkill;

	rm->LoadResource("SkillAttrebutes.CP", [this, index, &tempSkill](const Utilz::GUID& guid, void* filePointer, size_t fileSize) {
		tempSkill = ((SkillAttributes*)(filePointer) + index);
		return 0;
	});

	attributes[0] = tempSkill->skillDamage;
	attributes[1] = tempSkill->effectValue;
	attributes[2] = tempSkill->range;
	attributes[3] = tempSkill->duration;
}

