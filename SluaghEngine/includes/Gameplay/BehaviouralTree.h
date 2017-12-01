#ifndef SE_GAMEPLAY_BEHAVIOURAL_TREE_H
#define SE_GAMEPLAY_BEHAVIOURAL_TREE_H
#include "Utilz/CPUTimeCluster.h"

/*The Behavioural Tree and its components are based upon:
 * -The descriptions in the GPU pro series
 * - https://www.gamasutra.com/blogs/ChrisSimpson/20140717/221339/Behavior_trees_for_AI_How_they_work.php
 * - https://gamedevdaily.io/managing-ai-in-gigantic-523dc84763cf
 * - https://gamedevdaily.io/advanced-behavior-tree-structures-4b9dc0516f92
 */

namespace SE
{
	namespace Gameplay
	{
		struct GameBlackboard;
		struct EnemyBlackboard;
		class IBehaviour;

		/**
		*
		* @brief The entry point of the Behavioural Tree, that makes the decisions for the Enemy
		*
		* @details To be written
		* 
		* @warning To be written
		*
		* @sa IBehaviour
		*
		**/
		class BehaviouralTree
		{
		private:
			static Utilz::CPUTimeCluster behaviouralTimer;
			static bool firstInitialize;
			static size_t numberOfActiveTrees;
			static bool firstTick;
			static size_t memoryUsage;
			BehaviouralTree() = delete;
		public:
			BehaviouralTree* CopyTree(GameBlackboard* gameBlackboard, EnemyBlackboard* enemyBlackboard) const;
			BehaviouralTree(IBehaviour* root);
			~BehaviouralTree();

			/**
			* @brief	Start the iteration of the behavioural tree
			*
			* @details	This function will run the behavioural tree and perform an action for the AI. (To be updated)
			*
			* @retval void No return
			*
			* @warning Make sure that the blackboards are updated before calling!
			*/
			void Tick();

			/**
			 * @brief Memory size of this tree, including the EnemyBlackboard but EXCLUDING the gameBlackboard.
			 */
			size_t SizeOfTree();
		protected:
			IBehaviour* rootBehaviour;


		};
	}
}

#endif