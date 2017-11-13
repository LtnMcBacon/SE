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
	nrOfSkills = 1;
	selectedSkills = 0;
	renewSkillList = 0;
	fileParser.entityIndex = 0;
	fileParser.ParseFiles("CharacterCreationMenu.HuD");
	fileParser.InitiateTextures();

	auto returnToMain = [this]()->void
	{
		this->CurrentState = State::MAIN_MENU_STATE;
	}; std::function<void()> back = returnToMain;

	auto startGame = [this]()->void
	{	
		if (selectedSkills == nrOfSkills)
		{
			allSkillsSelected = true;
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
			infoToPass->skills[i].skillName = chosenSkills.at(i).skillName;
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
			infoToPass->skills[i].coolDown				= chosenSkills.at(i).coolDown;

		}
		passableInfo = infoToPass;
	}


	ProfileReturn(CurrentState);
}
	


//void CharacterCreationState::SkillBtns(int nrOfSkills)
//{
//	int width = 1280;
//	int height = 720;
//	int offset = 225;
//	int borderOffset = 200;
//	int rectSize = 100;
//	SkillSpots = new anchorPos[nrOfSkills];
//
//	while (borderOffset + offset*nrOfSkills > width - rectSize && borderOffset > 0 && offset > 0)
//	{
//		borderOffset -= 10;
//		offset -= 10;
//	}
//
//	for (size_t i = 0; i < nrOfSkills; i++)
//	{
//		
//		SkillSpots[i].x = (borderOffset + offset*i);
//		SkillSpots[i].y = 100;
//	}
//	Skillindex = new int[nrOfSkills];
//
//	for (size_t i = 0; i < nrOfSkills; i++)
//	{
//		int randomIndex = CoreInit::subSystems.window->GetRand() % fileParser.skillButtonVec.size();
//		int count = 0;
//		while ( count < nrOfSkills)
//		{
//			if (randomIndex == Skillindex[count])
//			{
//				randomIndex = CoreInit::subSystems.window->GetRand() % fileParser.skillButtonVec.size();
//				int count = 0;
//			}
//			else
//			{
//				count++;
//			}
//		}
//		Skillindex[i] = randomIndex;
//	}
//
//	for (size_t k = 0; k < nrOfSkills; k++)
//	{
//		fileParser.skillButtonVec[Skillindex[k]].PositionX = SkillSpots[k].x;
//		fileParser.skillButtonVec[Skillindex[k]].PositionY = SkillSpots[k].y;
//	}
//
//	fileParser.InitiateSkillsNperks(Skillindex, nrOfSkills, true);
//
//}
//void CharacterCreationState::PerkBtns(int nrOfPerks)
//{
//	int width = 1280;
//	int height = 720;
//	int offset = 225;
//	int borderOffset = 200;
//	int rectSize = 100;
//	perkSpots = new anchorPos[nrOfPerks];
//
//	while (borderOffset + offset*nrOfPerks > width - rectSize && borderOffset > 0 && offset > 0)
//	{
//		borderOffset -= 10;
//		offset -= 10;
//	}
//
//	for (size_t i = 0; i < nrOfPerks; i++)
//	{
//
//		perkSpots[i].x = (borderOffset + offset*i);
//		perkSpots[i].y = 400;
//	}
//	perkIndex = new int[nrOfPerks];
//
//	for (size_t i = 0; i < nrOfPerks; i++)
//	{
//		int randomIndex = CoreInit::subSystems.window->GetRand() % fileParser.skillButtonVec.size();
//		int count = 0;
//		while (count < nrOfPerks)
//		{
//			if (randomIndex == perkIndex[count])
//			{
//				randomIndex =  CoreInit::subSystems.window->GetRand() % fileParser.perkButtonVec.size();
//				int count = 0;
//			}
//			else
//			{
//				count++;
//			}
//		}
//		perkIndex[i] = randomIndex;
//	}
//
//	for (size_t k = 0; k < nrOfPerks; k++)
//	{
//		fileParser.perkButtonVec[perkIndex[k]].PositionX = perkSpots[k].x;
//		fileParser.perkButtonVec[perkIndex[k]].PositionY = perkSpots[k].y;
//	}
//
//	//	draws the selected buttons on the screen
//	fileParser.InitiateSkillsNperks(perkIndex, nrOfPerks, false);
//}

void SE::Gameplay::CharacterCreationState::getSkills()
{
	StartProfile;
	int offset = 225;
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
		unsigned int indexPLaceHolder = 2;
		unsigned int index = sf.getRandomSkillIndex();
		sf.readSkillInfo(index, skillName, skillInfo);

	
		int count = 0;
		int j = 0;
		int p = 0;
		
		if (nrOfOtherSkills > chosenSkillsIndex.size())
		{
			while (j < nrOfOtherSkills)
			{

				if (indexPLaceHolder == OtherSkills[j])
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
					if (indexPLaceHolder == chosenSkillsIndex[p])
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

				if (indexPLaceHolder == chosenSkillsIndex[j])
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
					if (indexPLaceHolder == OtherSkills[p])
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
	
		OtherSkills.push_back(indexPLaceHolder);
		nrOfOtherSkills++;
		skill.skillName = skillName;
		skill.atkType	= static_cast<DamageSources>(skillInfo[0]);
		skill.element	= static_cast<DamageTypes>(skillInfo[1]);
		skill.boon		= static_cast<Boons>(skillInfo[2]);
		skill.bane		= static_cast<Banes>(skillInfo[3]);
		skill.animation = 0;
		skill.particle	= 0;
	
		auto SkillIndexReturn = [this, indexPLaceHolder, skill]()->void
		{
			if (selectedSkills < nrOfSkills)
			{
				this->chosenSkillsIndex.push_back(indexPLaceHolder);
				this->chosenSkills.push_back(skill);
				this->selectedSkills++;
			}
		}; std::function<void()> skillChoice = SkillIndexReturn;


		for (auto& skillButton: fileParser.skillButtonVec)
		{
			if (skillButton.rectName == skillName)
			{
				skillButton.skillIndex = indexPLaceHolder;
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

void SE::Gameplay::CharacterCreationState::importSkillButtons()
{
	StartProfile;
	for (auto& fileName : skillButtonFiles)
	{
		fileParser.ParseSkillButtons(fileName);
	}
	ProfileReturnVoid;
}
