#include "CharacterCreationState.h"
#include <Profiler.h>
#include "CoreInit.h"
#include <SkillFactory.h>
#include <PlayerUnit.h>
#include <Skill.h>

using namespace SE;
using namespace Gameplay;


CharacterCreationState::CharacterCreationState()
{

}
CharacterCreationState::CharacterCreationState(Window::IWindow * Input)
{
	StartProfile;
	nrOfSkills = 3;
	selectedSkills = 0;
	renewSkillList = 0;
	fileParser.entityIndex = 0;
	fileParser.ParseFiles("CharacterCreationMenu.HuD");
	fileParser.InitiateTextures();
	//Testing perk importer
	std::string tempPath[2];
	tempPath[0] = "fuckYou.prk";
	tempPath[1] = "testPerkData.prk";
	perks.loadPerkData(tempPath, 2);
	//****************************
	auto returnToMain = [this]()->void
	{
		this->CurrentState = State::MAIN_MENU_STATE;
	}; std::function<void()> back = returnToMain;

	auto startGame = [this]()->void
	{
		if (selectedSkills == nrOfSkills)
		{
			if (selectedPerks == nrOfPerks)
			{
				this->CurrentState = State::PLAY_STATE;
			}
		}
		
	}; std::function<void()> begin = startGame;


	for (auto& button : fileParser.ButtonVector)
	{
		if (button.rectName == "backBtn")
		{
			fileParser.GUIButtons.CreateButton(button.PositionX, button.PositionY, button.Width, button.Height, button.layerDepth, button.rectName, back, button.textName, button.hoverTex, button.PressTex);
		}
		else if (button.rectName == "confirmBtn")
		{
			fileParser.GUIButtons.CreateButton(button.PositionX, button.PositionY, button.Width, button.Height, button.layerDepth, button.rectName, begin, button.textName, button.hoverTex, button.PressTex);
		}
		else if (button.rectName == "skillDescription")
		{
			fileParser.GUIButtons.CreateButton(button.PositionX, button.PositionY, button.Width, button.Height, button.layerDepth, button.rectName, back, button.textName, button.hoverTex, button.PressTex);
		}
	}
	fileParser.GUIButtons.DrawButtons();

	importSkillButtons();

	getSkills();
	this->input = Input;
	ProfileReturnVoid;
}
CharacterCreationState::~CharacterCreationState()
{
	StartProfile;
	fileParser.GUIButtons.DeleteButtons();
	ProfileReturnVoid;
	
}
IGameState::State CharacterCreationState::Update(void* &passableInfo)
{
	StartProfile;

	if (selectedSkills != renewSkillList)
	{
		fileParser.GUIButtons.deleteSkillPerkBtns();
		getSkills();
		renewSkillList = selectedSkills;
	}
	if (selectedSkills == nrOfSkills && allSkillsSelected == false)
	{
		allSkillsSelected = true;
		fileParser.GUIButtons.deleteSkillPerkBtns();
		getPerks();
	}

	bool pressed = input->ButtonDown(uint32_t(GameInput::ACTION));
	bool released = input->ButtonUp(uint32_t(GameInput::ACTION));
	int mousePosX, mousePosY;
	input->GetMousePos(mousePosX, mousePosY);
	
	
	fileParser.GUIButtons.ButtonHover(mousePosX, mousePosY, pressed, released);

	if (input->ButtonPressed(0))
	IGameState::State empty = State::CHARACTER_CREATION_STATE;
	
	PlayStateData* infoToPass = new PlayStateData;
	if (selectedSkills  == true)
	{

		for (int i = 0; i < 3; i++)
		{
			/*infoToPass->skills[i].skillName = chosenSkills.at(i).skillName;
			infoToPass->skills[i].atkType	= chosenSkills.at(i).atkType;
			infoToPass->skills[i].element	= chosenSkills.at(i).element;
			infoToPass->skills[i].boon		= chosenSkills.at(i).boon;
			infoToPass->skills[i].bane		= chosenSkills.at(i).bane;
			infoToPass->skills[i].animation = chosenSkills.at(i).animation;
			infoToPass->skills[i].particle	= chosenSkills.at(i).particle;
			
			infoToPass->skills[i].projectileFileGUID	= chosenSkills.at(i).projectileFileGUID;
			infoToPass->skills[i].skillDamage			= chosenSkills.at(i).skillDamage;
			infoToPass->skills[i].boonEffectValue		= chosenSkills.at(i).boonEffectValue;
			infoToPass->skills[i].boonRange				= chosenSkills.at(i).boonRange;
			infoToPass->skills[i].boonDuration			= chosenSkills.at(i).boonDuration;
			infoToPass->skills[i].baneEffectValue		= chosenSkills.at(i).baneEffectValue;
			infoToPass->skills[i].baneRange				= chosenSkills.at(i).baneRange;
			infoToPass->skills[i].baneDuration			= chosenSkills.at(i).baneDuration;
			infoToPass->skills[i].coolDown				= chosenSkills.at(i).coolDown;*/

			infoToPass->skills[i].animation = 0;
			infoToPass->skills[i].atkType = DamageSources::DAMAGE_SOURCE_RANGED;
			infoToPass->skills[i].bane = Banes::CONDITIONAL_BANES_NONE;
			infoToPass->skills[i].baneDuration = 0;
			infoToPass->skills[i].baneEffectValue = 0;
			infoToPass->skills[i].baneRange = 0;
			infoToPass->skills[i].boon = Boons::CONDITIONAL_BOONS_NONE;
			infoToPass->skills[i].boonDuration = 0;
			infoToPass->skills[i].boonEffectValue = 0;
			infoToPass->skills[i].boonRange = 0;
			infoToPass->skills[i].cooldown = 1.0f;
			infoToPass->skills[i].damageType = DamageType::PHYSICAL;
			infoToPass->skills[i].particle = 0;

			if(i == 0)
				infoToPass->skills[i].projectileFileGUID = "turretProjectile.SEP";
			if(i == 1)
				infoToPass->skills[i].projectileFileGUID = "FlameWave.SEP";

			infoToPass->skills[i].skillDamage = 5;
			infoToPass->skills[i].skillName = "skill1";
		}


		passableInfo = infoToPass;
	}


	ProfileReturn(CurrentState);
}
	
void SE::Gameplay::CharacterCreationState::getSkills()
{
	StartProfile;
	int offset = 300;
	int borderOffset = 200;
	int rectSize = 100;
	SkillFactory sf;
	Skill skill;
	std::vector<int> OtherSkills;
	int nrOfOtherSkills = 0;
	for (size_t i = 0; i < nrOfSkills; i++)
	{
		int anchorX = borderOffset + offset*i;
		int anchorY = 100;

		std::string skillName;
		unsigned short int skillInfo[8];
		
		unsigned int index = sf.getRandomSkillIndex();
		sf.readSkillInfo(index, skillName, skillInfo);

	
		int count = 0;
		int j = 0;
		int p = 0;
		
		if (nrOfOtherSkills > chosenSkillsIndex.size())
		{
			while (j < nrOfOtherSkills)
			{

				if (index == OtherSkills[j])
				{
					j = 0;
					p = 0;
					index = sf.getRandomSkillIndex();
					sf.readSkillInfo(index, skillName, skillInfo);
				}
				else
				{
					j++;
				}

				while (p < chosenSkillsIndex.size())
				{
					if (index == chosenSkillsIndex[p])
					{
						j = 0;
						p = 0;
						index = sf.getRandomSkillIndex();
						sf.readSkillInfo(index, skillName, skillInfo);
					}
					else
					{
						p++;
					}
				}

			}
		}
		else
		{
			while (j < chosenSkillsIndex.size())
			{

				if (index == chosenSkillsIndex[j])
				{
					j = 0;
					p = 0;
					index = sf.getRandomSkillIndex();
					sf.readSkillInfo(index, skillName, skillInfo);
				}
				else
				{
					j++;
				}

				while (p < nrOfOtherSkills)
				{
					if (index == OtherSkills[p])
					{
						j = 0;
						p = 0;
						index = sf.getRandomSkillIndex();
						sf.readSkillInfo(index, skillName, skillInfo);
					}
					else
					{
						p++;
					}
				}

			}
		}		
	
		OtherSkills.push_back(index);
		nrOfOtherSkills++;
		skill.skillName = skillName;
		skill.atkType	= static_cast<DamageSources>(skillInfo[0]);
		skill.damageType	= static_cast<DamageType>(skillInfo[1]);
		skill.boon		= static_cast<Boons>(skillInfo[2]);
		skill.bane		= static_cast<Banes>(skillInfo[3]);
		skill.animation = 0;
		skill.particle	= 0;
	
		auto SkillIndexReturn = [this, index, skill]()->void
		{
			if (selectedSkills < nrOfSkills)
			{
				this->chosenSkillsIndex.push_back(index);
				this->chosenSkills.push_back(skill);
				this->selectedSkills++;
			}
		}; std::function<void()> skillChoice = SkillIndexReturn;


		for (auto& skillButton: fileParser.skillButtonVec)
		{
			if (skillButton.rectName == skillName)
			{
				skillButton.skillIndex = index;
				skillButton.bindButton = skillChoice;

				fileParser.GUIButtons.CreateButton(
					anchorX,
					anchorY,
					skillButton.Width,
					skillButton.Height,
					0,
					skillButton.rectName,
					SkillIndexReturn,
					skillInfo,
					skillButton.textName,
					skillButton.hoverTex,
					skillButton.PressTex,
					""
					);
							
			}
		}
	}
	fileParser.GUIButtons.DrawButtons();
	OtherSkills.clear();
	ProfileReturnVoid;
}

void SE::Gameplay::CharacterCreationState::getPerks()
{
	StartProfile;
	int offset = 225;
	int borderOffset = 200;
	int rectSize = 100;


}

void SE::Gameplay::CharacterCreationState::importSkillButtons()
{
	StartProfile;
	for (auto& fileName : skillButtonFiles)
	{
		fileParser.ParseSkillButtons(fileName);
	}
	ProfileReturnVoid;
}
