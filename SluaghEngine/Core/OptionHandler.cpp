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

		int OptionHandler::GetOptionInt(const std::string& section, const std::string& optionName, int defaultVal)
		{
			StartProfile;
			if (optionMap.size() > 0)
			{
				for (auto& mapsPair : optionMap)
				{
					if (mapsPair.first == section)
					{
						std::map<std::string, std::string> values = optionMap[mapsPair.first];
						auto fileLoaded = values.find(optionName);
						if (fileLoaded == values.end())
						{
							SetOptionInt(section, optionName, defaultVal);
							ProfileReturnConst(defaultVal);
						}
						else
						{
							ProfileReturn(std::stoi(values[optionName]));
						}
					}
				}
			}
			SetOptionInt(section, optionName, defaultVal);
			ProfileReturnConst(defaultVal);
		}

		size_t OptionHandler::GetOptionUnsignedInt(const std::string& section, const std::string& optionName, size_t defaultVal)
		{
			StartProfile;
			if (optionMap.size() > 0)
			{
				for (auto& mapsPair : optionMap)
				{
					if (mapsPair.first == section)
					{
						std::map<std::string, std::string> values = optionMap[mapsPair.first];
						auto fileLoaded = values.find(optionName);
						if (fileLoaded == values.end())
						{
							SetOptionUnsignedInt(section, optionName, defaultVal);
							ProfileReturnConst(defaultVal);
						}
						else
						{
							ProfileReturn((size_t) std::stoi(values[optionName]));
						}
					}
				}
			}
			SetOptionUnsignedInt(section, optionName, defaultVal);
			ProfileReturnConst(defaultVal);
		}

		bool OptionHandler::GetOptionBool(const std::string& section, const std::string& optionName, bool defaultVal)
		{
			StartProfile;
			if (optionMap.size() > 0)
			{
				for (auto& mapsPair : optionMap)
				{
					if (mapsPair.first == section)
					{
						std::map<std::string, std::string> values = optionMap[mapsPair.first];
						auto fileLoaded = values.find(optionName);
						if (fileLoaded == values.end())
						{
							SetOptionBool(section, optionName, defaultVal);
							ProfileReturnConst(defaultVal);
						}
						else
						{
							if (values[optionName] == "true" || values[optionName] == "True" || values[optionName] == "TRUE")
							{
								ProfileReturnConst(true);
							}
							else if (values[optionName] == "false" || values[optionName] == "False" || values[optionName] == "FALSE")
							{
								ProfileReturnConst(false);
							}
							else if (std::stoi(values[optionName]) == 1)
							{
								ProfileReturnConst(true);
							}
							else if (std::stoi(values[optionName]) == 0)
							{
								ProfileReturnConst(false);
							}
							throw std::invalid_argument("Invalid arg expect bool. Got " + values[optionName]);
						}
					}
				}
			}
			SetOptionBool(section, optionName, defaultVal);
			ProfileReturnConst(defaultVal);
		}

		std::string OptionHandler::GetOptionString(const std::string& section, const std::string& optionName, std::string defaultVal)
		{
			StartProfile;
			if (optionMap.size() > 0)
			{
				for (auto& mapsPair : optionMap)
				{
					if (mapsPair.first == section)
					{
						std::map<std::string, std::string> values = optionMap[mapsPair.first];
						auto fileLoaded = values.find(optionName);
						if (fileLoaded == values.end())
						{
							SetOptionString(section, optionName, defaultVal);
							ProfileReturnConst(defaultVal);
						}
						else
						{
							ProfileReturn(values[optionName]);
						}
					}
				}
			}
			SetOptionString(section, optionName, defaultVal);
			ProfileReturnConst(defaultVal);
		}

		double OptionHandler::GetOptionDouble(const std::string & section, const std::string & optionName, double defaultVal)
		{
			StartProfile;
			if (optionMap.size() > 0)
			{
				for (auto& mapsPair : optionMap)
				{
					if (mapsPair.first == section)
					{
						std::map<std::string, std::string> values = optionMap[mapsPair.first];
						auto fileLoaded = values.find(optionName);
						if (fileLoaded == values.end())
						{
							SetOptionDouble(section, optionName, defaultVal);
							ProfileReturnConst(defaultVal);
						}
						else
						{
							ProfileReturn(std::stod(values[optionName]));
						}
					}
				}
			}
			SetOptionDouble(section, optionName, defaultVal);
			ProfileReturnConst(defaultVal);
		}


		void OptionHandler::SetOptionInt(const std::string& section, const std::string& optionName, int newValue)
		{
			StartProfile;
			auto fileLoaded = optionMap.find(section);
			if (fileLoaded == optionMap.end())
			{
				std::map<std::string, std::string> value;
				value[optionName] = std::to_string(newValue);
				optionMap[section] = value;
				ProfileReturnVoid;
			}
			else
			{
				std::map<std::string, std::string> &values = optionMap[section];
				values[optionName] = std::to_string(newValue);
				ProfileReturnVoid
			}
			ProfileReturnVoid;
		}

		void OptionHandler::SetOptionUnsignedInt(const std::string & section, const std::string & optionName, size_t newValue)
		{
			StartProfile;
			auto fileLoaded = optionMap.find(section);
			if (fileLoaded == optionMap.end())
			{
				std::map<std::string, std::string> value;
				value[optionName] = std::to_string(newValue);
				optionMap[section] = value;
				ProfileReturnVoid;
			}
			else
			{
				std::map<std::string, std::string> &values = optionMap[section];
				values[optionName] = std::to_string(newValue);
				ProfileReturnVoid
			}
			ProfileReturnVoid;
		}

		void OptionHandler::SetOptionBool(const std::string & section, const std::string & optionName, bool newValue)
		{
			StartProfile;
			auto fileLoaded = optionMap.find(section);
			if (fileLoaded == optionMap.end())
			{
				std::map<std::string, std::string> value;
				value[optionName] = newValue ? "true" : "false";
				optionMap[section] = value;
				ProfileReturnVoid;
			}
			else
			{
				std::map<std::string, std::string> &values = optionMap[section];
				values[optionName] = newValue ? "true" : "false";
				ProfileReturnVoid
			}
			ProfileReturnVoid;
		}

		void OptionHandler::SetOptionString(const std::string & section, const std::string & optionName, std::string newValue)
		{
			StartProfile;
			auto fileLoaded = optionMap.find(section);
			if (fileLoaded == optionMap.end())
			{
				std::map<std::string, std::string> value;
				value[optionName] = newValue;
				optionMap[section] = value;
				ProfileReturnVoid;
			}
			else
			{
				std::map<std::string, std::string> &values = optionMap[section];
				values[optionName] = newValue;
				ProfileReturnVoid
			}
			ProfileReturnVoid;
		}

		void OptionHandler::SetOptionDouble(const std::string & section, const std::string & optionName, double newValue)
		{
			StartProfile;
			auto fileLoaded = optionMap.find(section);
			if (fileLoaded == optionMap.end())
			{
				std::map<std::string, std::string> value;
				value[optionName] = std::to_string(newValue);
				optionMap[section] = value;
				ProfileReturnVoid;
			}
			else
			{
				std::map<std::string, std::string> &values = optionMap[section];
				values[optionName] = std::to_string(newValue);
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