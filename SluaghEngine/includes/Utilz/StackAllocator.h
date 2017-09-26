#pragma once
#ifndef SE_UTILZ_STACKALLOCATOR_H_
#define SE_UTILZ_STACKALLOCATOR_H_
#include <stack>

namespace SE
{
	namespace Utilz
	{
		class StackAllocator
		{
		public:
			StackAllocator();
			~StackAllocator();
			/**
			* @brief Gets a piece of memory to distribute
			*
			* @param[in] amountOfMemory The requested size for the allocator
			*
			*/
			void InitStackAlloc(size_t amountOfMemory);
			/**
			* @brief Get a pointer to memory with requested size
			*
			* @param[in] requestedMemSize The requested size for the pointer
			* @param[in] aligment The size of the data type
			*
			* @retval void* Pointer to memory
			* @retval nullptr Tells that creation of pointer to memory 
			* was unsucessful (not enough memory left)
			*
			*/
			void* GetMemoryAligned(size_t requestedMemSize, size_t aligment);
			/**
			* @brief Clears the allocated memory 
			*
			*/
			void ClearStackAlloc();
			/**
			* @brief Get a pointer to current pos
			*
			* @retval void* Pointer to current pos
			*
			*/
			void* GetMarker();
			/**
			* @brief Clears to marker if it was the last created marker
			*
			* @retval void* Marker pointer
			*
			*/
			int ClearToMarker(void* marker);

		private:
			void* GetMemUnaligned(size_t requestedMemSize);
			void* stackBeg;
			void* stackCurrent;
			size_t stackSize;
			size_t occupiedSize;
			std::stack<void*> takenMarkers;
		};
	}
}

#endif // end SE_UTILZ_STACKALLOCATOR_H_