#include <Core/EntityManager.h>
#include <Utilz\Console.h>
#include <Utilz\CMDConsole.h>
#include <iostream>

#ifdef _DEBUG
#pragma comment(lib, "coreD.lib")
#pragma comment(lib, "UtilzD.lib")
#else
#pragma comment(lib, "core.lib")
#pragma comment(lib, "Utilz.lib")
#endif

using namespace SE::Core;
using namespace SE::Utilz;
int main(int argc, char** argv)
{
	bool running = true;
	Console::Initialize(new CMDConsole);
	Console::AddCommand([&running](IConsoleBackend* backend, int argc, char** argv)
	{
		running = false;
	
	},
		"exit",
		"exit the application");

	//Console::Show();

	EntityManager em;


	Entity e[2048];

	for (int i = 0; i < 2048; i++)
	{
		e[i] = em.Create();
	}

	for (int i = 0; i < 1024; i++)
	{
		em.Destroy(e[i]);
	}
	for (int i = 0; i < 1024; i++)
	{
		if (em.Alive(e[i]))
			std::cout << i << ": " << e[i].id << "\n";
	}
	for (int i = 1024; i < 2048; i++)
	{
		if (!em.Alive(e[i]))
			std::cout << "Dead: " << e[i].id << "\n";
	}
	for (int i = 0; i < 1024; i++)
	{
		e[i] = em.Create();
	}

	for (int i = 0; i < 2048; i++)
	{
		if (!em.Alive(e[i]))
			std::cout << "Is dead: " << i << "\n";
	}
	for (int x = 0; x < 120; x++)
	{
		for (int i = 0; i < 2048; i++)
		{
			em.Destroy(e[i]);
		}
		for (int i = 0; i < 2048; i++)
		{
			e[i] = em.Create();
		}
	}

	for (int i = 0; i < 2048; i++)
	{
		for (int j = i + 1; j < 2048; j++)
		{
			if (e[i].id == e[j].id)
				std::cout << "Duplicate detected\n";
		}
	}



	while (running);

	return 0;

}