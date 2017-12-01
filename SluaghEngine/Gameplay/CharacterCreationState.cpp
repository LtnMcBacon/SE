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
	nrOfSkills = 2;
	nrOfPerks = 0;
	selectedSkills = 0;
	renewSkillList = 0;
	fileParser.entityIndex = 0;
	fileParser.ParseFiles("CharacterCreationMenu.HuD");
	fileParser.InitiateTextures();
	//Testing perk importer
	for (auto& fileName: Perkfiles)
	{
		perks.loadPerkData(fileName);
	}
	
	
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
				allSkillsSelected = true;
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
		else if (button.rectName == "skillBackgroundBtn")
		{
			fileParser.GUIButtons.CreateButton(190, 90, button.Width, button.Height, 1, button.rectName, NULL, button.textName, button.hoverTex, button.PressTex);
			fileParser.GUIButtons.CreateButton(570, 90, button.Width, button.Height, 1, "skillBackgroundBtn2", NULL, button.textName, button.hoverTex, button.PressTex);
			fileParser.GUIButtons.CreateButton(950, 90, button.Width, button.Height, 1, "skillBackgroundBtn3", NULL, button.textName, button.hoverTex, button.PressTex);
		}
	}

	fileParser.GUIButtons.DrawButtons();
	importSkillButtons();
	importPerkButtons();


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
		renewSkillList = selectedSkills;
		fileParser.GUIButtons.deleteSkillPerkBtns();
		if (selectedSkills < nrOfSkills)
		{
			getSkills();
		}
		else
		{
			getPerks();
		}
	}
	else if (selectedPerks != renewPerks)
	{
		fileParser.GUIButtons.deleteSkillPerkBtns();
		renewPerks = selectedPerks;
		if (selectedPerks < nrOfPerks)
		{
			getPerks();
		}
	}
	//if (selectedPerks == nrOfPerks)
	//{
	//	fileParser.GUIButtons.DeleteSpecificButtons("skillBackgroundBtn");
	//	fileParser.GUIButtons.DeleteSpecificButtons("skillBackgroundBtn2");
	//	fileParser.GUIButtons.DeleteSpecificButtons("skillBackgroundBtn3");
	//}


	bool pressed = input->ButtonDown(uint32_t(GameInput::ACTION));
	bool released = input->ButtonUp(uint32_t(GameInput::ACTION));
	int mousePosX, mousePosY;
	input->GetMousePos(mousePosX, mousePosY);
	
	
	fileParser.GUIButtons.ButtonHover(mousePosX, mousePosY, pressed, released);

	
	
	
	if (allSkillsSelected  == true)
	{
		
		PlayStateData* infoToPass = new PlayStateData;

		for (int i = 0; i < nrOfSkills; i++)
		{
			SkillFactory sf;
			float attrArray[8];
			sf.readAttributesFromFile(chosenSkillsIndex[i], chosenSkills[i].projectileFileGUID, attrArray);

			infoToPass->skills[i].skillName = chosenSkills.at(i).skillName;
			infoToPass->skills[i].atkType	= chosenSkills.at(i).atkType;
			infoToPass->skills[i].damageType	= chosenSkills.at(i).damageType;
			infoToPass->skills[i].boon		= chosenSkills.at(i).boon;
			infoToPass->skills[i].bane		= chosenSkills.at(i).bane;
			infoToPass->skills[i].animation = chosenSkills.at(i).animation;
			infoToPass->skills[i].particle	= chosenSkills.at(i).particle;
			
			infoToPass->skills[i].projectileFileGUID	= chosenSkills[i].projectileFileGUID;
			infoToPass->skills[i].skillDamage			= attrArray[0];
			infoToPass->skills[i].boonEffectValue		= attrArray[1];
			infoToPass->skills[i].boonRange				= attrArray[2];
			infoToPass->skills[i].boonDuration			= attrArray[3];
			infoToPass->skills[i].baneEffectValue		= attrArray[4];
			infoToPass->skills[i].baneRange				= attrArray[5];
			infoToPass->skills[i].baneDuration			= attrArray[6];
			infoToPass->skills[i].cooldown				= attrArray[7];
		}

		for (size_t i = 0; i < nrOfPerks; i++)
		{
			infoToPass->perks[i] = chosenPerks.at(i);
		}


		passableInfo = infoToPass;
		CurrentState = State::PLAY_STATE;
	}


	ProfileReturn(CurrentState);
}
	
void SE::Gameplay::CharacterCreationState::getSkills()
{
	StartProfile;
	int offset = 380;
	int borderOffset = 200;
	
	SkillFactory sf;
	Skill skill;
	std::vector<int> OtherSkills;
	int nrOfOtherSkills = 0;
	for (size_t i = 0; i < 3; i++)
	{
		int anchorX = borderOffset + offset*i;
		int anchorY = 100;

		std::string skillName;
		std::string skillDesc;
		unsigned short int skillInfo[8];
		
		unsigned int index = sf.getRandomSkillIndex();
		sf.readSkillInfo(index, skillName, skillDesc, skillInfo);

	
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
					sf.readSkillInfo(index, skillName, skillDesc, skillInfo);
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
						sf.readSkillInfo(index, skillName, skillDesc, skillInfo);
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
					sf.readSkillInfo(index, skillName, skillDesc, skillInfo);
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
						sf.readSkillInfo(index, skillName, skillDesc, skillInfo);
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
		skill.skillName	 = skillName;
		skill.atkType	 = static_cast<DamageSources>(skillInfo[0]);
		skill.damageType = static_cast<DamageType>(skillInfo[1]);
		skill.boon		 = static_cast<Boons>(1 << skillInfo[2]);
		skill.bane		 = static_cast<Banes>(1 << skillInfo[3]);
		skill.animation	 = 0;
		skill.particle	 = 0;
	
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
				skillButton.skillName = skillName;
				skillButton.skillIndex = index;
				skillButton.bindButton = skillChoice;

				fileParser.GUIButtons.CreateButton(
					anchorX,
					anchorY,
					skillButton.Width,
					skillButton.Height,
					0.1,
					skillButton.rectName,
					skillChoice,
					skillInfo,
					skillName,
					skillButton.textName,
					skillButton.hoverTex,
					skillButton.PressTex,
					""
					);
				break;
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
	int offset = 380;
	int borderOffset = 200;
	
	
	std::vector<std::string> otherPerks;
	int nrOfOtherPerks = 0;
	
	for (size_t i = 0; i < 3; i++)
	{
		int anchorX = borderOffset + offset*i;
		int anchorY = 100;

		std::string perkName = randomizePerk();
				
		int count = 0;
		int j = 0;
		int p = 0;

		if (nrOfOtherPerks > chosenPerkName.size())
		{
			while (j < nrOfOtherPerks)
			{

				if (perkName == otherPerks[j])
				{
					j = 0;
					p = 0;
					perkName = randomizePerk();
				}
				else
				{
					j++;
				}

				while (p < chosenPerkName.size())
				{
					if (perkName == chosenPerkName[p])
					{
						j = 0;
						p = 0;
						perkName = randomizePerk();
						
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
			while (j < chosenPerkName.size())
			{

				if (perkName == chosenPerkName[j])
				{
					j = 0;
					p = 0;
					perkName = randomizePerk();
					
				}
				else
				{
					j++;
				}

				while (p < nrOfOtherPerks)
				{
					if (perkName == otherPerks[p])
					{
						j = 0;
						p = 0;
						perkName = randomizePerk();
					}
					else
					{
						p++;
					}
				}

			}
		}

		otherPerks.push_back(perkName);
		nrOfOtherPerks++;
		PerkData tempPerk;
		for (size_t i = 0; i < perks.perkVec.size(); i++)
		{
			if (perks.perkVec[i].name == perkName)
			{
				tempPerk = perks.perkVec[i];
				break;
			}
		}
		

		auto perkNameReturn = [this, perkName,tempPerk]()->void
		{
			if (selectedPerks < nrOfPerks)
			{
				this->chosenPerkName.push_back(perkName);
				this->chosenPerks.push_back(tempPerk);
				this->selectedPerks++;
			}
		}; std::function<void()> perkChoice = perkNameReturn;


		for (auto& perkButton : fileParser.perkButtonVec)
		{
			if (perkButton.rectName == perkName)
			{
				perkButton.perkName = perkName;
				perkButton.bindButton = perkChoice;

				fileParser.GUIButtons.CreateButton(
					anchorX,
					anchorY,
					perkButton.Width,
					perkButton.Height,
					0.1,
					perkButton.rectName,
					perkChoice,
					true,
					perkName,
					perkButton.textName,
					perkButton.hoverTex,
					perkButton.PressTex,
					""
				);
				break;

			}
		}
	}
	fileParser.GUIButtons.DrawButtons();
	otherPerks.clear();
	ProfileReturnVoid;


}

std::string SE::Gameplay::CharacterCreationState::randomizePerk()
{
	StartProfile;
	int i = CoreInit::subSystems.window->GetRand() % perks.perkVec.size();
	std::string perkName = perks.perkVec[i].name;
	
	ProfileReturnConst(perkName);
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

void SE::Gameplay::CharacterCreationState::importPerkButtons()
{
	StartProfile;
	for (auto& fileName : perkButtonFiles)
	{
		fileParser.ParsePerks(fileName);
	}
	ProfileReturnVoid;
}

void SE::Gameplay::CharacterCreationState::interpretPerks(PerkData perk)
{

}