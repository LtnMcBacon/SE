#ifndef SE_CORE_ANIMATION_FILE_HEADERS_H_
#define SE_CORE_ANIMATION_FILE_HEADERS_H_

#include <stdint.h>
namespace SE
{
	namespace Core
	{
		struct Skeleton_Header {

			uint32_t nrOfJoints;
		};

		struct JointAttributes {

			uint32_t ParentIndex;
			float bindposeMatrix[4][4];
		};

		struct Animation_Header {

			uint32_t nrOfJoints;
			uint32_t animationLength;
		};
	}
}

#endif