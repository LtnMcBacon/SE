#pragma once
#ifndef SE_UTILZ_DOUBLE_EDGE_STACK_ALLOCATOR_H_
#define SE_UTILZ_DOUBLE_EDGE_STACK_ALLOCATOR_H_
#include <stack>
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
			/**
			* @brief Get a pointer to current left pos
			*
			* @retval void* Pointer to current left pos
			*
			*/
			void* GetMarkerLeft();
			/**
			* @brief Get a pointer to current right pos
			*
			* @retval void* Pointer to current right pos
			*
			*/
			void* GetMarkerRight();
			/**
			* @brief Clears to marker if it was the last created marker
			*
			* @retval void* Marker pointer
			*
			*/
			int ClearToMarkerLeft(void* marker);
			/**
			* @brief Clears to marker if it was the last created marker
			*
			* @retval void* Marker pointer
			*
			*/
			int ClearToMarkerRight(void* marker);

		private:
			void* GetMemUnalignedLeft(size_t requestedMemSize);
			void* GetMemUnalignedRight(size_t requestedMemSize);
			void* stackBeg;
			void* stackCurrentLeft;
			void* stackCurrentRight;
			size_t stackSize;
			size_t occupiedSizeLeft;
			size_t occupiedSizeRight;
			std::stack<void*> takenMarkersLeft;
			std::stack<void*> takenMarkersRight;
		};
	}
}

#endif // end SE_UTILZ_DOUBLE_EDGE_STACK_ALLOCATOR_H_