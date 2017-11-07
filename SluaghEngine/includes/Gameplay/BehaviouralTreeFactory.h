#ifndef SE_GAMEPLAY_BEHAVIOURAL_TREE_FACTORY_H
#define SE_GAMEPLAY_BEHAVIOURAL_TREE_FACTORY_H
#include <map>
#include "Utilz/GUID.h"
#include <vector>
#include <ResourceHandler\IResourceHandler.h>

namespace SE
{
	
	namespace Gameplay
	{
		class BehaviouralTree;
		class IBehaviour;
		struct GameBlackboard;
		struct EnemyBlackboard;
		class BehaviouralTreeFactory
		{
		private:

			std::map<Utilz::GUID, BehaviouralTree*, Utilz::GUID::Compare> behaviouralTrees;

			struct NodeData
			{
				std::string Type;
				std::vector<std::string> nodeData;
				std::vector<int> childID;

			};
			/*Create from type*/
			/**
			* @brief	Read the type of the node and call the correct "Create" function.
			* 
			* @sa CreateTreeFromNodeData
			*/
			IBehaviour* CreateFromType(NodeData* dataArray, int nodeID);

			/*Create Leaves*/


			/**
			* @brief	Create a Leaf Node with the data given on position nodeID in dataArray.
			*
			* @warning Not properly created! Will be divided into many kind of leaves!
			* Right now only returns a "SucceederLeaf", that always succeed.
			*
			* @sa CreateTreeFromNodeData
			*/
			IBehaviour* CreateLeaf(NodeData* dataArray, int nodeID);


			/**
			* @brief	Create a FlowFieldMovementLeaf Node with the data given on position nodeID in dataArray.
			*
			*
			* @sa CreateTreeFromNodeData
			*/
			IBehaviour* CreateFlowFieldMovementLeaf(NodeData* dataArray, int nodeID);

			/**
			* @brief	Create a MoveTowardsPlayer Node with the data given on position nodeID in dataArray.
			*
			*
			* @sa CreateTreeFromNodeData
			*/
			IBehaviour* CreateMoveTowardsPlayerLeaf(NodeData* dataArray, int nodeID);

			/**
			* @brief	Create a GlaistigAttack Node with the data given on position nodeID in dataArray.
			*
			*
			* @sa CreateTreeFromNodeData
			*/
			IBehaviour* CreateGlaistigAttackLeaf(NodeData* dataArray, int nodeID);

			/**
			* @brief	Create a GlaistigAttack Node with the data given on position nodeID in dataArray.
			*
			*
			* @sa CreateTreeFromNodeData
			*/
			IBehaviour* CreateResetAttackCooldownLeaf(NodeData* dataArray, int nodeID);

			/**
			* @brief	Create a GlaistigAttack Node with the data given on position nodeID in dataArray.
			*
			*
			* @sa CreateTreeFromNodeData
			*/
			IBehaviour* CreateAddTimeToAttackCooldownLeaf(NodeData* dataArray, int nodeID);

			/**
			* @brief	Create a GlaistigAttack Node with the data given on position nodeID in dataArray.
			*
			*
			* @sa CreateTreeFromNodeData
			*/
			IBehaviour* CreateTickDownAttackCooldownLeaf(NodeData* dataArray, int nodeID);

			/**
			* @brief	Create a TimerCondition Node with the data given on position nodeID in dataArray.
			*
			*
			* @sa CreateTreeFromNodeData
			*/
			IBehaviour* CreateTimerConditionLeaf(NodeData* dataArray, int nodeID);

			/**
			* @brief	Create a TimerCondition Node with the data given on position nodeID in dataArray.
			*
			*
			* @sa CreateTreeFromNodeData
			*/
			IBehaviour* CreateLineOfSightConditionLeaf(NodeData* dataArray, int nodeID);

			/**
			* @brief	Create a RangeToPlayerCondition Node with the data given on position nodeID in dataArray.
			*
			*
			* @sa CreateTreeFromNodeData
			*/
			IBehaviour* CreateRangeToPlayerConditionLeaf(NodeData* dataArray, int nodeID);

			/**
			* @brief	Create a AttackCooldownZeroCondition Node with the data given on position nodeID in dataArray.
			*
			*
			* @sa CreateTreeFromNodeData
			*/
			IBehaviour* CreateAttackCooldownZeroConditionLeaf(NodeData* dataArray, int nodeID);

			/**
			* @brief	Create a StunnedCondition Node with the data given on position nodeID in dataArray.
			*
			*
			* @sa CreateTreeFromNodeData
			*/
			IBehaviour* CreateStunnedConditionLeaf(NodeData* dataArray, int nodeID);

			/**
			* @brief	Create a ObstacleOnPositionCondition Node with the data given on position nodeID in dataArray.
			*
			*
			* @sa CreateTreeFromNodeData
			*/
			IBehaviour* CreateObstacleOnPositionConditionLeaf(NodeData* dataArray, int nodeID);

			/**
			* @brief	Create a ChannelingCondition Node with the data given on position nodeID in dataArray.
			*
			*
			* @sa CreateTreeFromNodeData
			*/
			IBehaviour* CreateChannelingConditionLeaf(NodeData* dataArray, int nodeID);

			/**
			* @brief	Create a InTheSameRoomAsThePlayerCondition Node with the data given on position nodeID in dataArray.
			*
			*
			* @sa CreateTreeFromNodeData
			*/
			IBehaviour* CreateInTheSameRoomAsThePlayerConditionLeaf(NodeData* dataArray, int nodeID);

			/**
			* @brief	Create a AnimiationRunningCondition Node with the data given on position nodeID in dataArray.
			*
			*
			* @sa CreateTreeFromNodeData
			*/
			IBehaviour* CreateAnimationRunningLeaf(NodeData* dataArray, int nodeID);

			/**
			* @brief	Create a ObstacleOnPositionCondition Node with the data given on position nodeID in dataArray.
			*
			*
			* @sa CreateTreeFromNodeData
			*/
			IBehaviour* CreateMakeInvulnerableLeaf(NodeData* dataArray, int nodeID);

			/**
			* @brief	Create a ObstacleOnPositionCondition Node with the data given on position nodeID in dataArray.
			*
			*
			* @sa CreateTreeFromNodeData
			*/
			IBehaviour* CreateMakeVulnerableLeaf(NodeData* dataArray, int nodeID);

			/**
			* @brief	Create a ObstacleOnPositionCondition Node with the data given on position nodeID in dataArray.
			*
			*
			* @sa CreateTreeFromNodeData
			*/
			IBehaviour* CreateFaceThePlayerLeaf(NodeData* dataArray, int nodeID);

			/**
			* @brief	Create a ObstacleOnPositionCondition Node with the data given on position nodeID in dataArray.
			*
			*
			* @sa CreateTreeFromNodeData
			*/
			IBehaviour* CreateLeapAtPlayerLeaf(NodeData* dataArray, int nodeID);

			/**
			* @brief	Create a BodachMeleeAttackLeaf Node with the data given on position nodeID in dataArray.
			*
			*
			* @sa CreateTreeFromNodeData
			*/
			IBehaviour* CreateBodachMeleeAttackLeaf(NodeData* dataArray, int nodeID);

			/**
			* @brief	Create a PechMeleeAttackLeaf Node with the data given on position nodeID in dataArray.
			*
			*
			* @sa CreateTreeFromNodeData
			*/
			IBehaviour* CreatePechMeleeAttackLeaf(NodeData* dataArray, int nodeID);

			/**
			* @brief	Create a PechRAngedAttackLeaf Node with the data given on position nodeID in dataArray.
			*
			*
			* @sa CreateTreeFromNodeData
			*/
			IBehaviour* CreatePechRangedAttackLeaf(NodeData* dataArray, int nodeID);

			/**
			* @brief	Create an AnimationLeaf Node with the data given on position nodeID in dataArray.
			*
			*
			* @sa CreateTreeFromNodeData
			*/
			IBehaviour* CreateAnimationLeaf(NodeData* dataArray, int nodeID);


			/**
			* @brief	Create a ObstacleOnPositionCondition Node with the data given on position nodeID in dataArray.
			*
			*
			* @sa CreateTreeFromNodeData
			*/
			IBehaviour* CreateWhileChannelingLeaf(NodeData* dataArray, int nodeID);

			/*Create Composites*/

			/**
			* @brief	Create a Sequence Node with the data given on position nodeID in dataArray.
			*
			* @sa CreateTreeFromNodeData
			*/
			IBehaviour* CreateSequence(NodeData* dataArray, int nodeID);

			/**
			* @brief	Create a Selector Node with the data given on position nodeID in dataArray.
			*
			* @sa CreateTreeFromNodeData
			*/
			IBehaviour* CreateSelector(NodeData* dataArray, int nodeID);

			/**
			* @brief	Create a RandomSequence Node with the data given on position nodeID in dataArray.
			*
			* @sa CreateTreeFromNodeData
			*/
			IBehaviour* CreateRandomSequence(NodeData* dataArray, int nodeID);

			/**
			* @brief	Create a RandomSelector Node with the data given on position nodeID in dataArray.
			*
			* @sa CreateTreeFromNodeData
			*/
			IBehaviour* CreateRandomSelector(NodeData* dataArray, int nodeID);

			/**
			* @brief	Create a Sequence Node with the data given on position nodeID in dataArray.
			*
			* @sa CreateTreeFromNodeData
			*/
			IBehaviour* CreateRestartingSequence(NodeData* dataArray, int nodeID);

			/*Create Decorators*/

			/**
			* @brief	Create an Inverter Node with the data given on position nodeID in dataArray.
			*
			* @sa CreateTreeFromNodeData
			*/
			IBehaviour* CreateInverter(NodeData* dataArray, int nodeID);

			/**
			* @brief	Create a Repeater Node with the data given on position nodeID in dataArray.
			*
			* @sa CreateTreeFromNodeData
			*/
			IBehaviour* CreateRepeater(NodeData* dataArray, int nodeID);

			/**
			* @brief	Create a RepeatUntilFail Node with the data given on position nodeID in dataArray.
			*
			* @sa CreateTreeFromNodeData
			*/
			IBehaviour* CreateRepeatUntilFail(NodeData* dataArray, int nodeID);

			/**
			* @brief	Create a Succeeder Node with the data given on position nodeID in dataArray.
			*
			* @sa CreateTreeFromNodeData
			*/
			IBehaviour* CreateSucceeder(NodeData* dataArray, int nodeID);

			/**
			* @brief	Create a FailUntilSuccess Node with the data given on position nodeID in dataArray.
			*
			* @sa CreateTreeFromNodeData
			*/
			IBehaviour* CreateFailUntilSuccess(NodeData* dataArray, int nodeID);
			/**
			* @brief	Create a BehaviouralTree from the NodeData created in 
			* LoadTreeFromResourceHandler
			*
			* @details Always called from LoadTreeFromResourceHandler, this function
			* takes the data loaded into a NodeData array and build the BehaviouralTree.
			* If the BehaviouralTree is created correctly, it will be placed in a map, using the GUID
			* as key.
			*
			* @param[in] GUID The GUID of the file that we load the BehaviouralTree from. Used as key for the map.
			* @param[in] dataArray The data read in from LoadTreeFromResourceHandler.
			* @param[in] size The number of nodes that the tree consists of.
			*
			* @sa LoadTreeFromResourceHandler for the information creation, 
			* CreateFromType for the creation of individual nodes.
			*/
			bool CreateTreeFromNodeData(const Utilz::GUID& GUID, NodeData* dataArray, size_t size);
		public:

			/**
			* @brief	Load a Tree from data file and store it in an internal map.
			* 
			* @details If the Tree already exists in the map, nothing happens.
			*
			* @param[in] GUID The GUID of the file that should be loaded
			* 
			* @retval true The Tree is stored in the map
			* @retval false Something went wrong
			*
			* @warning Note that this function WILL NOT return a BehaviouralTree, only load it into memory.
			* For the creation of a BehaviouralTree from a loaded file, see BuildBehaviouralTree.
			*
			*/
			bool LoadTree(const Utilz::GUID& GUID);


			/**
			* @brief	Build a BehaviouralTree from a tree loaded in and placed in the map.
			*
			* @details Create a copy of a BehaviouralTree already loaded in, giving them the GameBlackboard
			* and EnemyBlackboard loaded in.
			*
			* @param[in] GUID The GUID of the file that should be loaded
			* @param[in] gameBlackboard The blackboard of the game. MUST NOT BE NULLPTR!
			* @param[in] enemyBlackboard The blackboard for the enemy. MUST NOT BE NULLPTR!
			*
			* @retval nullptr The tree is probably not loaded in.
			* @retval BehaviouralTree* The BehaviouralTree created from the GUID.
			*
			* @warning Note that this function needs the BehaviouralTree loaded in through the LoadTree function!
			* If no BehaviouralTree is presented in the map for the inparam GUID, a nullptr will be returned.
			*
			*/
			BehaviouralTree* BuildBehaviouralTree(const Utilz::GUID& GUID, GameBlackboard* gameBlackboard, EnemyBlackboard* enemyBlackboard);
			BehaviouralTreeFactory();
			~BehaviouralTreeFactory();
		};
	}
}


#endif
