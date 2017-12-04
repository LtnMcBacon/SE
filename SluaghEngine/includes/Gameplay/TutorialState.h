#ifndef SE_GAMEPLAY_TUTORIAL_STATE_H_
#define SE_GAMEPLAY_TUTORIAL_STATE_H_
#include "IGameState.h"
#include "ProjectileManager.h"
namespace SE::Gameplay
{
	class Room;
	class PlayerUnit;
	class TutorialState : public IGameState
	{
	public:
		TutorialState();
		~TutorialState();

		State Update(void*&)override;

	private:
		void NothingScript(float dt);
		void NoneScript(float dt);
		void GreetingScript(float dt);
		void WASDScript(float dt);
		void MouseScript(float dt);
		void BraJobbatMovementScript(float dt);
		void GlimmerPickupWeaponScript(float dt);
		void SpawnPickupWeaponScript(float dt);
		void PickupWeaponScript(float dt);
		void Utm‰rktPickupWeaponScript(float dt);
		void VisaTaPÂSigVapenScript(float dt);
		void TaPÂSigVapenScript(float dt);


		void BytaVapenAddInitScript(float dt);
		void BytaVapenAddScript(float dt);


		int hugg = 0;
		void SlÂMedVapenInitScript(float dt);
		void SlÂMedVapenScript(float dt);



		void SpawnaTr‰ningsDockaScript(float dt);
		
		void TestaSpellInitScript(float dt);
		void TestaSpellScript(float dt);


		void SpawnaFiendeScript(float dt);
		void SpawnaGlastigScript(float dt);
		void SpawnaBodachScript(float dt);
		void SpawnaNuckelaveeScript(float dt);
		void SpawnAndScript(float dt);

		void GÂTillSluaghSvartScript(float dt);
		void EndTutorialScript(float dt);

		bool w = false;
		bool a = false;
		bool s = false;
		bool d = false;


		void WaitForMovementInputScript(float dt);

		void NonSuspiciousScript(float dt);

		void(TutorialState::*scriptToRun)(float);


		Room* room;
		PlayerUnit* player;
		ProjectileManager* projectileManager;

		bool scriptStarted = false;
		struct SkillIndicator
		{
			Core::Entity frame;
			Core::Entity Image;
			float currentCooldown = 0;
			float maxCooldown;
		}skillIndicators[2];
	};
}


#endif