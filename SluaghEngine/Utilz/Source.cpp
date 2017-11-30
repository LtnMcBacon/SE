#include <Delegate.h>
#include <GUID.h>

int load(const SE::Utilz::GUID& guid, void* data, size_t size)
{
	return 0;
}
int main()
{
	SE::Utilz::Delegate<int(const SE::Utilz::GUID& guid, void* data, size_t size)> del(&load);
	del("test", nullptr, 0);
	return 0;
}