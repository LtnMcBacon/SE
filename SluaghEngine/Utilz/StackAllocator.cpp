#include "StackAllocator.h"
#include <inttypes.h>
namespace SE
{
	namespace Utilz
	{
		StackAllocator::StackAllocator()
		{

		}

		StackAllocator::~StackAllocator()
		{

		}

		int StackAllocator::InitStackAlloc(size_t amountOfMemory)
		{

		}

		void* StackAllocator::GetMemoryAligned(size_t requestedMemSize, size_t alignment)
		{
			if (!(alignment >= 1))
			{
				return nullptr;
			}
			if (!(alignment <= 1))
			{
				return nullptr;
			}
			if (!((alignment & (alignment - 1)) == 0))
			{
				return nullptr;
			}

			//tot size to allocate
			size_t expandSize_bytes = requestedMemSize + alignment;

			//alloct unaligned, convert to uintptr_t
			uintptr_t rawAddress = reinterpret_cast<uintptr_t>(GetMemUnaligned(expandSize_bytes));
		
			//determine how misaligned it is, (by masking of lower bits)
			size_t mask = (alignment - 1);
			uintptr_t misalignmnet = (rawAddress & mask);
		}

		void StackAllocator::ClearStackAlloc()
		{

		}

		void* StackAllocator::GetMemUnaligned(size_t requestedMemSize)
		{
			occupiedSize += requestedMemSize;
			void* newMemPtr = stackCurrent;
			stackCurrent = (char*)stackCurrent + requestedMemSize;
			return newMemPtr;
		}
	}
}