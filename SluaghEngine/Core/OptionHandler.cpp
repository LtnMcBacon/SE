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
			if (optionMap.size() > 0)
			{
				for (auto& mapsPair : optionMap)
				{
					if (mapsPair.first == section)
					{
						std::map<std::string, int> values = optionMap[mapsPair.first];
						auto fileLoaded = values.find(optionName);
						if (fileLoaded == values.end())
						{
							SetOption(section, optionName, defaultVal);
							ProfileReturnConst(defaultVal);
						}
						else
						{
							ProfileReturn(values[optionName]);
						}
					}
				}
			}
			SetOption(section, optionName, defaultVal);
			ProfileReturnConst(defaultVal);
		}

		void OptionHandler::SetOption(const std::string& section, const std::string& optionName, int newValue)
		{
			StartProfile;
			auto fileLoaded = optionMap.find(section);
			if (fileLoaded == optionMap.end())
			{
				std::map<std::string, int> value;
				value[optionName] = newValue;
				optionMap[section] = value;
				ProfileReturnVoid;
			}
			else
			{
				std::map<std::string, int> &values = optionMap[section];
				values[optionName] = newValue;
				ProfileReturnVoid
			}
			ProfileReturnVoid;
		}

		int OptionHandler::UnloadOption(const std::string& filename)
		{
			StartProfile;
			if (optionMap.size() > 0)
			{
				Utilz::INIReader reader;
				ProfileReturn(reader.WriteToINI(filename, optionMap));
			}
			ProfileReturnConst(-1);
		}

		void OptionHandler::Register(const Utilz::Delegate<void()>& delegat)
		{
			StartProfile;
			optionEvent += delegat;
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