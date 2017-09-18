#ifndef _SE_GAMEPLAY_ROOM_H_
#define _SE_GAMEPLAY_ROOM_H_
#include <vector>

namespace SE
{
	namespace Gameplay
	{
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
			enum class AdjacencyRooms
			{
				
				ADJACENT_ROOM_NORTH,
				ADJACENT_ROOM_EAST,
				ADJACENT_ROOM_SOUTH,
				ADJACENT_ROOM_WEST
			};
		private:

		public:
			Room();
			~Room();

			inline void AddAdjacentRoomByDirection(AdjacencyRooms direction, Room* roomToAdd)
			{
				adjacentRooms[int(direction)] = roomToAdd;
			}

			bool AddEnemyToRoom(/*Enemy class toAdd*/);
			
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
