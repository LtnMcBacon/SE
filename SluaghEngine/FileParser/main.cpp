#include <PathParsing.h>
#include "HLSLParser.h"
#include <fstream>
#include <filesystem>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include "TextureDesc.h"
#include <FBX_Converter\FBXConverter.h>
#include <functional>
#include <Utilz\GUID.h>

#ifdef _DEBUG
#pragma comment(lib, "FBX_ConverterD.lib")
#else
#pragma comment(lib, "FBX_Converter.lib")
#endif

namespace fs = std::experimental::filesystem;
using namespace SE;
using namespace std;

std::vector<SE::Utilz::GUID> RoomMaps; 

int ImageParse(const char* filename, const char* outputFilename);

struct Accepted
{
	std::string ext;
	std::string newExt;
	std::string map;
	std::function<void(const char* filename, const char* outFilename)> callback;
};
static std::string texMap = "";
std::vector<Accepted> acceptedExt = 
{
	{ "fbx", "nil", "../FBXTemp", [](const char* filename, const char* outFilename) {
	SE::FBX::FBXConverter File;
	if (!File.Load(filename, "FBXTemp", texMap)) {

		printf("Could not parse fbx file %s\n", filename);
		return;
	}

	File.Write();

	File.Deallocate();
} },

	{ "hlsl", "hlsl", "Shaders", [](const char* filename, const char* outFilename) {
	Parsers::HLSLParser hlslp;
	printf("Parsing file: %s...", filename);
	if (hlslp.Parse(Utilz::utf8ToUtf16(filename).c_str(), outFilename))
		printf("\n\tCould not parse: %s\n", filename);
	else
		printf(" Success\n");
} },

	{ "txt", "txt", "Text", [](const char* filename, const char* outFilename) {
	std::ifstream  src(filename, std::ios::binary);

	std::ofstream  dst(outFilename, std::ios::binary | std::ios::trunc);

	dst << src.rdbuf();
	src.close();
	dst.close(); } },
	{ "spritefont", "spritefont", "Font", [](const char* filename, const char* outFilename) {
		fs::copy_file(filename, outFilename, fs::v1::copy_options::overwrite_existing);
	} },
	{ "gui", "sei", "GUI", [](const char* filename, const char* outFilename) {
		printf("Parsing file: %s...\n", filename);

		if (ImageParse(filename, outFilename))
			printf("Could not parse: %s\n", filename);
		} },
		{"wav", "wav", "Sounds", [](const char* filename, const char* outFilename) {
			fs::copy_file(filename, outFilename, fs::v1::copy_options::overwrite_existing); }},

			{"SEFBT", "SEFBT", "BehaviouralTrees", [](const char* filename, const char* outFilename) {
				fs::copy_file(filename, outFilename, fs::v1::copy_options::overwrite_existing); } },

				{ "SEC", "SEC", "Enemies", [](const char* filename, const char* outFilename) {
					fs::copy_file(filename, outFilename, fs::v1::copy_options::overwrite_existing); }},
					{ "raw", "raw", "RoomMaps", [](const char* filename, const char* outFilename)
					{
						RoomMaps.push_back(Utilz::getFilename(filename));
						fs::copy_file(filename, outFilename, fs::v1::copy_options::overwrite_existing);
					}},

					{ "SEP", "SEP", "Projectiles", [](const char* filename, const char* outFilename) {
					fs::copy_file(filename, outFilename, fs::v1::copy_options::overwrite_existing); } },

	
	{ "si", "si", "Gameplay", [](const char* filename, const char* outFilename) {
		fs::copy_file(filename, outFilename, fs::v1::copy_options::overwrite_existing); } },
	{ "sa", "sa", "Gameplay", [](const char* filename, const char* outFilename) {
		fs::copy_file(filename, outFilename, fs::v1::copy_options::overwrite_existing); } }
};

std::vector<Accepted> fbxAccepted =
{
	{ "jpg", "sei", "Textures", [](const char* filename, const char* outFilename) {
	if (ImageParse(filename, outFilename))
		printf("Could not parse: %s\n", filename); } },
	{ "png", "sei", "Textures", [](const char* filename, const char* outFilename) {
		if (ImageParse(filename, outFilename))
			printf("Could not parse: %s\n", filename); } },
	{ "tga", "sei", "Textures", [](const char* filename, const char* outFilename) {
			if (ImageParse(filename, outFilename))
				printf("Could not parse: %s\n", filename); } },


	{ "mesh", "mesh", "Meshes", [](const char* filename, const char* outFilename) {
				fs::copy_file(filename, outFilename, fs::v1::copy_options::overwrite_existing); } },
	{ "anim", "anim", "Animations", [](const char* filename, const char* outFilename) {
					fs::copy_file(filename, outFilename, fs::v1::copy_options::overwrite_existing); } },
	{ "light", "light", "Lights", [](const char* filename, const char* outFilename) {
						fs::copy_file(filename, outFilename, fs::v1::copy_options::overwrite_existing); } },
	{ "mat", "mat", "Materials", [](const char* filename, const char* outFilename) {
					fs::copy_file(filename, outFilename, fs::v1::copy_options::overwrite_existing); } },					
	{ "skel", "skel", "Skeletons", [](const char* filename, const char* outFilename) {
							fs::copy_file(filename, outFilename, fs::v1::copy_options::overwrite_existing); } },
};



Accepted* IsAccepted(std::vector<Accepted>& acc, const Utilz::File& file)
{
	for (auto& a : acc)
	{
		if (Utilz::getExtension(file.name) == a.ext)
		{
			return &a;
		}
	}

	return nullptr;
}

int main(int argc, char* argv[])
{

	if (argc < 3)
	{
		printf("Usage: %s <source directory(, ...)> [target directory]\n", argv[0]);
	}
	else
	{
		vector<std::string> acceptedFiles;
		fs::remove_all(argv[argc-1]);
		fs::remove_all("FBXTemp");
		for (int i = 1; i < argc - 1; i++)
		{
			texMap = argv[i];


		
			vector<Utilz::File> files;
		

			Utilz::get_all_files_names_within_folder(argv[i], files);
			for (auto& f : files)
			{
				auto a = IsAccepted(acceptedExt, f);
				if (a != nullptr)
				{
					std::string path = std::string(argv[argc - 1]) + "/" + a->map + "/" + Utilz::removeExtension(f.name) + "." + a->newExt;
					if (a->newExt != "nil")
						acceptedFiles.push_back(path);
					auto idx = path.find_last_of("\\/");
					auto path2 = path.substr(0, idx);
					fs::create_directories(path2);

					a->callback(f.fullPath.c_str(), path.c_str());
				}
			}

			vector<Utilz::File> fbxConvFiles;
			Utilz::get_all_files_names_within_folder("FBXTemp", fbxConvFiles);

			for (auto& f : fbxConvFiles)
			{
				auto a = IsAccepted(fbxAccepted, f);
				if (a != nullptr)
				{
					std::string path = std::string(argv[argc - 1]) + "/" + a->map + "/" + Utilz::removeExtension(f.name) + "." + a->newExt;
					if (a->newExt != "nil")
						acceptedFiles.push_back(path);
					auto idx = path.find_last_of("\\/");
					auto path2 = path.substr(0, idx);
					fs::create_directories(path2);

					a->callback(f.fullPath.c_str(), path.c_str());
				}
			}
		}
		std::ofstream gE;
		gE.open("rawLoaderEntries.txt", std::ios::trunc);
		if (gE.is_open())
		{
			for (auto& f : acceptedFiles)
				gE << f << std::endl;
			gE << std::string(argv[argc - 1]) + "/RoomGeneration.txt" << std::endl;
		}

		std::ofstream RM; 
		RM.open(std::string(argv[argc - 1]) + "/RoomGeneration.txt", std::ios::trunc|std::ios::binary);
		if (RM.is_open())
		{
			uint32_t nrOfRooms = RoomMaps.size(); 
			RM.write((char*)&nrOfRooms, sizeof(uint32_t)); 
			RM.write((char*)RoomMaps.data(), sizeof(SE::Utilz::GUID) * nrOfRooms); 
		}
		//fs::remove_all("FBXTemp");
	}
	
	return 0;
}


int ImageParse(const char* filename, const char* outputFilename)
{
	Graphics::TextureDesc texDesc;
	int x, y, n;
	stbi_uc* pixelData = stbi_load(filename, &x, &y, &n, 4);
	texDesc.width = x;
	texDesc.height = y;

	std::ofstream outfile;
	std::string path = outputFilename;
	auto idx = path.find_last_of("\\/");
	path = path.substr(0, idx);
	fs::create_directories(path);
	outfile.open(outputFilename, std::ios::binary | std::ios::trunc | std::ios::out);
	if (!outfile.is_open())
	{
		stbi_image_free(pixelData);
		printf("Wow, %s\n", outputFilename);
		return -1;
	}

	outfile.write((char*)&texDesc, sizeof(texDesc));
	outfile.write((char*)pixelData, x * y * 4);

	outfile.close();

	stbi_image_free(pixelData);

	return 0;
}