#ifndef SE_CORE_INTERFACE_EVENT_MANAGER_H_
#define SE_CORE_INTERFACE_EVENT_MANAGER_H_


#if defined DLL_EXPORT_CORE
#define DECLDIR_CORE __declspec(dllexport)
#else
#define DECLDIR_CORE __declspec(dllimport)
#endif


namespace SE
{
	namespace Core
	{
		class IEventManager
		{
		public:

			virtual void RegisterToSetRenderObjectInfo(const Utilz::Delegate<void(const Entity& entity, SE::Graphics::RenderJob* info)>&& callback) = 0;
			virtual void TriggerSetRenderObjectInfo(const Entity& entity, SE::Graphics::RenderJob* info) = 0;


		protected:
			IEventManager() {};
			



		};

		/**
		* @brief Create an instance of the EventManager
		**/
		DECLDIR_CORE IEventManager* CreateEventManager();
	}
}

#endif