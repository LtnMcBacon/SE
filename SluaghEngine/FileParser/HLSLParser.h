#ifndef SE_PARSERS_HLSL_PARSER_H_
#define SE_PARSERS_HLSL_PARSER_H_

namespace SE
{
	namespace Parsers
	{
		class HLSLParser
		{
		public:
			HLSLParser();
			~HLSLParser();

			int Parse(const wchar_t* filename, const char* outFilename);
		};

	}
}

#endif // SE_PARSERS_HLSL_PARSER_H_