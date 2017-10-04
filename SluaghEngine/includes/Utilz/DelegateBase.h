#ifndef SE_UTILZ_Delegate_BASE_H_
#define SE_UTILZ_Delegate_BASE_H_
namespace SE
{
	namespace Utilz
	{
		template<typename T>
		class Delegate_Base;

		template<typename RET, typename ...PARAMS>
		class Delegate_Base<RET(PARAMS...)> {

		protected:

			using stub_type = RET(*)(void* this_ptr, PARAMS...);

			struct InvocationElement {
				InvocationElement() = default;
				InvocationElement(void* this_ptr, stub_type aStub) : object(this_ptr), stub(aStub) {}
				void Clone(InvocationElement& target) const {
					target.stub = stub;
					target.object = object;
				} //Clone
				bool operator ==(const InvocationElement& another) const {
					return another.stub == stub && another.object == object;
				} //==
				bool operator !=(const InvocationElement& another) const {
					return another.stub != stub || another.object != object;
				} //!=
				void* object = nullptr;
				stub_type stub = nullptr;
			};

		};
	}
}
#endif // SE_UTILZ_Delegate_BASE_H_