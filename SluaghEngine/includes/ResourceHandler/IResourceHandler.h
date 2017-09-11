#ifndef SE_RESOURCE_HANDLER_IRESOURCE_HANDLER_H_
#define SE_RESOURCE_HANDLER_IRESOURCE_HANDLER_H_
#include <Utilz\GUID.h>
#include <functional>
namespace SE
{
	namespace ResourceHandler
	{
		class IResourceHandler
		{
		public:
			virtual ~IResourceHandler() = 0;


			virtual int Initialize() = 0;

			virtual void LoadResource(const Utilz::GUID& guid, const std::function<void(void* data, size_t size)>& callback) = 0;

		protected:
			IResourceHandler() {};
			IResourceHandler(const IResourceHandler& other) = delete;
			IResourceHandler(const IResourceHandler&& other) = delete;
			IResourceHandler& operator=(const IResourceHandler& other) = delete;
		};
	}
}


#endif //SE_RESOURCE_HANDLER_IRESOURCE_HANDLER_H_