#include "PerkImporter.h"
#include <fstream>
#include <Core\IEngine.h>
#include "CoreInit.h"
using namespace std;
SE::Gameplay::PerkImporter::PerkImporter()
{
//	resourceHandler->Initialize();
	

}

SE::Gameplay::PerkImporter::~PerkImporter()
{
}

void SE::Gameplay::PerkImporter::loadPerkData(string fileNames[], int perkCount)
{


	for (size_t i = 0; i < perkCount; i++)
	{
		
	//	file[fileNames[i].length()] = 0;
		
		string fileString = fileNames[i];
		
		
		auto res = CoreInit::subSystems.resourceHandler->LoadResource((Utilz::GUID)fileString, [this](auto guid, auto data, auto size) {
			PerkData tempData;
			size_t head = 0;
			int nameSize;
			
			memcpy(&nameSize, data, sizeof(int));
			head += sizeof(int);
			char* tempName = new char[nameSize + 1];
			tempName[nameSize] = 0;
			
			memcpy(tempName, (char*)data + head, sizeof(char) * nameSize);
			tempData.name = tempName;
			head += sizeof(char) * nameSize;

			memcpy(&tempData.checkSize, (char*)data + head, sizeof(int));
			head += sizeof(int);

			memcpy(&tempData.typeSize, (char*)data + head, sizeof(int));
			head += sizeof(int);

			for (size_t i = 0; i < tempData.typeSize; i++)
			{
				int type = -1;
				int value = -1;
				memcpy(&type, (char*)data + head, sizeof(int));
				head += sizeof(int);
				memcpy(&value, (char*)data + head, sizeof(int));
				head += sizeof(int);

				tempData.types.push_back(type);
				tempData.values.push_back(value);
			}
			for (size_t i = 0; i < tempData.checkSize; i++)
			{
				int check = -1;
				memcpy(&check, (char*)data + head, sizeof(int));
				head += sizeof(int);
				tempData.checks.push_back(check);
			}

			perks.push_back(tempData);
			delete[] tempName;
			return ResourceHandler::InvokeReturn::SUCCESS | ResourceHandler::InvokeReturn::DEC_RAM;
		});
		if (res < 0)
		{
			//throw exception("Failed to load perks");
		}
		
		
	}
	
	
}
