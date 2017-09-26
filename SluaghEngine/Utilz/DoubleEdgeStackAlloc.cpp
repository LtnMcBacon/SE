#include "DoubleEdgeStackAlloc.h"
#include <inttypes.h>
#include <stdlib.h>
#include <assert.h>

namespace SE
{
	namespace Utilz
	{
		DoubleEdgeStackAlloc::DoubleEdgeStackAlloc()
		{

		}

		DoubleEdgeStackAlloc::~DoubleEdgeStackAlloc()
		{
			free(stackBeg);
		}

		void DoubleEdgeStackAlloc::InitStackAlloc(size_t amountOfMemory)
		{
			stackBeg = malloc(amountOfMemory);
			stackCurrentLeft = stackBeg;
			stackCurrentRight = (char*)stackBeg + amountOfMemory;
			stackSize = amountOfMemory;
		}

		void* DoubleEdgeStackAlloc::GetMemoryAlignedLeft(size_t requestedMemSize, size_t alignment)
		{
			assert(alignment >= 1);
			assert(alignment <= 128);
			assert((alignment & (alignment - 1)) == 0);


			//tot size to allocate
			size_t expandSize_bytes = requestedMemSize + alignment;

			//alloct unaligned, convert to uintptr_t
			void* rawVoidptr = GetMemUnalignedLeft(expandSize_bytes);
			if (rawVoidptr == nullptr)
			{
				return nullptr;
			}
			uintptr_t rawAddress = reinterpret_cast<uintptr_t>(rawVoidptr);

			//determine how misaligned it is, (by masking of lower bits)
			size_t mask = (alignment - 1);
			uintptr_t misalignmnet = (rawAddress & mask);
			ptrdiff_t adjustment = alignment - misalignmnet;

			//calc adjusted address
			uintptr_t alignedAddress = rawAddress + adjustment;

			//store adjustment in byte before the aligned ptr
			if (!(adjustment < 256))
			{
				return nullptr;
			}
			uint8_t* pAlignedMem = reinterpret_cast<uint8_t*>(alignedAddress);
			pAlignedMem[-1] = static_cast<uint8_t>(adjustment);

			return static_cast<void*>(pAlignedMem);
		}

		void* DoubleEdgeStackAlloc::GetMemoryAlignedRight(size_t requestedMemSize, size_t alignment)
		{
			assert(alignment >= 1);
			assert(alignment <= 128);
			assert((alignment & (alignment - 1)) == 0);


			//tot size to allocate
			size_t expandSize_bytes = requestedMemSize + alignment;

			//alloct unaligned, convert to uintptr_t
			void* rawVoidptr = GetMemUnalignedRight(expandSize_bytes);
			if (rawVoidptr == nullptr)
			{
				return nullptr;
			}
			uintptr_t rawAddress = reinterpret_cast<uintptr_t>(rawVoidptr);

			//determine how misaligned it is, (by masking of lower bits)
			size_t mask = (alignment - 1);
			uintptr_t misalignmnet = (rawAddress & mask);
			ptrdiff_t adjustment = alignment - misalignmnet;

			//calc adjusted address
			uintptr_t alignedAddress = rawAddress + adjustment;

			//store adjustment in byte before the aligned ptr
			if (!(adjustment < 256))
			{
				return nullptr;
			}
			uint8_t* pAlignedMem = reinterpret_cast<uint8_t*>(alignedAddress);
			pAlignedMem[-1] = static_cast<uint8_t>(adjustment);

			return static_cast<void*>(pAlignedMem);
		}

		void DoubleEdgeStackAlloc::ClearStackAllocLeft()
		{
			stackCurrentLeft = stackBeg;
			occupiedSizeLeft = 0;
		}

		void DoubleEdgeStackAlloc::ClearStackAllocRight()
		{
			stackCurrentRight = (char*)stackBeg + stackSize;
			occupiedSizeRight = 0;
		}

		void* DoubleEdgeStackAlloc::GetMemUnalignedLeft(size_t requestedMemSize)
		{
			if (occupiedSizeLeft + occupiedSizeRight + requestedMemSize > stackSize)
			{
				return nullptr;
			}
			occupiedSizeLeft += requestedMemSize;
			void* newMemPtr = stackCurrentLeft;
			stackCurrentLeft = (char*)stackCurrentLeft + requestedMemSize;
			return newMemPtr;
		}

		void* DoubleEdgeStackAlloc::GetMemUnalignedRight(size_t requestedMemSize)
		{
			if (occupiedSizeLeft + occupiedSizeRight + requestedMemSize > stackSize)
			{
				return nullptr;
			}
			occupiedSizeRight += requestedMemSize;
			stackCurrentRight = (char*)stackCurrentRight - requestedMemSize;
			return stackCurrentRight;
		}
	
		void* DoubleEdgeStackAlloc::GetMarkerLeft()
		{
			takenMarkersLeft.push(stackCurrentLeft);
			return stackCurrentLeft;
		}
		
		void* DoubleEdgeStackAlloc::GetMarkerRight()
		{
			takenMarkersRight.push(stackCurrentRight);
			return stackCurrentRight;
		}

		int DoubleEdgeStackAlloc::ClearToMarkerLeft(void* marker)
		{
			if (takenMarkersLeft.size() > 0)
			{
				if (marker == takenMarkersLeft.top())
				{
					stackCurrentLeft = marker;
					takenMarkersLeft.pop();
					return 0;
				}
			}
			return -1;
		}

		int DoubleEdgeStackAlloc::ClearToMarkerRight(void* marker)
		{
			if (takenMarkersRight.size() > 0)
			{
				if (marker == takenMarkersRight.top())
				{
					stackCurrentRight = marker;
					takenMarkersRight.pop();
					return 0;
				}
			}
			return -1;
		}
	}
}