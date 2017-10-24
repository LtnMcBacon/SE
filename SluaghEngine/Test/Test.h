#ifndef SE_TEST_TEST_H_
#define SE_TEST_TEST_H_
#include <DevConsole\IConsole.h>

namespace SE
{
	namespace Test
	{
		/**
		*
		* @brief The base test class
		*
		* @details All tests classes should inherit from this one.
		*
		**/
		class Test
		{
		public:
			/**
			* @brief	Starts the test
			*
			*
			* @param[in] parameter_name Parameter description
			*
			* @warning This is a warning
			*
			* Example code:
			* @code
			*	example usage of the function. Note that links will be automatically generated to documented entities
			* @endcode
			*/
			virtual bool Run(SE::DevConsole::IConsole* console) = 0;
		protected:
			Test() {};
			Test(const Test& other) = delete;
			Test(const Test&& other) = delete;
			Test& operator=(const Test& other) = delete;
		};
	}
}


#endif