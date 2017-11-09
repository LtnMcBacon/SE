#ifndef SE_INTERFACE_ANIMATION_MANAGER_H_
#define SE_INTERFACE_ANIMATION_MANAGER_H_

#if defined DLL_EXPORT_CORE
#define DECLDIR_CORE __declspec(dllexport)
#else
#define DECLDIR_CORE __declspec(dllimport)
#endif


#include <Utilz\GUID.h>
#include <Graphics\IRenderer.h>
#include <ResourceHandler\IResourceHandler.h>
#include <Window\IWindow.h>

#include "IManager.h"
#include "IEntityManager.h"
#include "ITransformManager.h"
#include "IRenderableManager.h"
#include <DevConsole\IConsole.h>
#include "IEventManager.h"

#define ENUM_FLAG_OPERATOR(T,X) inline T operator X (T lhs, T rhs) { return (T) (static_cast<std::underlying_type_t <T>>(lhs) X static_cast<std::underlying_type_t <T>>(rhs)); } 
#define ENUM_FLAG_OPERATOR2(T,X) inline void operator X= (T& lhs, T rhs) { lhs = (T)(static_cast<std::underlying_type_t <T>>(lhs) X static_cast<std::underlying_type_t <T>>(rhs)); } 
#define ENUM_FLAGS(T) \
enum class T; \
inline T operator ~ (T t) { return (T) (~static_cast<std::underlying_type_t <T>>(t)); } \
inline bool operator & (T lhs, T rhs) { return (static_cast<std::underlying_type_t <T>>(lhs) & static_cast<std::underlying_type_t <T>>(rhs));  } \
ENUM_FLAG_OPERATOR2(T,|) \
ENUM_FLAG_OPERATOR2(T,&) \
ENUM_FLAG_OPERATOR(T,|) \
ENUM_FLAG_OPERATOR(T,^)

namespace SE
{
	namespace Core
	{
		enum class AnimationFlags {

			MIX = 1,
			BLENDTO = 1 << 1,
			BLENDTOANDBACK = 1 << 2,
			IMMEDIATE = 1 << 3,
			LOOP = 1 << 4,
			CURRENT = 1 << 5,
			FORCED = 1 << 6

		};

		class IAnimationManager : public IManager
		{
		public:

			struct InitializationInfo
			{
				ResourceHandler::IResourceHandler* resourceHandler;
				Graphics::IRenderer* renderer;
				DevConsole::IConsole* console;
				Window::IWindow* window;
				IEntityManager* entityManager;
				IEventManager* eventManager;
				ITransformManager* transformManager;
				Utilz::ThreadPool* threadPool;
			};
			struct CreateInfo
			{
				Utilz::GUID mesh;
				Utilz::GUID skeleton;
				size_t animationCount;
				Utilz::GUID* animations;
			};

			struct AnimationPlayInfo
			{
				static const size_t maxLayers = 4;
				size_t nrOfLayers = 0;
				Utilz::GUID animations[maxLayers];
				float timePos[maxLayers];
				float animationSpeed[maxLayers];
				bool looping[maxLayers];
				float blendSpeed[maxLayers];
			};

			virtual ~IAnimationManager() {};

			virtual void CreateAnimatedObject(const Entity& entity, const CreateInfo& info) = 0;

			virtual void AttachToEntity(const Entity& source, const Entity& entityToAttach, const Utilz::GUID& jointGUID, int slotIndex) = 0;

			virtual void Start(const Entity& entity, GUID* animations, size_t nrOfAnims, float duration, AnimationFlags flag) = 0;
			virtual void Start(const Entity& entity, const AnimationPlayInfo& playInfo) = 0;
			virtual void Start(const Entity& entity, bool looping)const = 0;
			virtual void SetSpeed(const Entity& entity, float speed) = 0;
			virtual void SetKeyFrame(const Entity& entity, float keyFrame) = 0;
			virtual void SetBlendSpeed(const Entity& entity, int index, float speed) = 0;
			virtual void Pause(const Entity& entity)const = 0;
			virtual bool IsAnimationPlaying(const Entity& entity) const = 0;
			virtual void UpdateBlending(const Entity& entity, int index) = 0;

			virtual void ToggleVisible(const Entity& entity, bool visible) = 0;

		protected:
			IAnimationManager() {};
	
			IAnimationManager(const IAnimationManager& other) = delete;
			IAnimationManager(const IAnimationManager&& other) = delete;
			IAnimationManager& operator=(const IAnimationManager& other) = delete;
			IAnimationManager& operator=(const IAnimationManager&& other) = delete;
		};


		/**
		* @brief Create an instance of the AnimationManager
		**/
		DECLDIR_CORE IAnimationManager* CreateAnimationManager(const IAnimationManager::InitializationInfo& initInfo);
	}
}

ENUM_FLAGS(SE::Core::AnimationFlags);

#endif //SE_INTERFACE_ANIMATION_MANAGER_H_