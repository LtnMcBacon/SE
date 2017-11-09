#include "CharacterCreationState.h"
#include <Profiler.h>
#include "CoreInit.h"
#include <SkillFactory.h>
#include <PlayerUnit.h>
using namespace SE;
using namespace Gameplay;


CharacterCreationState::CharacterCreationState()
{

}
CharacterCreationState::CharacterCreationState(Window::IWindow * Input)
{
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


	for (auto& button : fileParser.ButtonVector)
	{
		if (button.rectName == "backBtn")
		{
			fileParser.GUIButtons.CreateButton(button.PositionX, button.PositionY, button.Width, button.Height, button.layerDepth, button.rectName, back, button.textName, button.hoverTex, button.PressTex, "Back");
		}
		else if (button.rectName == "confirmBtn")
		{
			fileParser.GUIButtons.CreateButton(button.PositionX, button.PositionY, button.Width, button.Height, button.layerDepth, button.rectName, back, button.textName, button.hoverTex, button.PressTex, "Confirm");
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
}
CharacterCreationState::~CharacterCreationState()
{
	fileParser.GUIButtons.DeleteButtons();
	
}
IGameState::State CharacterCreationState::Update(void* &passableInfo)
{
	StartProfile;

	if (selectedSkills != 0)
	{
		
		getSkills();
		renewSkillList = selectedSkills;
	}

	bool pressed = input->ButtonDown(uint32_t(GameInput::ACTION));
	bool released = input->ButtonUp(uint32_t(GameInput::ACTION));
	int mousePosX, mousePosY;
	input->GetMousePos(mousePosX, mousePosY);
	
	
	fileParser.GUIButtons.ButtonHover(mousePosX, mousePosY, pressed, released);
	
	


	if (input->ButtonPressed(0))
	{
		CurrentState = State::MAIN_MENU_STATE;
	}
		

	/*Code*/

	

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
	
	int offset = 225;
	int borderOffset = 200;
	int rectSize = 100;
	SkillFactory sf;
	PlayerUnit::Skill s1;
	std::vector<int> OtherSkills;
	int nrOfOtherSkills = 0;
	for (size_t i = 0; i < nrOfSkills; i++)
	{

		int anchorX = borderOffset + offset*i;
		int anchorY = 100;

		std::string skillName = "Fireball";
		unsigned short int skillInfo1[7] = {1,2,3,4,5,6,7};
		unsigned int indexPLaceHolder = 2;
		//unsigned int index = sf.readSkillInfo(skillName1, skillInfo1);

	
		int count = 0;
		int j = 0;
		int p = 0;
		
		if (nrOfOtherSkills > chosenSkills.size())
		{
			while (j < nrOfOtherSkills)
			{

				if (indexPLaceHolder == OtherSkills[j])
				{
					j = 0;
					p = 0;
					//index = sf.readSkillInfo(skillName1, skillInfo1);
				}
				else
				{
					j++;
				}

				while (p < chosenSkills.size())
				{
					if (indexPLaceHolder == chosenSkills[p])
					{
						j = 0;
						p = 0;
						//index = sf.readSkillInfo(skillName1, skillInfo1);
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
			while (j < chosenSkills.size())
			{

				if (indexPLaceHolder == chosenSkills[j])
				{
					j = 0;
					p = 0;
					//index = sf.readSkillInfo(skillName1, skillInfo1);
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
						//index = sf.readSkillInfo(skillName1, skillInfo1);
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
		//s1.skillName = skillName1;
		//s1.atkType = static_cast<DamageSources>(skillInfo1[0]);
		//s1.element = static_cast<DamageTypes>(skillInfo1[1]);
		//s1.boon = static_cast<Boons>(skillInfo1[2]);
		//s1.bane = static_cast<Banes>(skillInfo1[3]);
		//s1.animation = 0;
		//s1.particle = 0;
	
		auto SkillIndexReturn = [this, indexPLaceHolder]()->void
		{
			if (selectedSkills < nrOfSkills)
			{
				this->chosenSkills.push_back(indexPLaceHolder);
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
					skillInfo1,
					skillButton.textName,
					skillButton.hoverTex,
					skillButton.PressTex,
					""
					);
			}
		}
	


		//PlayerUnit::addPlayerSkills(s1);
	}
	fileParser.GUIButtons.DrawButtons();
	OtherSkills.clear();

}

void SE::Gameplay::CharacterCreationState::importSkillButtons()
{

	for (auto& fileName : skillButtonFiles)
	{
		fileParser.ParseSkillButtons(fileName);
	}
}
