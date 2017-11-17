#include "CharacterCreationState.h"
#include <Profiler.h>
using namespace SE;
using namespace Gameplay;


CharacterCreationState::CharacterCreationState()
{

}
CharacterCreationState::CharacterCreationState(Window::IWindow * Input)
{
	this->input = Input;
}
CharacterCreationState::~CharacterCreationState()
{

}
IGameState::State CharacterCreationState::Update(void* &passableInfo)
{
	StartProfile;

	IGameState::State empty = State::CHARACTER_CREATION_STATE;
	
	PlayStateData* infoToPass = new PlayStateData;

	for (int i = 0; i < 2; i++)
	{
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
		infoToPass->skills[i].coolDown = 1.0f;
		infoToPass->skills[i].element = DamageTypes::DAMAGE_TYPE_PHYSICAL;
		infoToPass->skills[i].particle = 0;

		if(i == 0)
			infoToPass->skills[i].projectileFileGUID = "turretProjectile.SEP";
		if(i == 1)
			infoToPass->skills[i].projectileFileGUID = "dnaProjectiles.SEP";

		infoToPass->skills[i].skillDamage = 5;
		infoToPass->skills[i].skillName = "skill1";
	}
	passableInfo = infoToPass;


	empty = State::PLAY_STATE;
	
	ProfileReturn(empty);
}