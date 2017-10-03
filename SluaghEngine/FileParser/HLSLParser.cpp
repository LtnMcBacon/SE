#include "HLSLParser.h"
#include <fstream>
#include <d3dcompiler.h>
#pragma comment(lib, "D3Dcompiler.lib")
#include <filesystem>
#include <windows.h>
#include <PathParsing.h>
namespace fs = std::experimental::filesystem;
SE::Parsers::HLSLParser::HLSLParser()
{
}


SE::Parsers::HLSLParser::~HLSLParser()
{
}

void OutputShaderErrorMessage(ID3DBlob* errorMessage,const wchar_t* shaderFilename)
{
	char* compileErrors;
	unsigned long bufferSize, i;


	// Get a pointer to the error message text buffer.
	compileErrors = (char*)(errorMessage->GetBufferPointer());

	// Get the length of the message.
	bufferSize = errorMessage->GetBufferSize();

	// Write out the error message.
	for (i = 0; i<bufferSize; i++)
	{
		printf("%s\n", compileErrors);
	}

	// Release the error message.
	errorMessage->Release();
	
	return;
}

HRESULT Parse_Spec(const wchar_t* filename, const char* entry, const char* targetV, ID3DBlob** blob, ID3DBlob** err)
{

	auto hr = D3DCompileFromFile(
		filename,
		nullptr,
		nullptr,
		entry,
		targetV,
		D3DCOMPILE_OPTIMIZATION_LEVEL3,
		0,
		blob,
		err
	);

	return hr;
}

void Write(const char* outfilename, ID3DBlob* data)
{
	std::ofstream outfile;
	std::string path = outfilename;
	auto idx = path.find_last_of("\\/");
	path = path.substr(0, idx);
	fs::create_directories(path);
	outfile.open(outfilename, std::ios::binary | std::ios::trunc | std::ios::out);
	if (!outfile.is_open())
	{
		printf("Wow, %s\n", outfilename);
		return;
	}
	//printf("Buffer size: %u\n", data->GetBufferSize());
	outfile.write((const char*)data->GetBufferPointer(), data->GetBufferSize());

	outfile.close();

	data->Release();
}

int SE::Parsers::HLSLParser::Parse(const wchar_t* filename, const char* outFilename)const
{

	ID3DBlob* psBlob;
	ID3DBlob* psBlobErr;
	HRESULT ps = Parse_Spec(filename, "PS_main", "ps_5_0", &psBlob, &psBlobErr);
	if (SUCCEEDED(ps))
	{
		Write(outFilename, psBlob);
		return 0;
	}


	ID3DBlob* gsBlob;
	ID3DBlob* gsBlobErr;
	HRESULT gs = Parse_Spec(filename, "GS_main", "gs_5_0", &gsBlob, &gsBlobErr);
	if (SUCCEEDED(gs))
	{
		Write(outFilename, gsBlob);
		return 0;
	}


	ID3DBlob* vsBlob;
	ID3DBlob* vsBlobErr;
	HRESULT vs = Parse_Spec(filename, "VS_main", "vs_5_0", &vsBlob, &vsBlobErr);
	if (SUCCEEDED(vs))
	{
		Write(outFilename, vsBlob);
		return 0;
	}


	printf("%s\n", psBlobErr->GetBufferPointer());
	printf("%s\n", gsBlobErr->GetBufferPointer());
	printf("%s\n", vsBlobErr->GetBufferPointer());
	
	return -1;
}
