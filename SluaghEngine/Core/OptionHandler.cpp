#include <Core\OptionHandler.h>
#include <Utilz\INIReader.h>
#include <Profiler.h>

#ifdef _DEBUG
#pragma comment(lib, "UtilzD.lib")
#else
#pragma comment(lib, "Utilz.lib")
#endif

namespace SE {
	namespace Core {
		OptionHandler::OptionHandler()
		{

		}

		OptionHandler::~OptionHandler()
		{

		}

		int OptionHandler::Initialize(const std::string& filename)
		{
			StartProfile;
			Utilz::INIReader reader;
			ProfileReturn(reader.LoadINI(filename, optionMap));
		}

		int OptionHandler::GetOption(const std::string& section, const std::string& optionName, int defaultVal)
		{
			StartProfile;
			for (auto& mapsPair : optionMap)
			{
				if (mapsPair.first == section)
				{
					std::map<std::string, int> values = optionMap[mapsPair.first];
					for (auto& valuesPair : values)
					{
						if (valuesPair.first == optionName)
						{
							ProfileReturn(values[valuesPair.first]);
						}
					}
				}
			}
			ProfileReturnConst(defaultVal);
		}

		void OptionHandler::SetOption(const std::string& section, const std::string& optionName, int newValue)
		{
			StartProfile;
			for (auto& mapsPair : optionMap)
			{
				if (mapsPair.first == section)
				{
					std::map<std::string, int> values = optionMap[mapsPair.first];
					for (auto& valuesPair : values)
					{
						if (valuesPair.first == optionName)
						{
							values[valuesPair.first] = newValue;
							ProfileReturnVoid
						}
					}
				}
			}
			ProfileReturnVoid;
		}

		int OptionHandler::UnloadOption(const std::string& filename)
		{
			StartProfile;
			Utilz::INIReader reader;
			ProfileReturn(reader.WriteToINI(filename, optionMap));
		}

		void OptionHandler::Register(const Utilz::Delegate<void()>& delegat)
		{
			StartProfile;
			optionEvent.Add(delegat);
			ProfileReturnVoid;
		}

		void OptionHandler::Trigger()
		{
			StartProfile;
			optionEvent();
			ProfileReturnVoid;
		}

	}	//namespace Core
}	//namespace SE