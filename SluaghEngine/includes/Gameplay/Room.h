#ifndef _SE_GAMEPLAY_ROOM_H_
#define _SE_GAMEPLAY_ROOM_H_
#include <vector>
#include "EnemyUnit.h"
#include "FlowField.h"

namespace SE
{
	namespace Gameplay
	{
		/**
		*
		* @brief The class definint the rooms as used by Gameplay
		*
		* @details The Room class is used to save the representation of a room as seen and used in game. This representation consists of:
		* A flowfield map (Pointing to the player or the (last known) exist closest to the player's position.
		* A map representation for the room
		* The enemies in the room
		* A list of adjacent rooms
		*
		* @warning Not fully implemented! A lot of the functionality simply isn't there yet! A shell to start out from.
		*
		* @sa Here the enemy class and algorithms to create the room will be linked
		*
		**/
		class Room
		{
		private:
			Room* adjacentRooms[4] = {};
			char map[25][25];
			std::vector<EnemyUnit*> enemyEntities;
			FlowField* roomField;
			/*Needed:
			 * Representation of the room module(s) that build the room
			 * The enemies that are represented in the room
			 * FlowField map and calculations
			 * Function(s) to build the room
			 */
		public:
			/**
			*
			* @brief DirectionToAdjacentRoom is used to define the direction of an adjacent room in the Room class.
			*
			* @details DirectionToAdjacentRoom is used to define the direction of an adjacent room in the Room class.
			*
			* @warning Note that only four directions are allowed!
			*
			* @sa Read the warning at ReverseDirection before modifying!
			*
			**/
			enum class DirectionToAdjacentRoom
			{

				DIRECTION_ADJACENT_ROOM_NORTH,	/**<The room lies to the North (0) */
				DIRECTION_ADJACENT_ROOM_EAST,	/**<The room lies to the East (1) */
				DIRECTION_ADJACENT_ROOM_SOUTH,	/**<The room lies to the South (2) */
				DIRECTION_ADJACENT_ROOM_WEST	/**<The room lies to the West (3) */
			};
		private:

			/**
			* @brief	Update the Flowfield of a room, given a point that should be used for attraction.
			*
			* @details	Update the Flowfield of a certain room (Always the "current" room!), given a point (character position) that should be used
			* as center of attraction.
			*
			* This function is used once a frame (may be modified!, update this section to reflect that in that case) to make sure that the enemies
			* can move towards the player.
			*
			* @param[in] playerX The x position of the player
			* @param[in] playerY The y position of the player
			*
			* @retval void No return value
			*
			* @warning Due to this function not being implemented ("Cell/Point" not defined), the param[in] macro
			* will not work and thus hasn't been documented!
			*
			* Example code:
			* @code
			*	To be written
			* @endcode
			*/
			void UpdateFlowField(float playerX, float playerY);

			/**
			* @brief	Update the Flowfield of a room, given the direction of the exit that should be used as point for the flowfield.
			*
			* @details	This function is used to update the flow field of adjacent rooms to the current room. This function should be called
			* ONCE, when the player enters a new room and this room becomes the new "current".
			*
			* See UpdateFlowField for more information.
			*
			* @param[in] DirectionToAdjacentRoom The direction that the room to be UPDATED stores the CURRENT room in (the reverse from current room)
			*
			* @retval void No return value
			*
			* @warning Please note that the REVERSED direction is used. This is because the exit that will be used as a point for flowfield calculation
			* exists in the context of the room that the flowfield is calculated in.
			*
			* Example code:
			* @code
			*	To be written
			* @endcode
			*/
			void UpdateFlowField(DirectionToAdjacentRoom exit);

			/**
			* @brief	Reverse the direction for the enumerator that defines the connection between two rooms.
			*
			* @details	This help function is used to reverse the enumerator that defines the connection between two rooms.
			* Mainly, this is used when we need to update the flow fields in adjacent rooms; since the connection from
			* current room -> adjacent room is the reverse of adjacent room -> current room, this function can be called
			* on the direction the current room has defined for the connection.
			*
			* For instance, if we send in DIRECTION_ADJACENT_ROOM_NORTH to this function, we will get
			* DIRECTION_ADJACENT_ROOM_SOUTH back.
			*
			* @param[in] currentDirection The direction for the connection that we want to know the reverse of.
			*
			*
			* @retval DIRECTION_ADJACENT_ROOM_NORTH The reverse of the inputed direction is North.
			* @retval DIRECTION_ADJACENT_ROOM_EAST	The reverse of the inputed direction is East.
			* @retval DIRECTION_ADJACENT_ROOM_SOUTH The reverse of the inputed direction is South.
			* @retval DIRECTION_ADJACENT_ROOM_WEST	The reverse of the inputed direction is West.
			*
			* @warning If the enum "DirectionToAdjacentRoom" is modified, this function will no longer work!
			*
			* Example code:
			* @code
			* for(int i = 0; i < 4; i++)
			* {
			*	DirectionToAdjacentRoom directionToRoom = DirectionToAdjacentRoom(i); //Works because the enum is 0->3
			*	if(adjacentRooms[directionToRoom])
			* 		adjacentRooms[directionToRoom]->UpdateFlowField(ReverseDirection(directionToRoom));
			* }
			* @endcode
			*/
			inline static DirectionToAdjacentRoom ReverseDirection(DirectionToAdjacentRoom currentDirection)
			{
				return DirectionToAdjacentRoom((int(currentDirection) + 2) % 4);
			}

			/**
			* @brief	Update all the AIs in the room
			*
			* @details	This function is used to update all the AIs in a room. This will be used in two situations;
			* update the AI's in the current room, and to update the AI's in adjacent rooms.
			*
			* Not decided: A bool that turns animation updates off, as to make sure that animations in a room that will never be seen isn't done.
			*
			* @param[in] dt DeltaTime for the current frame
			*
			* @retval void No return value
			*
			* @warning Due to this function not being implemented (bool animations not decided), the param[in] macro
			* will not work and thus hasn't been documented!
			*
			* Example code:
			* @code
			for(int i = 0; i < 4; i++)
			* {
			*	DirectionToAdjacentRoom directionToRoom = DirectionToAdjacentRoom(i); //Works because the enum is 0->3
			*	if(adjacentRooms[directionToRoom])
			* 		adjacentRooms[directionToRoom]->UpdateAIs(...);
			* }
			* @endcode
			*/
			void UpdateAIs(float dt);

			/**
			* @brief	Function to gather all "Adjacent rooms" updates in one function.
			*
			* @details	Helperfunction used to gather all updates for adjacent rooms into a single function.
			* Should be part of the Update() function.
			* 
			* @param[in] dt DeltaTime for the current frame
			*
			* @retval void No return value
			*
			* @warning Might need to be revisited later; this function might become troublesomely big.
			*
			*/
			void UpdateAdjacentRooms(float dt);
		public:
			Room(char map[25][25]);
			~Room();

			/**
			* @brief	This function will allow the user to add a reference to an adjacent room into this room.
			*
			* @details	Adds a refernce to an adjacent room to the specified room's adjacency list. This list will later be used
			* when decisions about moving the player and enemies between rooms, but also to update flowfields and enemies in
			* adjacent rooms.
			*
			* Note: Only one room can be placed in each direction!
			* Note: Room is not RESPONSIBLE for the rooms placed in it's adjacency list.
			* Note: If room A is adjacent to room B, then room B is adjacent to room A. This means that A needs to be in B's list, and B be in A's.
			*
			*
			* @param[in] direction The direction (noted by an DirectionToAdjacentRoom enum) to place the new room in.
			* @param[in] roomToAdd A pointer to the room. This will be placed in the adjacency list.
			*
			* @retval void No return value.
			*
			* @warning Only one room can be placed in each direction. This means that any room already placed
			* in that direction will be overwritten.
			*
			* Example code:
			* @code
			*	Room* firstRoom = new Room();
			*	Room* secondRoom = new Room();
			*
			*	firstRoom->AddAdjacentRoomByDirection(DirectionToAdjacentRoom::ADJACENT_ROOM_NORTH, secondRoom);
			*	secondRoom->AddAdjacentRoomByDirection(DirectionToAdjacentRoom::ADJACENT_ROOM_SOUTH, firstRoom);
			* @endcode
			*/
			inline void AddAdjacentRoomByDirection(DirectionToAdjacentRoom direction, Room* roomToAdd)
			{
				adjacentRooms[int(direction)] = roomToAdd;
			}

			/**
			* @brief	This function is used to add an enemy to the room.
			*
			* @details	This function can be used to add an enemy to a specific room either during room construction
			* or when an enemy moves between two rooms. The flag "ignorePowerLevel" can be used to make sure that
			* an enemy is added to a room, no matter the current powerlevel of that room.
			*
			* @param[in] toAdd The enemy to be added into the room.
			*
			* @retval true The enemy has been added to the room
			* @retval false The powerlevel of the room is to high for the current enemy to be added to the room.
			*
			* @warning Note that the room WILL take ownership of the AIs it contains. This means that they will delete them!
			*
			* Example code:
			* @code
			*	To be added when function is implemented
			* @endcode
			*/
			bool AddEnemyToRoom(SE::Gameplay::EnemyUnit *enemyToAdd/*bool ignorePowerLevel*/);
			
			inline const FlowField *GetFlowFieldMap() const
			{
				return roomField;
			};

			inline const Room* GetAdjacentRoomByDirection(DirectionToAdjacentRoom direction) const
			{
				return adjacentRooms[int(direction)];
			}

			/**
			* @brief	Update the room
			*
			* @details	Update the room and the adjacent rooms. This includes AIs, Flowfields, and any
			* logic that needs to be updated.
			*
			* @param[in] dt DeltaTime for the current frame
			* @param[in] playerX The x position of the player
			* @param[in] playerY The y position of the player
			*
			* @retval void No return value
			*
			*/
			void Update(float dt, float playerX, float playerY);

		};

	}
}


#endif
