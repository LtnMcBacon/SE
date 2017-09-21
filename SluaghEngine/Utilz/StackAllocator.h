#pragma once
#ifndef SE_UTILZ_STACKALLOCATOR_H_
#define SE_UTILZ_STACKALLOCATOR_H_

namespace SE
{
	namespace Utilz
	{
		class StackAllocator
		{
		public:
			StackAllocator();
			~StackAllocator();
			int InitStackAlloc(size_t amountOfMemory);
			void* GetMemoryAligned(size_t requestedMemSize, size_t aligment);
			void ClearStackAlloc();

		private:
			void* GetMemUnaligned(size_t requestedMemSize);
			void* stackBeg;
			void* stackCurrent;
			size_t stackSize;
			size_t occupiedSize;
		};
	}
}

#endif // end SE_UTILZ_STACKALLOCATOR_H_