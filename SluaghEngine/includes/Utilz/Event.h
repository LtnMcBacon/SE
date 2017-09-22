#ifndef SE_UTILZ_EVENT_H_
#define SE_UTILZ_EVENT_H_
#include <list>
#include "Delegator.h"
namespace SE
{
	namespace Utilz
	{
		template<typename T>
		class Event;


		/**
		*
		* @brief An object to register functions to and event.
		*
		* @details An event can register multiple functions.
		* Create an event with ex:
		* Event<int(int)> event;
		* event.Add<&TestFunc>();
		* Type t;
		* event.Add<Type, &Type::Func>(&t);
		* event(10);
		*
		**/
		template<typename R, typename... Args>
		class Event<R(Args...)>
		{
		public:
			/**
			* @brief Invokes all registered functions.
			* @sa Event
			*/
			inline void operator()(Args... args)
			{
				for (auto& deleg : delegates)
					deleg.Invoke(std::forward<Args>(args)...);
			}


			inline void Add(const Delegate<R(Args...)>& del)
			{
				delegates.push_front(del);
			}

			/**
			* @brief Registers a function.
			* @sa Event
			*/
			template<R(*Function)(Args...)>
			inline void Add(void)
			{
				delegates.push_back(Delegate<R(Args...)>::Make<Function>());
			}

			/**
			* @brief Registers a class method.
			* @sa Event
			*/
			template<class C, R(C::*Function)(Args...)>
			inline void Add(C* instance)
			{
				delegates.push_back(Delegate<R(Args...)>::Make<C, Function>(instance));
			}

			/**
			* @brief Registers a const class method.
			* @sa Event
			*/
			template<class C, R(C::*Function)(Args...) const>
			inline void Add(const C* instance)
			{
				delegates.push_back(Delegate<R(Args...)>::Make<C, Function>(instance));
			}

			/**
			* @brief Removes a lambda.
			* @sa Event
			*/
			template<R(*Function)(Args...)>
			inline void Remove(const std::function<R(Args...)>& instance)
			{
				delegates.remove(Delegate<R(Args...)>::Make(instance));
			}
			/**
			* @brief Removes a function.
			* @sa Event
			*/
			template<R(*Function)(Args...)>
			inline void Remove(void)
			{
				delegates.remove(Delegate<R(Args...)>::Make<Function>());
			}

			/**
			* @brief Removes a class method.
			* @sa Event
			*/
			template<class C, R(C::*Function)(Args...)>
			inline void Remove(C* instance)
			{
				delegates.remove(Delegate<R(Args...)>::Make<C, Function>(instance));
			}

			/**
			* @brief Removes a const class method.
			* @sa Event
			*/
			template<class C, R(C::*Function)(Args...) const>
			inline void Remove(const C* instance)
			{
				delegates.remove(Delegate<R(Args...)>::Make<C, Function>(instance));
			}
		private:
			std::list<Delegate<R(Args...)>> delegates;
		};
	}
}
#endif