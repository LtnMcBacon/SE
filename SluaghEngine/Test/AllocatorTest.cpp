#include "AllocatorTest.h"
#include <Utilz\DoubleEdgeStackAlloc.h>

#ifdef _DEBUG
#pragma comment(lib, "utilzD.lib")
#else
#pragma comment(lib, "utilz.lib")
#endif

namespace SE
{
	namespace Test
	{
		AllocatorTest::AllocatorTest()
		{

		}
		AllocatorTest::~AllocatorTest()
		{

		}

		bool AllocatorTest::Run(SE::DevConsole::IConsole* console)
		{
			Utilz::DoubleEdgeStackAlloc alloc;
			alloc.InitStackAlloc(sizeof(float) * 1002);

			float* newfloat = (float*)alloc.GetMemoryAlignedLeft(sizeof(float) * 500, sizeof(float));

			if (newfloat == nullptr)
			{
				return false;
			}
			else
			{
				for (int i = 0; i < 500; i++)
				{
					newfloat[i]  = i;
				}
				/*for (int i = 0; i < 500; i++)
				{
					console->Print("%f \n", newfloat[i]);
				}*/
			}
				

			float* pizzaFloat = (float*)alloc.GetMemoryAlignedRight(sizeof(float) * 500, sizeof(float));

			if (pizzaFloat == nullptr)
			{
				return false;
			}
			else
			{
				for (int i = 0; i < 500; i++)
				{
					pizzaFloat[i] = i;
				}
				/*for (int i = 0; i < 500; i++)
				{
					console->Print("%f \n", pizzaFloat[i]);
				}*/
			}

			alloc.ClearStackAllocLeft();
			alloc.ClearStackAllocRight();

			newfloat = (float*)alloc.GetMemoryAlignedLeft(sizeof(float) * 500, sizeof(float));

			if (newfloat == nullptr)
			{
				return false;
			}
			else
			{
				for (int i = 0; i < 500; i++)
				{
					newfloat[i] = i;
				}
				/*for (int i = 0; i < 500; i++)
				{
					console->Print("%f \n", newfloat[i]);
				}*/
			}


			pizzaFloat = (float*)alloc.GetMemoryAlignedRight(sizeof(float) * 500, sizeof(float));

			if (pizzaFloat == nullptr)
			{
				return false;
			}
			else
			{
				for (int i = 0; i < 500; i++)
				{
					pizzaFloat[i] = i;
				}
				/*for (int i = 0; i < 500; i++)
				{
					console->Print("%f \n", pizzaFloat[i]);
				}*/
			}

			alloc.ClearStackAllocLeft();
			alloc.ClearStackAllocRight();

			return true;
		}
	}
}