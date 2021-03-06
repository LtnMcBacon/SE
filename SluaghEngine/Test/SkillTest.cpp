#include <Core\IEngine.h>
#include <Gameplay\SkillFactory.h>
#include <Gameplay\PlayerUnit.h>
#include "SkillTest.h"
#include <iostream>
#include <vector>
#include <Gameplay\Game.h>
#include "../Gameplay/CoreInit.h"

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

bool SE::Test::SkillTest::Run(SE::DevConsole::IConsole* console)
{
	auto engine = Core::CreateEngine();
	engine->Init();
	Gameplay::CoreInit::Init(engine);

	SE::Gameplay::SkillFactory SF;

	SE::Gameplay::Skill skill;

	unsigned short int tempEnumHolder[6];
	float floatValues[8];

	unsigned int index = SF.getRandomSkillIndex();
	SF.readSkillInfo(index, skill.skillName, skill.skillDesc, tempEnumHolder);
	SF.readAttributesFromFile(index, skill.projectileFileGUID, floatValues);

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
	std::cout << floatValues[4] << std::endl;
	std::cout << floatValues[5] << std::endl;
	std::cout << floatValues[6] << std::endl;
	std::cout << floatValues[7] << std::endl;



	engine->Release();
	delete engine;
	return true;
}