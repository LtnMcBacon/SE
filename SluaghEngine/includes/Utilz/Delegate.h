#ifndef SE_UTILZ_DELEGATE_H_
#define SE_UTILZ_DELEGATE_H_
#include "DelegateBase.h"

namespace SE
{
	namespace Utilz
	{
		template <typename T> class Delegate;
		template <typename T> class Event;

		template<typename RET, typename ...PARAMS>
		class Delegate<RET(PARAMS...)> final : private Delegate_Base<RET(PARAMS...)> {
			friend class Event<RET(PARAMS...)>;
		public:

			Delegate() = default;

			/**
			*@brief Returns wether or not the delegate is nullptr.
			*@retval true Delegate is not nullptr
			*@retval false Delegate is nullptr
			*/
			bool isNull() const { return invocation.stub == nullptr; }

			/**
			*@brief Returns wether or not the delegate is nullptr.
			*@retval true Delegate is not nullptr
			*@retval false Delegate is nullptr
			*/
			bool operator ==(void* ptr) const {
				return (ptr == nullptr) && this->isNull();
			} 

			/**
			*@brief Returns wether or not the delegate is not nullptr.
			*@retval true Delegate is nullptr
			*@retval false Delegate is not nullptr
			*/
			bool operator !=(void* ptr) const {
				return (ptr != nullptr) || (!this->isNull());
			}

			/**
			*@brief Copy constructor.
			*/
			Delegate(const Delegate& another) { another.invocation.Clone(invocation); }

			/**
			*@brief Constructor for lambda.
			*/
			template <typename LAMBDA>
			Delegate(const LAMBDA& lambda) {
				assign((void*)(&lambda), lambda_stub<LAMBDA>);
			} 

			/**
			*@brief Assignment operator.
			*/
			Delegate& operator =(const Delegate& another) {
				another.invocation.Clone(invocation);
				return *this;
			} //operator =

			  /**
			  *@brief Assignment operator from lambda.
			  */
			template <typename LAMBDA> // template instantiation is not needed, will be deduced (inferred):
			Delegate& operator =(const LAMBDA& instance) {
				assign((void*)(&instance), lambda_stub<LAMBDA>);
				return *this;
			} //operator =


			  /**
			  *@brief Equal operator.
			  */
			bool operator == (const Delegate& another) const { return invocation == another.invocation; }
			/**
			*@brief Inequality operator.
			*/
			bool operator != (const Delegate& another) const { return invocation != another.invocation; }

			/**
			*@brief Equal operator for Event.
			*/
			bool operator ==(const Event<RET(PARAMS...)>& another) const { return another == (*this); }

			/**
			*@brief Inequality operator for event.
			*/
			bool operator !=(const Event<RET(PARAMS...)>& another) const { return another != (*this); }

		

			/**
			*@brief Creates a Delegate from a class method.
			*@param [in] T The class type
			*@param [in] TMethod The function pointer
			*@param [in] instance The pointer to the object
			* Example code:
			* @code
			*		class A
			*{
			*private:
			*	int foo() { return 2; };
			*public:
			*	void Register(Delegate<int()>& del)
			*	{
			*		del = Delegate<int()>::Make<A, &A::foo>(this);
			*	}
			*};
			*
			*A a;
			*Deletgate<int()> del;
			*a.Register(del);
			*
			*cout << del(); // Prints 2;
			* @endcode
			*/
			template <class T, RET(T::*TMethod)(PARAMS...)>
			inline static Delegate Make(T* instance) {
				return Delegate(instance, method_stub<T, TMethod>);
			}

			/**
			*@brief Creates a Delegate from a const class method.
			*@param [in] T The class type
			*@param [in] TMethod The function pointer
			*@param [in] instance The pointer to the object
			* Example code:
			* @code
			*		class A
			*{
			*private:
			*	int foo()const { return 2; };
			*public:
			*	void Register(Delegate<int()>& del)
			*	{
			*		del = Delegate<int()>::Make<A, &A::foo>(this);
			*	}
			*};
			*
			*A a;
			*Deletgate<int()> del;
			*a.Register(del);
			*
			*cout << del(); // Prints 2;
			* @endcode
			*/
			template <class T, RET(T::*TMethod)(PARAMS...) const>
			static Delegate Make(T const* instance) {
				return Delegate(const_cast<T*>(instance), const_method_stub<T, TMethod>);
			} 

			/**
			*@brief Creates a Delegate from function.
			*@param [in] TMethod The function pointer
			* Example code:
			* @code
			* int foo(){return 2;}
			*Deletgate<int()> del;
			*del = Delegate<int()>::Make<&foo>();
			*
			*cout << del(); // Prints 2;
			* @endcode
			*/
			template <RET(*TMethod)(PARAMS...)>
			static Delegate Make() {
				return Delegate(nullptr, function_stub<TMethod>);
			}

			/**
			*@brief Creates a Delegate from function.
			*@param [in] instance The lambda reference
			* Example code:
			* @code
			*Deletgate<int()> del;
			*del = [](){return 2;};
			*
			*cout << del(); // Prints 2;
			* @endcode
			*/
			template <typename LAMBDA>
			static Delegate Make(const LAMBDA & instance) {
				return Delegate((void*)(&instance), lambda_stub<LAMBDA>);
			} //create


			  /**
			  *@brief Invokes the delegate
			  *@param [in] arg The parameter list
			  */
			RET operator()(PARAMS... arg) const {
				return (*invocation.stub)(invocation.object, arg...);
			} //operator()

		private:

			Delegate(void* anObject, typename Delegate_Base<RET(PARAMS...)>::stub_type aStub) {
				invocation.object = anObject;
				invocation.stub = aStub;
			} //Delegate

			void assign(void* anObject, typename Delegate_Base<RET(PARAMS...)>::stub_type aStub) {
				this->invocation.object = anObject;
				this->invocation.stub = aStub;
			} //assign

			template <class T, RET(T::*TMethod)(PARAMS...)>
			static RET method_stub(void* this_ptr, PARAMS... params) {
				T* p = static_cast<T*>(this_ptr);
				return (p->*TMethod)(params...);
			} //method_stub

			template <class T, RET(T::*TMethod)(PARAMS...) const>
			static RET const_method_stub(void* this_ptr, PARAMS... params) {
				T* const p = static_cast<T*>(this_ptr);
				return (p->*TMethod)(params...);
			} //const_method_stub

			template <RET(*TMethod)(PARAMS...)>
			static RET function_stub(void* this_ptr, PARAMS... params) {
				return (TMethod)(params...);
			} //function_stub

			template <typename LAMBDA>
			static RET lambda_stub(void* this_ptr, PARAMS... arg) {
				LAMBDA* p = static_cast<LAMBDA*>(this_ptr);
				return (p->operator())(arg...);
			} //lambda_stub

			
			typename Delegate_Base<RET(PARAMS...)>::InvocationElement invocation;

		};
	}
}
#endif // SE_UTILZ_DELEGATE_H_