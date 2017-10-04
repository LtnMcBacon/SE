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

			bool isNull() const { return invocation.stub == nullptr; }
			bool operator ==(void* ptr) const {
				return (ptr == nullptr) && this->isNull();
			} //operator ==
			bool operator !=(void* ptr) const {
				return (ptr != nullptr) || (!this->isNull());
			} //operator !=

			Delegate(const Delegate& another) { another.invocation.Clone(invocation); }

			template <typename LAMBDA>
			Delegate(const LAMBDA& lambda) {
				assign((void*)(&lambda), lambda_stub<LAMBDA>);
			} //Delegate

			Delegate& operator =(const Delegate& another) {
				another.invocation.Clone(invocation);
				return *this;
			} //operator =

			template <typename LAMBDA> // template instantiation is not needed, will be deduced (inferred):
			Delegate& operator =(const LAMBDA& instance) {
				assign((void*)(&instance), lambda_stub<LAMBDA>);
				return *this;
			} //operator =

			bool operator == (const Delegate& another) const { return invocation == another.invocation; }
			bool operator != (const Delegate& another) const { return invocation != another.invocation; }

			bool operator ==(const Event<RET(PARAMS...)>& another) const { return another == (*this); }
			bool operator !=(const Event<RET(PARAMS...)>& another) const { return another != (*this); }

			template <class T, RET(T::*TMethod)(PARAMS...)>
			static Delegate Make(T* instance) {
				return Delegate(instance, method_stub<T, TMethod>);
			}

			template <class T, RET(T::*TMethod)(PARAMS...) const>
			static Delegate Make(T const* instance) {
				return Delegate(const_cast<T*>(instance), const_method_stub<T, TMethod>);
			} 

			template <RET(*TMethod)(PARAMS...)>
			static Delegate Make() {
				return Delegate(nullptr, function_stub<TMethod>);
			}

			template <typename LAMBDA>
			static Delegate Make(const LAMBDA & instance) {
				return Delegate((void*)(&instance), lambda_stub<LAMBDA>);
			} //create

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