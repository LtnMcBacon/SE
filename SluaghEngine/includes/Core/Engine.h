#ifndef SE_CORE_ENGINE_H_
#define SE_CORE_ENGINE_H_
namespace SE
{
	namespace Core
	{
		class EntityManager;
		class Engine
		{
		public:
			static Engine& GetInstance();

			

			EntityManager& GetEntityManager() const;
		private:
			Engine();
			Engine(const Engine& other) = delete;
			Engine(const Engine&& other) = delete;
			Engine& operator=(const Engine& rhs) = delete;
			~Engine();

			EntityManager* _entityManager;


		};

	}
}
#endif // SE_CORE_ENGINE_H_
