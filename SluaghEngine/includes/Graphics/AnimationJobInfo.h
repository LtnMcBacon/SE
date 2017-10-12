#ifndef SE_GRAPHICS_ANIMATION_JOB_INFO_H_
#define SE_GRAPHICS_ANIMATION_JOB_INFO_H_
namespace SE
{
	namespace Graphics
	{
		struct AnimationJobInfo
		{
			int animationHandle;
			float speed;
			float timePos;
			bool animating;

			AnimationJobInfo() : speed(0.0f), timePos(0.0f), animating(false) {  };
			AnimationJobInfo(float sp) :  speed(sp), timePos(0.0f), animating(false) {};
		};
	}
}

#endif //SE_GRAPHICS_ANIMATION_JOB_INFO_H_