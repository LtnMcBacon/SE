#include <Console.h>
#include <IConsoleBackend.h>
#include <Windows.h>
#include <cstdarg>
SE::Utilz::Console* SE::Utilz::Console::console = nullptr;


void SE::Utilz::Console::Run()
{
	char buffer[256];
	while (running)
	{
		backend->Getline(buffer, 256);
		InterpretCommand(buffer);

		Sleep(100);
	}
}

void SE::Utilz::Console::InterpretCommand(char * command)
{
	int argc = 0;
	char* argv[MAX_ARGUMENTS];
	ParseCommandString(command, &argc, argv);


	if (argc > 0)
	{
		uint32_t hash = (uint32_t)std::hash<std::string>{}(argv[0]);

		auto find = commands.find(hash);
		if (find != commands.end())
			find->second.commandFunction(backend, argc, argv);
		else
			backend->Print("Unknown command not found, %s\n\n", argv[0]);
	}

}

void SE::Utilz::Console::ParseCommandString(char * command, int * argc, char ** argv)
{
	*argc = 0;
	int j = 0;
	int i = 0;
	char c = command[i];
	while (c != '\0')
	{
		while (!(c == ' ' || c == '\0'))
		{
			i++;
			c = command[i];
		}
		argv[*argc] = &command[j];
		(*argc)++;
		if (command[i] != '\0')
		{
			command[i] = '\0';
			i++;
			j = i;
			c = command[i];
			if (*argc >= MAX_ARGUMENTS)
			{
				break;
			}

		}


	}
}

SE::Utilz::Console::Console()
{
	commands[std::hash<std::string>{}("commands")] =
	{
		[this](IConsoleBackend* backend, int argc, char** argv) 
	{
		
		backend->Print("\n\n");
		for (auto& c : commands)
		{
			
			backend->Print("%s\t-\t%s\n", c.second.name, c.second.description);
		}
		backend->Print("\n");
	},
		"commands",
		"Lists all available commands"
	};
}
SE::Utilz::Console::~Console()
{
}

int SE::Utilz::Console::Initialize(IConsoleBackend * backend)
{
	_ASSERT(!Console::console);
	_ASSERT(backend);
	
	int r = backend->Initialize();
	if (r != 0)
		return r;

	Console::console = new Console;
	Console::console->backend = backend;
	Console::console->thread = std::thread(&Console::Run, Console::console);

	return 0;
}

void SE::Utilz::Console::Shutdown()
{
	_ASSERT(Console::console);
	Console::console->backend->Shutdown();
	Console::console->running = false;
	Console::console->thread.join();
	delete Console::console->backend;
	delete Console::console;
}

void SE::Utilz::Console::Show()
{
	Console::console->backend->Show();
}

void SE::Utilz::Console::Hide()
{
	Console::console->backend->Hide();
}

/*Hash the identifier and add the command*/
int SE::Utilz::Console::AddCommand(const DevConsole_Command& commandFunction, char * name, char * description)
{
		uint32_t hash = (uint32_t)std::hash<std::string>{}(name);			
		Console::console->commands[hash] = { commandFunction, name ,description };
		return 0;
}

void SE::Utilz::Console::Print(const char * str, ...)
{
	va_list args;
	va_start(args, str);
	Console::console->backend->VPrint(str, args);
	va_end(args);
}
