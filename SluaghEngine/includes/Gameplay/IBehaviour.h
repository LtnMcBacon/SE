#ifndef SE_GAMEPLAY_IBEHAVIOUR_H
#define SE_GAMEPLAY_IBEHAVIOUR_H

namespace SE
{
	namespace Gameplay
	{
		class EnemyBlackboard;
		class GameBlackboard;

		/**
		*
		* @brief The statues that can be returned by the IBehaviour and its subclasses
		* 
		* @sa IBehaviour.
		*
		**/
		enum class Status
		{
			BEHAVIOUR_FAILURE,
			BEHAVIOUR_RUNNING,
			BEHAVIOUR_SUCCESS,
			BEHAVIOUR_SUSPENDED,
			BEHAVIOUR_INVALID
		};
		/**
		*
		* @brief The IBehaviour class is the interface class that all classes in the BehaviouralTree inheritance from.
		*
		* @details Every subclass must implement ALL functions, save the GetStatus function, from this class.
		* 
		* @warning If the Status::BEHAVIOUR_INVALID is returned, something is seriously wrong.
		*
		* @sa BehaviouralTree for the root-node and its use.
		*
		**/
		class IBehaviour
		{
		private:
			IBehaviour() = delete;
		protected:
			EnemyBlackboard* enemyBlackboard;
			GameBlackboard* gameBlackboard;
			Status myStatus = Status::BEHAVIOUR_INVALID;

			/**
			* @brief	Initialize the behaviour
			*
			* @details	Make sure that all the data the behaviour need is properly set up.
			* 
			* Note that if the behaviour needs some conditional checks done before being able to run,
			* that should be placed in a behaviour (derived from the ICondition class) before this behaviour.
			*
			* @retval void No return value
			*
			* @warning Should ONLY be called once, immediately before the first update tick
			*
			*/
			virtual void OnInitialization() { };

			/**
			* @brief	Update the behaviour.
			*
			* @details	Update the behaviour. Exactly what this does is up to the behaviour to define.
			*
			* @retval BEHAVIOUR_FAILURE The behaviour failed to complete.
			* @retval BEHAVIOUR_RUNNING The behaviour is executing, and may take multiple frames to finish.
			* @retval BEHAVIOUR_SUCCESS The behaviour is finished and succeeded in its execution.
			* @retval BEHAVIOUR_SUSPENDED The behaviour is suspended by another node, and should be checked in on later.
			* @retval BEHAVIOUR_INVALID Something is wrong, since this should only be a "default" state.
			*
			* @warning For debugging purposes, make sure that this function always returns "BEHAVIOUR_INVALID" last, even if that's after
			* a "while true" loop.
			*
			*/
			virtual Status Update() = 0;
			
			/** 
			* @brief	Function to clear up after the behaviour has run.
			* 
			* @details	This function is used to make sure that a behaviour cleans up after itself. If this function 
			* is not defined in a derived class, an emtpy OnTermination will be called from the IBehaviour class.
			*
			* @param[in] terminationStatus the termination status of the Behaviour.
			*
			* @retval void No return value
			* 
			* @warning Should ONLY be called once, immediately after update returns a status that is not BEHAVIOUR_RUNNING
			*/
			virtual void OnTermination(Status terminationStatus) { };

		public:
			IBehaviour(EnemyBlackboard* enemyBlackboard, GameBlackboard* gameBlackboard) :
				enemyBlackboard(enemyBlackboard),
				gameBlackboard(gameBlackboard) {
				myStatus = Status::BEHAVIOUR_INVALID;
			};

			virtual ~IBehaviour(){};

			inline Status GetStatus() const { return myStatus; }

			/**
			* @brief	Wrap function to make sure that the behaviours are called properly.
			*
			* @details	Wraper function of the OnInitialization, Update, and OnTermination functions.
			* Used to make sure that a lot of beginner mistakes are avoided (gathering the functions in
			* a single entry point). See Game AI Pro (1), page 76.
			*
			* @retval BEHAVIOUR_FAILURE The behaviour failed to complete.
			* @retval BEHAVIOUR_RUNNING The behaviour is executing, and may take multiple frames to finish.
			* @retval BEHAVIOUR_SUCCESS The behaviour is finished and succeeded in its execution.
			* @retval BEHAVIOUR_SUSPENDED The behaviour is suspended by another node, and should be checked in on later.
			* @retval BEHAVIOUR_INVALID Something is wrong, since this should only be a "default" state.
			*
			*/
			Status Tick();


		};

	}
}



#endif