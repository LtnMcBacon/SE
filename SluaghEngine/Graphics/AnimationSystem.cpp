#include <Profiler.h>
#include "AnimationSystem.h"

using namespace DirectX;

SE::Graphics::AnimationSystem::AnimationSystem() {

	skeletons.reserve(3);
}

SE::Graphics::AnimationSystem::~AnimationSystem() {


}

int SE::Graphics::AnimationSystem::AddSkeleton(JointAttributes* jointData, size_t nrOfJoints, int *skeletonID) {

	// The number of joints must be larger 0
	if (nrOfJoints > 0){

		// Temporary skeleton
		Skeleton skeleton;

		// Load all the bindposes and their corresponding parent index
		// Parent index of the first root joint will never be used, since there is no parent
		for (UINT i = 0; i < nrOfJoints; i++) {

			Joint joint;

			joint.parentIndex = jointData[i].ParentIndex;
			joint.inverseBindPoseMatrix = XMLoadFloat4x4(&jointData[i].bindposeMatrix);

			skeleton.Hierarchy.push_back(joint);

		}

		// Push back the new skeleton
		skeletons.push_back(skeleton);

		*skeletonID = skeletons.size() - 1;

		return 0;

	}

	else {

		return -1;
	}
}

int SE::Graphics::AnimationSystem::AddAnimation(DirectX::XMFLOAT4X4* matrices, size_t nrOfKeyframes, size_t nrOfJoints, int *animationID) {

	// The number of joints must be larger 0
	if (nrOfJoints > 0) {

		Animation currentAnimation;
		currentAnimation.Length = nrOfKeyframes;

		for (UINT i = 0; i < nrOfJoints; i++) {

			// Every joint requires its own set of keyframes of the animation
			JointKeyFrame jointKeyFrame;

			for (UINT j = 0; j < nrOfKeyframes; j++) {

				Keyframe currentKeyFrame;

				// Get the keyframe matrix
				DirectX::XMMATRIX keyFrameMatrix = DirectX::XMLoadFloat4x4(&matrices[j + i*nrOfKeyframes]);
				currentKeyFrame.LocalTransform = keyFrameMatrix;

				// Set the time pose
				currentKeyFrame.TimePos = j;

				// Split up keyframe matrix to scale, quaternion and translation
				DirectX::XMVECTOR scale, rotQuat, trans;
				DirectX::XMMatrixDecompose(&scale, &rotQuat, &trans, keyFrameMatrix);

				DirectX::XMStoreFloat4(&currentKeyFrame.Scale, scale);
				DirectX::XMStoreFloat4(&currentKeyFrame.RotationQuat, rotQuat);
				DirectX::XMStoreFloat4(&currentKeyFrame.Translation, trans);

				// Push back the current keyframe
				jointKeyFrame.Keyframes.push_back(currentKeyFrame);
			}

			// Push back the animation at the current joint in the given skeleton
			currentAnimation.Joints.push_back(jointKeyFrame);
		}
		*animationID = animations.size();
		animations.push_back(currentAnimation);
		

		return 0;

	}

	else {

		return -1;
	}
}

void SE::Graphics::AnimationSystem::UpdateAnimation(int animIndex, int skeletonIndex, float timePos, DirectX::XMFLOAT4X4* at) {
	StartProfile;

	auto& skeleton = skeletons[skeletonIndex];
	auto& animation = animations[animIndex];

	// Open up a new XMFLOAT4x4 array to temporarily store the calculated joint transformations. Make on for the updated hierarchy as well
	std::vector<XMMATRIX> interpolatedJointTransforms;

	// Resize the vectors to match the joint hierarchy size
	interpolatedJointTransforms.resize(skeleton.Hierarchy.size());

	// Interpolate will sort out the interpolation for every joint's animation, thus returns a matrix for every iteration
	for (int i = 0; i < skeleton.Hierarchy.size(); i++) {

		CalculateJointMatrix(i, animation, timePos, interpolatedJointTransforms[i]); // check interpolations
	}
	
	//With all the calculated matrices at our disposal, let's update the transformations in the secondary joint array
	for (UINT i = 0; i < skeleton.Hierarchy.size(); i++) {

		// Create a reference to the currenct joint to be processed
		Joint &b = skeleton.Hierarchy[i];
		
		// Get the current joint GLOBAL transformation at the current animation time pose
		b.GlobalTx = interpolatedJointTransforms[i];

		XMFLOAT4X4 temp;
		XMStoreFloat4x4(&temp, XMMatrixTranspose(b.inverseBindPoseMatrix * b.GlobalTx));

		// Create the matrix by multiplying the joint global transformation with the inverse bind pose
		XMStoreFloat4x4(at + i, XMMatrixTranspose(b.inverseBindPoseMatrix * b.GlobalTx));

		int a = 0;
	}
	StopProfile;
}

void SE::Graphics::AnimationSystem::CalculateJointMatrix(int jointIndex,const Animation& animation, float animTimePos, DirectX::XMMATRIX& out) {

	StartProfile;
	// Animation has just started, so return the first keyframe
	int animationLength = animation.Length - 1;
	auto& joint = animation.Joints[jointIndex];
	if (animTimePos <= animation.Joints[jointIndex].Keyframes[0].TimePos) //first keyframe
	{
		ReturnFirstFrameMatrix(joint, out);		
	}

	// Animation has reached its end, so return the last keyframe
	else if (animTimePos >= animation.Joints[jointIndex].Keyframes[animationLength].TimePos) // last keyframe
	{
		ReturnLastFrameMatrix(joint, animation, out);
	}

	// Animation time is between two frames so they should be interpolated
	else
	{
		Interpolate(joint, animTimePos, out);
	}
	StopProfile;
}

void SE::Graphics::AnimationSystem::ReturnFirstFrameMatrix(const JointKeyFrame& joint, DirectX::XMMATRIX& out) {
	
	StartProfile;
	XMVECTOR S = XMLoadFloat4(&joint.Keyframes[0].Scale);
	XMVECTOR T = XMLoadFloat4(&joint.Keyframes[0].Translation);
	XMVECTOR Q = XMLoadFloat4(&joint.Keyframes[0].RotationQuat);

	XMVECTOR zero = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f); // Origo?

	out = XMMatrixAffineTransformation(S, zero, Q, T);
	StopProfile;
}

void SE::Graphics::AnimationSystem::ReturnLastFrameMatrix(const JointKeyFrame& joint, const Animation& animation, DirectX::XMMATRIX& out) {

	StartProfile;
	size_t animationLength = static_cast<size_t>(animation.Length - 1);

	XMVECTOR S = XMLoadFloat4(&joint.Keyframes[animationLength].Scale);
	XMVECTOR T = XMLoadFloat4(&joint.Keyframes[animationLength].Translation);
	XMVECTOR Q = XMLoadFloat4(&joint.Keyframes[animationLength].RotationQuat);

	XMVECTOR zero = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);

	out = XMMatrixAffineTransformation(S, zero, Q, T);
	StopProfile;
}

void SE::Graphics::AnimationSystem::Interpolate(const JointKeyFrame& joint, float animTimePos, DirectX::XMMATRIX& out)
{
	StartProfile;
	// I am using an int here to truncate the animation timepose to know which matrices I am interested about
	// Ex. if time is 1.2, the returning frame is 1.
	int currentFrameIndex = animTimePos;

	float kFirst = joint.Keyframes[currentFrameIndex].TimePos;
	float kLast = joint.Keyframes[currentFrameIndex + 1].TimePos;

	// Though the interpolation percent will be mainly responsible of returning a slightly changed matrix
	float interpolationPercent = (animTimePos - kFirst) / (kLast - kFirst);

	XMVECTOR kFirstScale = XMLoadFloat4(&joint.Keyframes[currentFrameIndex].Scale); // interpolating between the current keyframe and the comming keyframe.
	XMVECTOR kLastScale = XMLoadFloat4(&joint.Keyframes[currentFrameIndex + 1].Scale);

	XMVECTOR kFirstTranslation = XMLoadFloat4(&joint.Keyframes[currentFrameIndex].Translation);
	XMVECTOR kLastTranslation = XMLoadFloat4(&joint.Keyframes[currentFrameIndex + 1].Translation);

	XMVECTOR kFirstQuaternion = XMLoadFloat4(&joint.Keyframes[currentFrameIndex].RotationQuat);
	XMVECTOR kLastQuaternion = XMLoadFloat4(&joint.Keyframes[currentFrameIndex + 1].RotationQuat);

	XMVECTOR S = XMVectorLerp(kFirstScale, kLastScale, interpolationPercent);
	XMVECTOR T = XMVectorLerp(kFirstTranslation, kLastTranslation, interpolationPercent);
	XMVECTOR Q = XMQuaternionSlerp(kFirstQuaternion, kLastQuaternion, interpolationPercent);

	XMVECTOR zero = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);

	out = XMMatrixAffineTransformation(S, zero, Q, T);

	StopProfile;
}

