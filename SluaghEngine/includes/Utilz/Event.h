#ifndef SE_UTILZ_EVENT_H_
#define SE_UTILZ_EVENT_H_
#include "Delegate.h"
#include <list>
#include <functional>

namespace SE
{
	namespace Utilz
	{
		template<typename RET, typename ...PARAMS>

		/**
		* @brief Invokes all registered functions(delegates).
		* @sa Delegate
		*/
		class Event<RET(PARAMS...)> final : private Delegate_Base<RET(PARAMS...)> {
		public:

			Event() = default;
			~Event() {
				for (auto& element : invocationList) delete element;
				invocationList.clear();
			} 

			/**
			*@brief Is the event empty.
			*/
			bool isNull() const { return invocationList.size() < 1; }

			/**
			*@brief Returns wether or not the event is nullptr.
			*@retval true Delegate is nullptr
			*@retval false Delegate is not nullptr
			*/
			bool operator ==(void* ptr) const {
				return (ptr == nullptr) && this->isNull();
			} 

			/**
			*@brief Returns wether or not the delegate is not nullptr.
			*@retval true Delegate is not nullptr
			*@retval false Delegate is nullptr
			*/
			bool operator !=(void* ptr) const {
				return (ptr != nullptr) || (!this->isNull());
			}

			/**
			*@brief Returns the number of Delegates registered.
			*/
			size_t size() const { return invocationList.size(); }

			/**
			*@brief Adds the Delegates from one Event to another.
			*/
			Event& operator =(const Event& other)
			{
				for (auto& item : other.invocationList) // clone, not copy; flattens hierarchy:
					this->invocationList.push_back(new typename Delegate_Base<RET(PARAMS...)>::InvocationElement(item->object, item->stub));
				return *this;
			}


			/**
			*@brief Adds the Delegates from one Event to another(Copy Constructor).
			*/
			Event(const Event& other)
			{
				for (auto& item : other.invocationList) // clone, not copy; flattens hierarchy:
					this->invocationList.push_back(new typename Delegate_Base<RET(PARAMS...)>::InvocationElement(item->object, item->stub));
			}

			/**
			*@brief Are the two events the same.
			*/
			bool operator ==(const Event& another) const {
				if (invocationList.size() != another.invocationList.size()) return false;
				auto anotherIt = another.invocationList.begin();
				for (auto it = invocationList.begin(); it != invocationList.end(); ++it)
					if (**it != **anotherIt) return false;
				return true;
			} 
			/**
			*@brief Are the two events not the same.
			*/
			bool operator !=(const Event& another) const { return !(*this == another); }

			/**
			*@brief If the first Delegate in the event is the same delegate.
			*/
			bool operator ==(const Delegate<RET(PARAMS...)>& another) const {
				if (isNull() && another.isNull()) return true;
				if (another.isNull() || (size() != 1)) return false;
				return (another.invocation == **invocationList.begin());
			} 
			bool operator !=(const Delegate<RET(PARAMS...)>& another) const { return !(*this == another); }


			/**
			*@brief Adds the Delegates from one Event to another.
			*/
			Event& operator +=(const Event& another) {
				for (auto& item : another.invocationList) // clone, not copy; flattens hierarchy:
					this->invocationList.push_back(new typename Delegate_Base<RET(PARAMS...)>::InvocationElement(item->object, item->stub));
				return *this;
			} 

			/**
			*@brief Adds the lambda from to the event
			*/
			template <typename LAMBDA> // template instantiation is not neededm, will be deduced/inferred:
			Event& operator +=(const LAMBDA & lambda) {
				Delegate<RET(PARAMS...)> d = Delegate<RET(PARAMS...)>::template create<LAMBDA>(lambda);
				return *this += d;
			} 

			/**
			*@brief Adds the Delegate to the event.
			*/
			Event& operator +=(const Delegate<RET(PARAMS...)>& another) {
				if (another.isNull()) return *this;
				this->invocationList.push_back(new typename Delegate_Base<RET(PARAMS...)>::InvocationElement(another.invocation.object, another.invocation.stub));
				return *this;
			} 

			/**
			*@brief Invokes all registered functions.
			* will work even if RET is void, return values are ignored:
			* (for handling return values, see operator(..., handler))
			*@param [in] arg The parameter list
			*/
			void operator()(PARAMS... arg) const {
				for (auto& item : invocationList)
					(*(item->stub))(item->object, arg...);
			} 

			/**
			*@brief Invokes all registered functions and provides a handler for returns.
			*@param [in] arg The parameter list
			*@param [in] handler The handler function for handling each return.
			*/
			template<typename HANDLER>
			void operator()(PARAMS... arg, HANDLER handler) const {
				size_t index = 0;
				for (auto& item : invocationList) {
					RET value = (*(item->stub))(item->object, arg...);
					handler(index, &value);
					++index;
				} //loop
			} //operator()



			  /**
			  *@brief Invokes all registered functions and provides a handler for returns.
			  *@param [in] arg The parameter list
			  *@param [in] handler The handler function for handling each return.
			  */
			void operator()(PARAMS... arg, Delegate<void(size_t, RET*)> handler) const {
				operator() < decltype(handler) > (arg..., handler);
			}


			/**
			*@brief Invokes all registered functions and provides a handler for returns.
			*@param [in] arg The parameter list
			*@param [in] handler The handler function for handling each return.
			*/
			void operator()(PARAMS... arg, std::function<void(size_t, RET*)> handler) const {
				operator() < decltype(handler) > (arg..., handler);
			} 




			/**
			*@brief Adds the delegate to the event.
			*@param [in] another The Delegate to register
			* Example code:
			* @code
			*		class A
			*{
			*private:
			*	Event<int()> event;
			*public:
			*	void Register(const Delegate<int()>&& del)
			*	{
			*		event.Add(del);
			*		event += del; // Also works!!
			*	}
			*};
			*int foo(){return 2;};
			*A a;
			*
			*a.Register(Delegate<int()>::Make<&foo>());
			*
			* @endcode
			*/
			void Add(const Delegate<RET(PARAMS...)>& another) {
				if (another.isNull()) return;
				this->invocationList.push_back(new typename Delegate_Base<RET(PARAMS...)>::InvocationElement(another.invocation.object, another.invocation.stub));
			}

			/**
			*@brief Adds the class method to the event.
			*@param [in] another The Delegate to register
			* Example code:
			* @code
			*		class A
			*{
			*public:
			*	int foo()
			*	{
			*		return 2;
			*	}
			*};
			*A a;
			*
			*Event<int()> event;
			*event.Add<A, &A::foo>(&a);
			*
			* @endcode
			*/
			template <class T, RET(T::*TMethod)(PARAMS...)>
			void Add(T* instance) {
				operator+=(Delegate<RET(PARAMS...)>::Make<T, TMethod>(instance));
			}

			/**
			*@brief Adds the class method to the event.
			*@param [in] another The Delegate to register
			* Example code:
			* @code
			*		class A
			*{
			*public:
			*	int foo()const
			*	{
			*		return 2;
			*	}
			*};
			*A a;
			*
			*Event<int()> event;
			*event.Add<A, &A::foo>(&a);
			* event += Delgate<int()>::Make<A, &A::foo>(&a); // Works as well.
			*
			* @endcode
			*/
			template <class T, RET(T::*TMethod)(PARAMS...) const>
			void Add(T const* instance) {
				operator+=(Delegate<RET(PARAMS...)>::Make<T, TMethod>(instance));
			}

			/**
			*@brief Adds the function to the event.
			*@param [in] another The Delegate to register
			* Example code:
			* @code
			* int foo(){return 2;};
			*
			*Event<int()> event;
			*event.Add<&foo>();
			*event += Delegate<int()>::Make<&foo>(); // Works as well
			* @endcode
			*/
			template <RET(*TMethod)(PARAMS...)>
			void Add() {
				operator+=(Delegate<RET(PARAMS...)>::Make<TMethod>(nullptr));
			}

			/**
			*@brief Adds the lambda to the event.
			*@param [in] another The Delegate to register
			* Example code:
			* @code
			* auto lam = [](){return 2;};
			*Event<int()> event;
			*event.Add([](){return 2;});
			*event += [](){return 2;}; // Works as well
			*event.Add(lam); // Works as well.
			*event += lam; // Works as well.
			* @endcode
			*/
			template <typename LAMBDA>
			void Add(const LAMBDA & instance) {
				operator+=(Delegate<RET(PARAMS...)>::Make<LAMBDA>(instance));
			} //create

		private:

			std::list<typename Delegate_Base<RET(PARAMS...)>::InvocationElement *> invocationList;

		}; 

	}
}
#endif //SE_UTILZ_EVENT_H_