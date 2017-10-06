#include "..\Gameplay\SkillFactory.h"
#include "SkillTest.h"
#include <iostream>
#include <vector>

SE::Test::SkillTest::SkillTest()
{

}
SE::Test::SkillTest::~SkillTest()
{

}

template <typename Enumeration>
auto as_integer(Enumeration const value)
	-> typename std::underlying_type<Enumeration>::type
{
	return static_cast<typename std::underlying_type<Enumeration>::type>(value);
}

bool SE::Test::SkillTest::Run(SE::Utilz::IConsoleBackend* console)
{
	struct Skill
	{
		std::string skillName = "";
		enum class AtkType { SELFCAST, MELEE, RANGED, AREA };
		enum class Element { NEUTRAL, MAGIC, FIRE, WATER, NATURE };
		enum class Boon { NONE, DAMAGE, KNOCKBACK, STUN, LIFESTEAL, HEAL, PROTECTION, RESISTANCE, CASTSPEED, SWIFTNESS, SLOW, INVULNERABILITY };
		enum class Bane { NONE, DAMAGE, STUN, BLOODLETTING, UNCOVER, WEAKNESS, SLOW };
		enum class Animation { NONE, SWIPE, SHOOT }; // CAST
		enum class Particle { NONE, FIRE, SMOKE, SPARKLE };

		AtkType atkType = AtkType::MELEE;
		Element element = Element::NEUTRAL;
		Boon boon = Boon::NONE;
		Bane bane = Bane::NONE;
		Animation animation = Animation::SWIPE;
		Particle particle = Particle::NONE;

		float skillDamage = 0.f;
		float effectValue = 0.f;
		float range = 0.f;
		float duration = 0.f;
	} skill;
	SE::Gameplay::SkillFactory SF;

	unsigned short int tempEnumHolder[6];
	float floatValues[4];

	unsigned int index = SF.characterCreationRetriveSkill(skill.skillName, tempEnumHolder);
	SF.readAttributesFromFile(0, floatValues);

	std::cout << skill.skillName << std::endl;
	std::cout << tempEnumHolder[0] << std::endl;
	std::cout << tempEnumHolder[1] << std::endl;
	std::cout << tempEnumHolder[2] << std::endl;
	std::cout << tempEnumHolder[3] << std::endl;
	std::cout << tempEnumHolder[4] << std::endl;
	std::cout << tempEnumHolder[5] << std::endl;

	std::cout << floatValues[0] << std::endl;
	std::cout << floatValues[1] << std::endl;
	std::cout << floatValues[2] << std::endl;
	std::cout << floatValues[3] << std::endl;

	getchar();

	return false;
}