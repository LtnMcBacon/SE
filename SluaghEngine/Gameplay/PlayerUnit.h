#ifndef _SE_GAMEPLAY_PLAYER_UNIT_H_
#define _SE_GAMEPLAY_PLAYER_UNIT_H_

#include "GameUnit.h"

namespace SE
{
	namespace Gameplay
	{
		/**
		*
		* @brief The player class
		*
		* @details This class contains the update functions and logic for the player in Sluagh.
		* It inherits from GameUnit.
		*
		* @warning Only a simple version at the moment
		*
		* @sa GameUnit
		*
		**/
		class PlayerUnit : public GameUnit
		{
		private:

			/**
			* @brief	Resolve the events that has been added this frame.
			*
			* @details	Go through all the events that has been added to the
			* PlayerUnit this frame.
			*
			* @retval void No return value
			*
			* @warning Not implemented at all yet! Will be called from the "Update" function
			*
			*/
			void ResolveEvents();

			/**
			* @brief	Hinder collision during movement
			*
			* @details	This function checks if the units new position after a move will cause a collision.
			* If a move will cause a collision in x- or y-direction, then that direction-component will be nulled
			* before further computation is done.
			*
			* @param [in] dt Delta time for this frame
			* @param [in/out] xMov The x-component of the direction
			* @param [in/out] yMov The y-component of the direction
			*
			* @retval True Collision occoured
			* @retval False Collision didn't occour
			*
			*/
			bool CorrectCollision(float dt, float &xMov, float &yMov);

		public:

			struct MovementInput
			{
				bool downW;
				bool downA;
				bool downS;
				bool downD;

				float mousePosX;
				float mousePosY;

				MovementInput(bool w, bool a, bool s, bool d, float mouseX, float mouseY)
				{
					downW = w;
					downA = a;
					downS = s;
					downD = d;
					mousePosX = mouseX;
					mousePosY = mouseY;
				}
			};

			/**
			* @brief	Update the players movement
			*
			* @details	This function updates the position of the player and checks so that it is a legal position,
			* if not it tries to retain as much of the movement as possible
			*
			* @param [in] dt Delta time for this frame
			* @param [in] MovementInput input data
			*
			* @retval void No value
			*
			*/
			void UpdateMovement(float dt, const MovementInput& inputs);

			/**
			* @brief To be documented
			*/
			void AddForce(float force[2]);

			/**
			* @brief	Update the map
			*
			* @param [in] dt Delta time for this frame
			*
			* @retval void No value
			*
			*/
			void UpdateMap(const char** mapForRoom);

		private:
			PlayerUnit() {};
			PlayerUnit(const PlayerUnit& other) = delete;
			PlayerUnit(const PlayerUnit&& other) = delete;
			PlayerUnit& operator=(const PlayerUnit& rhs) = delete;

			const char** map = nullptr;
			float forcesToApply[2] = {};
			float extends = 0.25f; /*HARDCODED RIGHT NOW!*/

		public:
			
			PlayerUnit(void* skills, void* perks, float xPos, float yPos, const char** mapForRoom);
			~PlayerUnit();
		};

	}
}




#endif
