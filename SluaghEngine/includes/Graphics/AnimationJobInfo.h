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
			float timePos;
			bool animating;

			AnimationJobInfo() : skeletonID(-1), animationID(-1), speed(0.0f), timePos(0.0f), animating(false) {  };
			AnimationJobInfo(int s, int a, float sp) : skeletonID(s), animationID(a), speed(sp), timePos(0.0f), animating(false) {};
		};
	}
}

#endif //SE_GRAPHICS_ANIMATION_JOB_INFO_H_