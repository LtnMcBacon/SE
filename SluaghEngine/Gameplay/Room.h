#ifndef _SE_GAMEPLAY_ROOM_H_
#define _SE_GAMEPLAY_ROOM_H_
#include <vector>

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
			std::vector<std::vector<bool>> map;
			/*Needed:
			 * Representation of the room module(s) that build the room
			 * The enemies that are represented in the room
			 * FlowField map and calculations
			 * Function(s) to build the room
			 */
		public:
			/**
			*
			* @brief AdjacencyRooms is used to define the direction of an adjacent room in the Room class.
			*
			* @details AdjacencyRooms is used to define the direction of an adjacent room in the Room class.
			*
			* @warning Note that only four directions are allowed!
			*
			* @sa Nothing yet
			*
			**/
			enum class AdjacencyRooms
			{
				
				ADJACENT_ROOM_NORTH,	/**<The room lies to the North (0) */ 
				ADJACENT_ROOM_EAST,		/**<The room lies to the East (1) */ 
				ADJACENT_ROOM_SOUTH,	/**<The room lies to the South (2) */ 
				ADJACENT_ROOM_WEST		/**<The room lies to the West (3) */ 
			};
		private:

		public:
			Room();
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
			* @param[in] direction The direction (noted by an AdjacencyRooms enum) to place the new room in.
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
			*	firstRoom->AddAdjacentRoomByDirection(AdjacencyRooms::ADJACENT_ROOM_NORTH, secondRoom);
			*	secondRoom->AddAdjacentRoomByDirection(AdjacencyRooms::ADJACENT_ROOM_SOUTH, firstRoom);
			* @endcode
			*/
			inline void AddAdjacentRoomByDirection(AdjacencyRooms direction, Room* roomToAdd)
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
			*
			* @retval true The enemy has been added to the room
			* @retval false The powerlevel of the room is to high for the current enemy to be added to the room.
			*
			* @warning Due to this function not being implemented (Enemy class not constructed), the param[in] macro
			* will not work and thus hasn't been documented!
			*
			* Example code:
			* @code
			*	To be added when function is implemented
			* @endcode
			*/
			bool AddEnemyToRoom(/*EnemyClass *toAdd, bool ignorePowerLevel*/);
			
			
			inline const std::vector<std::vector<bool>>& GetMap() const
			{
				return map;
			};

			inline const Room* GetAdjacentRoomByDirection(AdjacencyRooms direction) const
			{
				return adjacentRooms[int(direction)];
			}

		};

	}
}


#endif
