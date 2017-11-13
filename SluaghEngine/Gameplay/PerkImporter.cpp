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
		auto res = CoreInit::subSystems.resourceHandler->LoadResource(fileNames[i], [this, i](auto guid, auto data, auto size) {
			PerkData tempData;
			memcpy(&tempData.name, data, sizeof(string));

			perks.push_back(tempData);
			return ResourceHandler::InvokeReturn::SUCCESS | ResourceHandler::InvokeReturn::DEC_RAM;
		});
		if (res < 0)
		{
			throw exception("Failed to load perks");
		}

	}
	
	
}
