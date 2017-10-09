#ifndef SE_GRAPHICS_ANIMATION_JOB_INFO_H_
#define SE_GRAPHICS_ANIMATION_JOB_INFO_H_
namespace SE
{
	namespace Graphics
	{
		struct AnimationJobInfo
		{
			int skeletonID;
			int animationID;
			float speed;

			AnimationJobInfo() : skeletonID(-1), animationID(-1), speed(0.0f) {  };
			AnimationJobInfo(int s, int a, float sp) : skeletonID(s), animationID(a), speed(sp) {};
		};
	}
}

#endif //SE_GRAPHICS_ANIMATION_JOB_INFO_H_