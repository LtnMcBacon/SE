#pragma once
#ifndef __SE_UTILZ_CIRCULAR_FIFO_H__
#define __SE_UTILZ_CIRCULAR_FIFO_H__

namespace SE
{
	namespace Utilz
	{
		template<typename Element, size_t size = 256>
		class CircularFiFo
		{
		public:
			enum { Capacity = size + 1 };
			CircularFiFo() {};
			~CircularFiFo() {};

			inline bool WasFull() const
			{
				auto index = tail;
				auto next_tail = (index + 1) % Capacity;
				return (next_tail == head);
			}

			inline bool wasEmpty() const
			{
				return (head == tail);
			}

			//Consumer only
			inline bool pop(Element& item)
			{
				const auto current_head = head;
				if (current_head == tail)
					return false;   // empty queue

				item = buffer[current_head];
				head = (current_head + 1) % Capacity;
				return true;
			}

			// Producer only
			inline bool push(Element& item)
			{
				auto current_tail = tail;
				auto next_tail = (current_tail) + 1 % Capacity;
				if (next_tail != head)
				{
					buffer[current_tail] = item;
					tail = next_tail;
					return true;
				}

				return false;  // full queue
			}

		private:
			Element buffer[Capacity];
			size_t head = 0;
			size_t tail = 0;
		};
	}	//namespace SE
}	//namespace Utilz

#endif  // __SE_UTILZ_CIRCULAR_FIFO_H__