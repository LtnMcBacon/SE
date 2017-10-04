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

			bool isNull() const { return invocationList.size() < 1; }
			bool operator ==(void* ptr) const {
				return (ptr == nullptr) && this->isNull();
			} 
			bool operator !=(void* ptr) const {
				return (ptr != nullptr) || (!this->isNull());
			}

			size_t size() const { return invocationList.size(); }

			Event& operator =(const Event& other)
			{
				for (auto& item : other.invocationList) // clone, not copy; flattens hierarchy:
					this->invocationList.push_back(new typename Delegate_Base<RET(PARAMS...)>::InvocationElement(item->object, item->stub));
				return *this;
			}
			Event(const Event& other)
			{
				for (auto& item : other.invocationList) // clone, not copy; flattens hierarchy:
					this->invocationList.push_back(new typename Delegate_Base<RET(PARAMS...)>::InvocationElement(item->object, item->stub));
			}

			bool operator ==(const Event& another) const {
				if (invocationList.size() != another.invocationList.size()) return false;
				auto anotherIt = another.invocationList.begin();
				for (auto it = invocationList.begin(); it != invocationList.end(); ++it)
					if (**it != **anotherIt) return false;
				return true;
			} 
			bool operator !=(const Event& another) const { return !(*this == another); }

			bool operator ==(const Delegate<RET(PARAMS...)>& another) const {
				if (isNull() && another.isNull()) return true;
				if (another.isNull() || (size() != 1)) return false;
				return (another.invocation == **invocationList.begin());
			} 
			bool operator !=(const Delegate<RET(PARAMS...)>& another) const { return !(*this == another); }

			Event& operator +=(const Event& another) {
				for (auto& item : another.invocationList) // clone, not copy; flattens hierarchy:
					this->invocationList.push_back(new typename Delegate_Base<RET(PARAMS...)>::InvocationElement(item->object, item->stub));
				return *this;
			} 

			template <typename LAMBDA> // template instantiation is not neededm, will be deduced/inferred:
			Event& operator +=(const LAMBDA & lambda) {
				Delegate<RET(PARAMS...)> d = Delegate<RET(PARAMS...)>::template create<LAMBDA>(lambda);
				return *this += d;
			} 

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

			void operator()(PARAMS... arg, Delegate<void(size_t, RET*)> handler) const {
				operator() < decltype(handler) > (arg..., handler);
			}
			void operator()(PARAMS... arg, std::function<void(size_t, RET*)> handler) const {
				operator() < decltype(handler) > (arg..., handler);
			} 

			template <class T, RET(T::*TMethod)(PARAMS...)>
			void Add(T* instance) {
				operator+=(Delegate<RET(PARAMS...)>::Make<T, TMethod>(instance));
			}

			template <class T, RET(T::*TMethod)(PARAMS...) const>
			void Add(T const* instance) {
				operator+=(Delegate<RET(PARAMS...)>::Make<T, TMethod>(instance));
			}


			template <RET(*TMethod)(PARAMS...)>
			void Add() {
				operator+=(Delegate<RET(PARAMS...)>::Make<TMethod>(nullptr));
			}

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