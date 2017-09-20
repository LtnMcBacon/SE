#ifndef SE_UTILZ_DELEGATOR_H_
#define SE_UTILZ_DELEGATOR_H_
#include <utility>
#include <assert.h>
#include <functional>
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


			inline static R LambdaProxy(void* instance, Args... args)
			{
				return (*(static_cast<std::function<R(Args...)>*>(instance)))(std::forward<Args>(args)...);
			}

			template<R(*Function)(Args...)>
			inline static R FunctionProxy(void*, Args... args)
			{
				return Function(std::forward<Args>(args)...);
			}

			template<class C, R(C::*Function)(Args...)>
			inline static R MethodProxy(void* instance, Args... args)
			{
				return (static_cast<C*>(instance)->*Function)(std::forward<Args>(args)...);
			}

			template <class C, R(C::*Function)(Args...) const>
			inline static R ConstMethodProxy(void* instance, Args... args)
			{
				return (static_cast<const C*>(instance)->*Function)(std::forward<Args>(args)...);
			}

		public:
			Delegate() : instance(nullptr), proxy(nullptr), lambda(false) {}
			Delegate(void* instance, ProxyFunction function, bool lambda = false) : instance(instance), proxy(function), lambda(lambda) {}
			Delegate(const Delegate& other)
			{
				//instance = other.lambda ? new std::function<R(Args...)>(*(other.instance)) : other.instance;
				if (other.lambda)
				{
					instance = new std::function<R(Args...)>;
					memcpy(instance, other.instance, sizeof(std::function<R(Args...)>));
				}
					
				else
					instance = other.instance;
				proxy = other.proxy;
				lambda = other.lambda;
			};
			Delegate(Delegate&& other)
			{
				instance = other.instance;
				proxy = other.proxy;
				lambda = other.lambda;
				other.lambda = false;
			};
			void operator=(const Delegate& other)
			{
				if (lambda)
				{
					delete instance;
				}

				instance = other.lambda ? new std::function<R(Args...)>(*(other.instance)) : other.instance;

				proxy = other.proxy;
				lambda = other.lambda;
			}
			~Delegate() { if (lambda) delete instance; }


			static Delegate Make(const std::function<R(Args...)>& instance)
			{
				auto ni = new std::function<R(Args...)>(instance);
				Delegate d(ni, &LambdaProxy, true);
				return d;
			}
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

			inline void Bind(const std::function<R(Args...)>& instance)
			{
				if (lambda) {
					delete instance;
					lambda = false;
				}
				lambda = true;
				this->instance = new std::function<R(Args...)>(instance);
				proxy = &LambdaProxy;
			}
			template<R(*Function)(Args...)>
			inline void Bind(void)
			{
				if (lambda) {
					delete instance;
					lambda = false;
				}
				instance = nullptr;
				proxy = &FunctionProxy<Function>;
			}

			template<class C, R(C::*Function)(Args...)>
			inline void Bind(C* instance)
			{
				if (lambda) {
					delete instance;
					lambda = false;
				}
				this->instance = instance;
				proxy = &MethodProxy<C, Function>;
			}

			template<class C, R(C::*Function)(Args...) const>
			inline void Bind(const C* instance)
			{
				if (lambda) {
					delete instance;
					lambda = false;
				}
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
			bool lambda;
			void* instance;
			ProxyFunction proxy;
		};

	}
}



#endif //SE_UTILZ_DELEGATOR_H_