#include "DataManager.h"
#include <Profiler.h>

//typedef std::variant<bool, int32_t, uint32_t, float, std::string, Core::Entity>  values;


std::ofstream& operator<<(std::ofstream& out, const std::pair<const SE::Utilz::GUID, SE::Core::IDataManager::values>& value)
{
	out.write((char*)&value.first, sizeof(value.first));
	uint32_t type = uint32_t(value.second.index());
	out.write((char*)&type, sizeof(type));

	switch (type)
	{
	case 0:
		out.write((char*)&std::get<bool>(value.second), sizeof(bool));
		break;
	case 1:
		out.write((char*)&std::get<int32_t>(value.second), sizeof(int32_t));
		break;
	case 2:
		out.write((char*)&std::get<uint32_t>(value.second), sizeof(uint32_t));
		break;
	case 3:
		out.write((char*)&std::get<float>(value.second), sizeof(float));
		break;
	case 4:
	{
		uint32_t len = std::get<std::string>(value.second).size();
		out.write((char*)&len, sizeof(len));
		out.write(std::get<std::string>(value.second).c_str(), len);
		break;
	}
	case 5:
		out.write((char*)&std::get<SE::Core::Entity>(value.second), sizeof(SE::Core::Entity));
		break;
	default:
		break;
	}

	return out;
}
std::ofstream& operator<<(std::ofstream& out, const std::unordered_map<SE::Utilz::GUID, SE::Core::IDataManager::values, SE::Utilz::GUID::Hasher>& values)
{
	uint32_t numValues = values.size();
	out.write((char*)&numValues, sizeof(numValues));
	for (auto& v : values)
		out << v;

	return out;
}


std::ifstream& operator>>(std::ifstream& in, std::pair<const SE::Utilz::GUID, SE::Core::IDataManager::values>& value)
{
	in.read((char*)&value.first, sizeof(value.first));
	uint32_t type = -1;
	in.read((char*)&type, sizeof(type));

	switch (type)
	{
	case 0:
	{
		bool v;		
		in.read((char*)&v, sizeof(bool));
		value.second = v;
		break;
	}

	case 1: 
	{
		int32_t v;
		in.read((char*)&v, sizeof(int32_t));
		value.second = v;
		break;
	}
	case 2:
	{
		uint32_t v;
		in.read((char*)&v, sizeof(uint32_t));
		value.second = v;
		break;
	}
	case 3:
	{
		float v;
		in.read((char*)&v, sizeof(float));
		value.second = v;
		break;
	}
	case 4:
	{
		uint32_t len = 0;
		in.read((char*)&len, sizeof(len));
		char* buff = new char[len];
		in.read(buff, len);
		value.second = std::string(buff, len);
		delete[] buff;
		break;
	}
	case 5:
	{
		SE::Core::Entity v;
		in.read((char*)&v, sizeof(SE::Core::Entity));
		value.second = v;
		break;
	}
	default:
		break;
	}

	return in;
}
std::ifstream& operator>>(std::ifstream& in, std::unordered_map<SE::Utilz::GUID, SE::Core::IDataManager::values, SE::Utilz::GUID::Hasher>& values)
{

	uint32_t numValues = 0;
	in.read((char*)&numValues, sizeof(numValues));
	for (uint32_t i = 0; i < numValues; i++)
	{
		std::pair<const SE::Utilz::GUID, SE::Core::IDataManager::values> value;
		in >> value;
		values.insert(value);
	}
	return in;
}
SE::Core::DataManager::DataManager(const IDataManager::InitializationInfo& info) : initInfo(info)
{
}


SE::Core::DataManager::~DataManager()
{

}

void SE::Core::DataManager::SetValue(const Entity entity, const Utilz::GUID key, const values value)
{
	StartProfile;
	auto const find = entityToIndex.find(entity);

	if (find == entityToIndex.end())
		CreateData(entity);
	
	entries[entityToIndex[entity]].values[key] = value;
	
	StopProfile;
}

SE::Core::DataManager::values SE::Core::DataManager::GetValue(const Entity entity, const Utilz::GUID key, const values default_value)
{
	StartProfile;
	const auto find = entityToIndex.find(entity);
	if (find == entityToIndex.end())
		ProfileReturnConst(default_value);
	auto const findV = entries[find->second].values.find(key);
	if (findV == entries[find->second].values.end())
		ProfileReturnConst(default_value);
	ProfileReturnConst(findV->second);
}

void SE::Core::DataManager::Copy(const Entity to, const Entity from)
{
	StartProfile;
	if (auto const find = entityToIndex.find(from); find == entityToIndex.end())
		ProfileReturnVoid;

	if(auto const find = entityToIndex.find(to); find == entityToIndex.end())
		CreateData(to);

	entries[entityToIndex[to]].values = entries[entityToIndex[from]].values;

	StopProfile;
}

std::optional<std::reference_wrapper<std::unordered_map<SE::Utilz::GUID, SE::Core::DataManager::values, SE::Utilz::GUID::Hasher>>> SE::Core::DataManager::GetAllValues(const Entity entity)
{

	StartProfile;

	if (auto const find = entityToIndex.find(entity); find == entityToIndex.end())
		ProfileReturnConst(std::nullopt);

	ProfileReturnConst(entries[entityToIndex[entity]].values);
}

bool SE::Core::DataManager::WriteToFile(const Entity entity, std::ofstream & file)
{
	_ASSERT_EXPR(file.is_open(), "File was not open, dataManagerWrite");
	StartProfile;
	if (auto const find = entityToIndex.find(entity); find == entityToIndex.end())
		ProfileReturnConst(false);
	file << entries[entityToIndex[entity]].values;
	ProfileReturnConst(true);
}

void SE::Core::DataManager::CreateFromFile(const Entity entity, std::ifstream & file)
{
	_ASSERT_EXPR(file.is_open(), "File was not open, dataManagerWrite");
	StartProfile;
	if (auto const find = entityToIndex.find(entity); find == entityToIndex.end())
		CreateData(entity);
	file >> entries[entityToIndex[entity]].values;
	ProfileReturnVoid;
}

void SE::Core::DataManager::Frame(Utilz::TimeCluster * timer)
{
	_ASSERT(timer);
	StartProfile;
	timer->Start("DataManager");
	GarbageCollection();

	timer->Stop("DataManager");
	StopProfile;
}

void SE::Core::DataManager::Destroy(size_t index)
{
	StartProfile;
	uint32_t last = entries.size() - 1;
	const Entity e = entries[index].entity;
	const Entity last_e = entries[last].entity;

	entries[index].entity = entries[last].entity;
	entries[index].values = entries[last].values;

	entityToIndex[last_e] = index;
	entityToIndex.erase(e);

	entries.pop_back();
	StopProfile;
}

void SE::Core::DataManager::Destroy(const Entity & entity)
{
}

void SE::Core::DataManager::GarbageCollection()
{
	StartProfile;
	uint32_t alive_in_row = 0;
	while (entries.size() > 0 && alive_in_row < 4U)
	{
		std::uniform_int_distribution<size_t> distribution(0U, entries.size() - 1U);
		size_t i = distribution(generator);
		if (initInfo.entityManager->Alive(entries[i].entity))
		{
			alive_in_row++;
			continue;
		}
		alive_in_row = 0;
		Destroy(i);
	}
	StopProfile;
}

void SE::Core::DataManager::CreateData(const Entity entity)
{
	StartProfile;
	size_t index = entityToIndex[entity] = entityToIndex.size();
	entries.push_back({ entity });
	ProfileReturnVoid;
}
