#include "ImageLoadTest.h"
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <Utilz/Timer.h>
#include <fstream>
using namespace SE;
using namespace Utilz;
SE::Test::ImageLoadTest::ImageLoadTest()
{

}

SE::Test::ImageLoadTest::~ImageLoadTest()
{
}

bool SE::Test::ImageLoadTest::Run(Utilz::IConsoleBackend* console)
{
	
	Timer timer;
	timer.Tick();
	int x = 0, y = 0, n = 0;
	stbi_uc* pixelData = stbi_load("Asset/purewhite.png", &x, &y, &n, 0);
	timer.Tick();
	console->Print("Loaded %d x %d resolution png in %f seconds.\n", x, y, timer.GetDelta<std::ratio<1,1>>());
	

	/*std::ofstream fout;
	fout.open("Asset/purewhite.rawimg",std::ios_base::out | std::ios_base::binary | std::ios_base::trunc);
	fout.write((char*)pixelData, x*y * n);
	fout.close();*/

	timer.Tick();
	std::ifstream fin;
	fin.open("Asset/purewhite.rawimg", std::ios_base::in | std::ios_base::binary);
	if (fin.is_open())
	{
		size_t fsize = fin.tellg();
		char* rawData = new char[fsize];
		fin.read(rawData, fsize);
		timer.Tick();
		console->Print("Loaded %d x %d in %f seconds.\n", x, y, timer.GetDelta<std::ratio<1, 1>>());
		stbi_image_free(pixelData);
		delete[] rawData;

		return true;
	}
	
	return false;
}
