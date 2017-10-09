
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

int SE::Graphics::AnimationSystem::AddAnimation(DirectX::XMFLOAT4X4* matrices, size_t nrOfKeyframes, size_t nrOfJoints, size_t skeletonIndex, int *animationID) {

	// The number of joints must be larger 0
	if (nrOfJoints > 0) {

		for (UINT i = 0; i < nrOfJoints; i++) {

			// Every joint requires its own set of keyframes of the animation
			Animation currentAnimation;
			currentAnimation.Length = nrOfKeyframes;

			for (UINT j = 0; j < nrOfKeyframes; j++) {

				Keyframe currentKeyFrame;

				// Get the keyframe matrix
				DirectX::XMMATRIX keyFrameMatrix = DirectX::XMLoadFloat4x4(&matrices[j]);
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
				currentAnimation.Keyframes.push_back(currentKeyFrame);
			}

			// Push back the animation at the current joint in the given skeleton
			skeletons[skeletonIndex].Hierarchy[i].Animations.push_back(currentAnimation);
		}

		*animationID = skeletons[skeletonIndex].Hierarchy[0].Animations.size() - 1;

		return 0;

	}

	else {

		return -1;
	}
}

void SE::Graphics::AnimationSystem::UpdateAnimation(int animIndex, int skeletonIndex, float timePos) {

	// Open up a new XMFLOAT4x4 array to temporarily store the calculated joint transformations. Make on for the updated hierarchy as well
	std::vector<DirectX::XMFLOAT4X4> interpolatedJointTransforms;

	// Reserve the vectors to match the joint hierarchy size
	interpolatedJointTransforms.reserve(skeletons[skeletonIndex].Hierarchy.size());

	// Interpolate will sort out the interpolation for every joint's animation, thus returns a matrix for every iteration
	for (int i = 0; i < skeletons[skeletonIndex].Hierarchy.size(); i++) {

		interpolatedJointTransforms.push_back(CalculateJointMatrix(i, animIndex, skeletons[skeletonIndex], timePos)); // check interpolations
	}

	Joint &root = skeletons[skeletonIndex].Hierarchy[0];
	root.LocalTx = XMLoadFloat4x4(&interpolatedJointTransforms[0]);
	root.GlobalTx = root.LocalTx;
	XMStoreFloat4x4(&skeletons[skeletonIndex].jointArray[0], XMMatrixTranspose(root.inverseBindPoseMatrix * root.GlobalTx));

	//With all the calculated matrices at our disposal, let's update the transformations in the secondary joint array
	for (UINT i = 1; i < skeletons[skeletonIndex].Hierarchy.size(); i++) {

		// Create a reference to the currenct joint to be processed
		Joint &b = skeletons[skeletonIndex].Hierarchy[i];

		b.LocalTx = XMLoadFloat4x4(&interpolatedJointTransforms[i]);

		// Get the current joint LOCAL transformation at the current animation time pose
		b.GlobalTx = b.LocalTx * skeletons[skeletonIndex].Hierarchy[b.parentIndex].GlobalTx;

		// Create the matrix by applying the inverse bind pose matrix on the global transformation
		XMStoreFloat4x4(&skeletons[skeletonIndex].jointArray[i], XMMatrixTranspose(b.inverseBindPoseMatrix * b.GlobalTx));

	}
}

XMFLOAT4X4 SE::Graphics::AnimationSystem::CalculateJointMatrix(int jointIndex, int animIndex, Skeleton &skeleton, float animTimePos) {

	// Animation has just started, so return the first keyframe
	int animationLength = skeleton.Hierarchy[jointIndex].Animations[animIndex].Length - 1;

	if (animTimePos <= skeleton.Hierarchy[jointIndex].Animations[animIndex].Keyframes[0].TimePos) //first keyframe
	{

		return ReturnFirstFrameMatrix(jointIndex, animIndex, skeleton);

	}

	// Animation has reached its end, so return the last keyframe
	else if (animTimePos >= skeleton.Hierarchy[jointIndex].Animations[animIndex].Keyframes[animationLength].TimePos) // last keyframe
	{

		return ReturnLastFrameMatrix(jointIndex, animIndex, skeleton);

	}

	// Animation time is between two frames so they should be interpolated
	else
	{

		return Interpolate(jointIndex, animIndex, skeleton, animTimePos);

	}
}

XMFLOAT4X4 SE::Graphics::AnimationSystem::ReturnFirstFrameMatrix(int jointIndex, int animIndex, Skeleton &skeleton) {

	XMFLOAT4X4 M;

	XMVECTOR S = XMLoadFloat4(&skeleton.Hierarchy[jointIndex].Animations[animIndex].Keyframes[0].Scale);
	XMVECTOR T = XMLoadFloat4(&skeleton.Hierarchy[jointIndex].Animations[animIndex].Keyframes[0].Translation);
	XMVECTOR Q = XMLoadFloat4(&skeleton.Hierarchy[jointIndex].Animations[animIndex].Keyframes[0].RotationQuat);

	XMVECTOR zero = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);

	XMStoreFloat4x4(&M, XMMatrixAffineTransformation(S, zero, Q, T));

	return M;
}

XMFLOAT4X4 SE::Graphics::AnimationSystem::ReturnLastFrameMatrix(int jointIndex, int animIndex, Skeleton &skeleton) {

	int animationLength = skeleton.Hierarchy[jointIndex].Animations[animIndex].Length - 1;

	XMFLOAT4X4 M;

	XMVECTOR S = XMLoadFloat4(&skeleton.Hierarchy[jointIndex].Animations[animIndex].Keyframes[animationLength].Scale);
	XMVECTOR T = XMLoadFloat4(&skeleton.Hierarchy[jointIndex].Animations[animIndex].Keyframes[animationLength].Translation);
	XMVECTOR Q = XMLoadFloat4(&skeleton.Hierarchy[jointIndex].Animations[animIndex].Keyframes[animationLength].RotationQuat);

	XMVECTOR zero = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);

	XMStoreFloat4x4(&M, XMMatrixAffineTransformation(S, zero, Q, T));

	return M;
}

XMFLOAT4X4 SE::Graphics::AnimationSystem::Interpolate(int jointIndex, int animIndex, Skeleton & skeleton, float animTimePos)
{
	XMFLOAT4X4 M;
	// I am using an int here to truncate the animation timepose to know which matrices I am interested about
	// Ex. if time is 1.2, the returning frame is 1.
	int currentFrameIndex = animTimePos;

	float kFirst = skeleton.Hierarchy[jointIndex].Animations[animIndex].Keyframes[currentFrameIndex].TimePos;
	float kLast = skeleton.Hierarchy[jointIndex].Animations[animIndex].Keyframes[currentFrameIndex + 1].TimePos;

	// Though the interpolation percent will be mainly responsible of returning a slightly changed matrix
	float interpolationPercent = (animTimePos - kFirst) / (kLast - kFirst);

	XMVECTOR kFirstScale = XMLoadFloat4(&skeleton.Hierarchy[jointIndex].Animations[animIndex].Keyframes[currentFrameIndex].Scale); // interpolating between the current keyframe and the comming keyframe.
	XMVECTOR kLastScale = XMLoadFloat4(&skeleton.Hierarchy[jointIndex].Animations[animIndex].Keyframes[currentFrameIndex + 1].Scale);

	XMVECTOR kFirstTranslation = XMLoadFloat4(&skeleton.Hierarchy[jointIndex].Animations[animIndex].Keyframes[currentFrameIndex].Translation);
	XMVECTOR kLastTranslation = XMLoadFloat4(&skeleton.Hierarchy[jointIndex].Animations[animIndex].Keyframes[currentFrameIndex + 1].Translation);

	XMVECTOR kFirstQuaternion = XMLoadFloat4(&skeleton.Hierarchy[jointIndex].Animations[animIndex].Keyframes[currentFrameIndex].RotationQuat);
	XMVECTOR kLastQuaternion = XMLoadFloat4(&skeleton.Hierarchy[jointIndex].Animations[animIndex].Keyframes[currentFrameIndex + 1].RotationQuat);

	XMVECTOR S = XMVectorLerp(kFirstScale, kLastScale, interpolationPercent);
	XMVECTOR T = XMVectorLerp(kFirstTranslation, kLastTranslation, interpolationPercent);
	XMVECTOR Q = XMQuaternionSlerp(kFirstQuaternion, kLastQuaternion, interpolationPercent);

	XMVECTOR zero = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);

	XMStoreFloat4x4(&M, XMMatrixAffineTransformation(S, zero, Q, T));

	return M;
}

