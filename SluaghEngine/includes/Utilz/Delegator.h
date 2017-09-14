#ifndef SE_UTILZ_DELEGATOR_H_
#define SE_UTILZ_DELEGATOR_H_
#include <utility>
#include <assert.h>
namespace SE
{
	namespace Utilz
	{
		template<typename T>
		class Delegate {};

		/**
		*
		* @brief An object to register a single function. You probably want Event
		* @sa Event
		*
		**/
		template<typename R, typename... Args>
		class Delegate<R(Args...)>
		{
			typedef R(*ProxyFunction)(void*, Args...);
			template<R (*Function)(Args...)>
			inline static R FunctionProxy(void*, Args... args)
			{
				return Function(std::forward<Args>(args)...);
			}

			template<class C, R (C::*Function)(Args...)>
			inline static R MethodProxy(void* instance, Args... args)
			{
				return (static_cast<C*>(instance)->*Function)(std::forward<Args>(args)...);
			}

			template <class C, R (C::*Function)(Args...) const>
			inline static R ConstMethodProxy(void* instance, Args... args)
			{
				return (static_cast<const C*>(instance)->*Function)(std::forward<Args>(args)...);
			}

		public:
			Delegate() : instance(nullptr), proxy(nullptr) {}
			Delegate(void* instance, ProxyFunction function) : instance(instance), proxy(function) {}

			template<R(*Function)(Args...)>
			static Delegate Make()
			{
				Delegate d(nullptr, &FunctionProxy<Function>);
				return d;
			}
			template<class C, R(C::*Function)(Args...)>
			static Delegate Make(C* instance)
			{
				Delegate d(instance, &MethodProxy<C, Function>);
				return d;
			}
			template<class C, R(C::*Function)(Args...) const>
			static Delegate Make(const C* instance)
			{
				Delegate d(const_cast<C*>(instance), &ConstMethodProxy<C, Function>);
				return d;
			}

			template<R (*Function)(Args...)>
			inline void Bind(void)
			{
				instance = nullptr;
				proxy = &FunctionProxy<Function>;
			}
			template<class C, R(C::*Function)(Args...)>
			inline void Bind(C* instance)
			{
				this->instance = instance;
				proxy = &MethodProxy<C, Function>;
			}
			template<class C, R(C::*Function)(Args...) const>
			inline void Bind(const C* instance)
			{
				this->instance = const_cast<C*>(instance);
				proxy = &ConstMethodProxy<C, Function>;
			}

			inline R operator()(Args... args)const
			{
				return proxy(instance, std::forward<Args>(args)...);
			}
			inline R Invoke(Args... args) const
			{
				return proxy(instance, std::forward<Args>(args)...);
			}
		private:
			void* instance;
			ProxyFunction proxy;
		};
		
	}
}



#endif //SE_UTILZ_DELEGATOR_H_