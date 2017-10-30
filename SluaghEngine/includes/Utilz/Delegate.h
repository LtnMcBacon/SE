#ifndef SE_UTILZ_DELEGATE_H_
#define SE_UTILZ_DELEGATE_H_
#include <functional>
namespace SE
{
	namespace Utilz
	{
		template <typename T> class Delegate;

		template<typename RET, typename... PARAMS>
		class Delegate<RET(PARAMS...)>
		{
			std::function<RET(PARAMS...)> invoker;
			//size_t uniqueIdentifier;
		public:

			Delegate() {}

			operator bool() { return invoker.operator bool(); }

			/**
			*@brief Copy constructor.
			*/
			Delegate(const Delegate& other)
			{
				this->invoker = other.invoker;
				//this->uniqueIdentifier = other.uniqueIdentifier;
			}
			/**
			*@brief Copy constructor with rvalue.
			*/
			Delegate(const Delegate&& other)
			{
				this->invoker = std::move(other.invoker);
				//this->uniqueIdentifier = other.uniqueIdentifier;
			}

			/**
			*@brief Create delegate from function pointer.
			*@param ptr The function pointer. (&foo)
			* Example code:
			* @code
			* void Call(const Delegate<void()>& del) { del();}
			* void foo(){cout << "Hello World" << endl;}
			*
			* Delegate<void()> del(&foo);
			* del(); // Prints "Hello World"
			* Call(&foo); // Prints "Hello World"
			* @endcode
			*/
			Delegate(RET(ptr)(PARAMS...))
			{
				invoker = ptr;
				//uniqueIdentifier = (size_t)ptr;
			}

			/**
			*@brief Create delegate from lambda.
			* Example code:
			* @code
			* void Call(const Delegate<void()>& del) { del();}
			* void foo(){cout << "Hello World" << endl;}
			*
			* Delegate<void()> del([](){ cout << "Hello World" << endl;});
			* del(); // Prints "Hello World"
			* Call([&del](){foo(); del();}); // Prints "Hello World" twice
			* @endcode
			*/
			template <typename T>
			Delegate(const T& lambda)
			{
				invoker = lambda;
			//	uniqueIdentifier = lambda.target_type().hash_code();
			}


			/**
			*@brief Create delegate from class method.
			*@param [in] instance The pointer to the class object. (Both this, and &myClass) works.
			*@param [in] TMethod The class method pointer. (&Class::Method)
			* Example code:
			* @code
			* void Call(const Delegate<void()>& del) { del();}
			* class A
			*{
			* public:
			* void foo(){cout << "Hello World" << endl;}
			*}
			*
			* A a;
			* Delegate<void()> del = {&a, &A::foo};
			* del(); // Prints "Hello World"
			* Call(del); // Prints "Hello World"
			* Call({&a, &A::foo}); // Prints "Hello World"
			* @endcode
			*/
			template <class T>
			Delegate(T* instance, RET(T::*TMethod)(PARAMS...))
			{
				invoker = [instance, TMethod](PARAMS... params) -> RET {
					T* p = static_cast<T*>(instance);
					return (instance->*TMethod)(std::forward<PARAMS>(params)...);
				};
				//union test
				//{
				//	size_t conv[2] = { 0, 0 };
				//	RET(T::*ptr)(PARAMS...);
				//};
				//test ptr;
				//ptr.ptr = TMethod;
				//if (ptr.conv[1])
				//	int i = 0;



				//uniqueIdentifier = (size_t)(instance) | ptr.conv[0];
				//	std::intptr_t b = reinterpret_cast<std::intptr_t>(TMethod);
				//	uniqueIdentifier = (size_t)instance | (size_t)TMethod;
			}


			/**
			*@brief Create delegate from const class method.
			*@param [in] instance The pointer to the class object. (Both this, and &myClass) works.
			*@param [in] TMethod The class method pointer. (&Class::Method)
			* Example code:
			* @code
			* void Call(const Delegate<void()>& del) { del();}
			* class A
			*{
			* public:
			* void foo()const{cout << "Hello World" << endl;}
			*}
			*
			* A a;
			* Delegate<void()> del = {&a, &A::foo};
			* del(); // Prints "Hello World"
			* Call(del); // Prints "Hello World"
			* Call({&a, &A::foo}); // Prints "Hello World"
			* @endcode
			*/
			template <class T>
			Delegate(const T* instance, RET(T::*TMethod)(PARAMS...) const)
			{
				invoker = [instance, TMethod](PARAMS... params) -> RET {
					T* const p = const_cast<T*>(instance);
					return (instance->*TMethod)(std::forward<PARAMS>(params)...);
				};
			}

			/**
			*@brief No equal operator for now.
			*/
			bool operator==(const Delegate& other)const = delete;
			bool operator+=(const Delegate& other)const = delete;
			bool operator-=(const Delegate& other)const = delete;
			bool operator+(const Delegate& other)const = delete;
			bool operator-(const Delegate& other)const = delete;

			/**
			*@brief Assignment from Delegate to Delegate.
			*/
			Delegate& operator=(const Delegate& other)
			{
				this->invoker = other.invoker;
				return *this;
			}

			/**
			*@brief Assignment from Delegate to Delegate, with rvalue.
			*/
			Delegate& operator=(const Delegate&& other)
			{
				this->invoker = std::move(other.invoker);
				return *this;
			}

			/**
			*@brief Invoke the delegate.
			*/
			inline RET operator()(PARAMS... args)const
			{
				return invoker(std::forward<PARAMS>(args)...);
			}
		};
	}
}
#endif // SE_UTILZ_DELEGATE_H_