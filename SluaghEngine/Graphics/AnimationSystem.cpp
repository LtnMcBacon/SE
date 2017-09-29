
#include "AnimationSystem.h"

SE::Graphics::AnimationSystem::AnimationSystem() {

	skeletons.reserve(3);
}

SE::Graphics::AnimationSystem::~AnimationSystem() {


}

int SE::Graphics::AnimationSystem::AddSkeleton(SE::Core::JointAttributes* jointData, size_t nrOfJoints, int *skeletonID) {

	// The number of joints must be larger 0
	if (nrOfJoints > 0){

		// Temporary skeleton
		Skeleton skeleton;

		// Load all the bindposes and their corresponding parent index
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