#ifndef _SE_GAMEPLAY_PERKIMPORTER_H_
#define _SE_GAMEPLAY_PERKIMPORTER_H_
#include "PerkData.h"
#include <ResourceHandler\IResourceHandler.h>
namespace SE
{
	namespace Gameplay
	{
		class PerkImporter
		{
		public:
			PerkImporter();
			~PerkImporter();

			/**
			* @brief Load all the perks
			* @detailed Loads all the perks from the Perk map and fills a PerkData struct which is added to a perks vector
			* @param[in] filePaths[] is an array with all the file names of the perks
			* @param[in] perkCount the amount of perks to be loaded
			*/
			void loadPerkData(std::string fileNames[], int perkCount);

			std::vector<PerkData> perks;

		private:

		};
	}

}


#endif