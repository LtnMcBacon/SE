#pragma once
#ifndef SE_UTILZ_DOUBLE_EDGE_STACK_ALLOCATOR_H_
#define SE_UTILZ_DOUBLE_EDGE_STACK_ALLOCATOR_H_

namespace SE
{
	namespace Utilz
	{
		class DoubleEdgeStackAlloc
		{
		public:
			DoubleEdgeStackAlloc();
			~DoubleEdgeStackAlloc();
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
			void* GetMemoryAlignedLeft(size_t requestedMemSize, size_t aligment);
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
			void* GetMemoryAlignedRight(size_t requestedMemSize, size_t aligment);
			/**
			* @brief Clears the allocated memory
			*
			*/
			void ClearStackAllocLeft();
			/**
			* @brief Clears the allocated memory
			*
			*/
			void ClearStackAllocRight();

		private:
			void* GetMemUnalignedLeft(size_t requestedMemSize);
			void* GetMemUnalignedRight(size_t requestedMemSize);
			void* stackBeg;
			void* stackCurrentLeft;
			void* stackCurrentRight;
			size_t stackSize;
			size_t occupiedSizeLeft;
			size_t occupiedSizeRight;
		};
	}
}

#endif // end SE_UTILZ_DOUBLE_EDGE_STACK_ALLOCATOR_H_