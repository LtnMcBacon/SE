#ifndef SE_GAMEPLAY_FLOWFIELD_H
#define SE_GAMEPLAY_FLOWFIELD_H
/**
* @brief	Struct to simplify positions
*/
struct pos // to be moved inside the class later on probably
{
	float x, y;
};

/**
*
* @brief Holds and updates a flowfield based on mapdata received at creation
*
* @details Holds and updates a flowfield based on mapdata received at creation, update should be called everytime the player moves so that the flowfield is correctly updated when the enemies
* sample their move vector from it
*
**/

class FlowField
{
private:

	const static int MAXSIZE = 32;

	/**
	* @brief	Enum over if tiles in the map/flowfield are blocked or walkable
	*/
	enum class Type
	{
		WALKABLE,
		BLOCKED
	};

	/**
	* @brief	Enum over flow vector directions
	*/
	enum class Direction
	{
		UP,
		LEFT,
		RIGHT,
		DOWN,
		UP_LEFT,
		DOWN_LEFT,
		UP_RIGHT,
		DOWN_RIGHT
	};

	/**
	* @brief	Constains map data
	*/
	struct MapData
	{
		Type type = Type::BLOCKED;
		float posX, posY;
		unsigned short int cost = -1;
		Direction dir;
	};

	MapData data[MAXSIZE][MAXSIZE];
	float sideLength;

	/**
	* @brief	Resets the value of each part of the grid to a high value
	*/
	void ResetField();

	/**
	* @brief	Updates the flowfield
	*
	* @details	Recursively updates the flowfield by checking if the neighbours have a higher cost than necessary and if they do then update their cost and let them check their neighbours
	*
	* @param[in] currentX the current X coordinate to work with in the grid
	* @param[in] currentY the current Y coordinate to work with in the grid
	*/
	void RecursiveLeifUpdate(unsigned int currentX, unsigned int currentY);

	/**
	* @brief	Updates the vectors of the grid
	*/
	void SetDirectionsBasedOnCosts();

public:
	FlowField(const char mapData[MAXSIZE][MAXSIZE], float sideLength, const pos& initialPullingPosition, float bottomLeftCornerXCoord, float bottomLeftCornerYCoord);
	~FlowField();

	/**
	* @brief	Updates the flowfield based on a position
	*
	* @details	Given a position the update function calculates the flowfield by updating the grids costs
	*
	* @param[in] playerPos the position of the player
	*/
	void Update(const pos& playerPos);

	/**
	* @brief	Gives a normalized vector indicating what direction the sampling enemy should move in
	*
	* @details	Samples from the flow chart what direction a given enemy should move in to move towards the player while walls and objects, the vector is normalized
	*
	* @param[in] enemyPos the position of the enemy
	* @param[in, out] xMagnitude the vectors x part
	* @param[in, out] yMagnitude the vectors y part
	*/
	void SampleFromMap(const pos& enemyPos, float& xMagnitude, float& yMagnitude);

};

#endif