#include <Profiler.h>
#include "AnimationSystem.h"

using namespace DirectX;
SE::Core::AnimationSystem::JointMatrices  SE::Core::AnimationSystem::mats;

SE::Core::AnimationSystem::AnimationSystem(Graphics::IRenderer* renderer) : RenderableManagerInstancing(renderer)
{
	DirectX::XMFLOAT4X4 id;
	DirectX::XMStoreFloat4x4(&id, DirectX::XMMatrixIdentity());
	for (auto i = 0; i < 30; i++)
		mats.jointMatrix[i] = id;

}

SE::Core::AnimationSystem::~AnimationSystem() {

}

int SE::Core::AnimationSystem::AddSkeleton(const Utilz::GUID& guid, JointAttributes* jointData, size_t nrOfJoints) {


	// The number of joints must be larger 0
	
	if (nrOfJoints > 0){
		auto& skeleton = skeletons[guid];
		// Load all the bindposes and their corresponding parent index
		// Parent index of the first root joint will never be used, since there is no parent
		for (size_t i = 0; i < nrOfJoints; i++) {

			Joint joint;

			joint.parentIndex = jointData[i].ParentIndex;
			joint.inverseBindPoseMatrix = XMLoadFloat4x4((XMFLOAT4X4*)&jointData[i].bindposeMatrix);

			DirectX::XMVECTOR scale, rotQuat, trans;
			DirectX::XMFLOAT4 S, Q, T;
			DirectX::XMMatrixDecompose(&scale, &rotQuat, &trans, joint.inverseBindPoseMatrix);

			DirectX::XMStoreFloat4(&S, scale);
			DirectX::XMStoreFloat4(&Q, rotQuat);
			DirectX::XMStoreFloat4(&T, trans);

			Q.x *= -1.0f;
			Q.y *= -1.0f;
			T.z *= -1.0f;

			scale = DirectX::XMLoadFloat4(&S);
			rotQuat = DirectX::XMLoadFloat4(&Q);
			trans = DirectX::XMLoadFloat4(&T);

			XMVECTOR zero = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
			joint.inverseBindPoseMatrix = XMMatrixAffineTransformation(scale, zero, rotQuat, trans);

			joint.jointName = jointData[i].jointName;

			skeleton.Hierarchy.push_back(joint);

		}

		return 0;

	}

	return -1;

}

bool SE::Core::AnimationSystem::IsSkeletonLoaded(const Utilz::GUID & guid) const
{
	auto const findS = skeletons.find(guid);
	return findS != skeletons.end();
}

int SE::Core::AnimationSystem::AddAnimation(const Utilz::GUID& guid, DirectX::XMFLOAT4X4* matrices, uint32_t* joints, size_t nrOfKeyframes, size_t nrOfJoints) {


	// The number of joints must be larger 0
	if (nrOfJoints > 0) {
		auto& currentAnimation = animations[guid];
		currentAnimation.Length = nrOfKeyframes;

		currentAnimation.jointToActualJoint.resize(nrOfJoints);

		memcpy(currentAnimation.jointToActualJoint.data(), joints, sizeof(uint32_t) * nrOfJoints);

		for (size_t i = 0; i < nrOfJoints; i++) {

			// Every joint requires its own set of keyframes of the animation
			JointKeyFrame jointKeyFrame;

			for (size_t j = 0; j < nrOfKeyframes; j++) {

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
				currentKeyFrame.RotationQuat.x *= -1.0f;
				currentKeyFrame.RotationQuat.y *= -1.0f;

				DirectX::XMStoreFloat4(&currentKeyFrame.Translation, trans);
				currentKeyFrame.Translation.z *= -1.0f;

				// Push back the current keyframe
				jointKeyFrame.Keyframes.push_back(currentKeyFrame);
			}

			// Push back the animation at the corresponding joint index in the given skeleton
			currentAnimation.Joints.push_back( jointKeyFrame);
			
		}

		return 0;
	}
	return -1;

}

bool SE::Core::AnimationSystem::IsAnimationLoaded(const Utilz::GUID & guid) const
{
	auto const findA = animations.find(guid);
	return findA != animations.end();
}

void SE::Core::AnimationSystem::CalculateMatrices(const Entity& entity, AnimationInfo& info, bool threaded)
{
	// Get the animation bucket
	const auto bucketAndID = entityToBucketAndIndexInBucket.find(entity);
	if (bucketAndID == entityToBucketAndIndexInBucket.end())
		return;
	auto bucket = (AnimationBucket*)pipelineToRenderBucket[bucketAndID->second.bucket];
	// Get the skeleton
	auto& skeleton = skeletons[info.skeleton];

	// Create vector of identity matrices with the same size as the skeleton
	DirectX::XMFLOAT4X4* bucketTransform;
	DirectX::XMFLOAT4X4* bucketTransformUnThreaded = nullptr;
	if (!threaded)
	{
		bucketTransformUnThreaded = bucket->matrices[mapingIndex][bucketAndID->second.index].jointMatrix;
		memcpy(bucketTransformUnThreaded, &mats, sizeof(XMFLOAT4X4));
	}	
	bucketTransform = bucket->matrices[updateIndex][bucketAndID->second.index].jointMatrix;
	memcpy(bucketTransform, &mats, sizeof(XMFLOAT4X4) * 30);

	// Create vector of bools to check blending status at each joint
	std::vector<bool> blendCheck;
	blendCheck.resize(skeleton.Hierarchy.size());

	// Fill bool vector with false
	memset(&blendCheck[0], false, sizeof(bool) * blendCheck.size());

	// Loop through each animation
	for (size_t layerIndex = 0; layerIndex < info.nrOfLayers; layerIndex++) {

		auto& animation = animations[info.animation[layerIndex]];

		// Loop through each joint in the animation
		for (size_t jointIndex = 0; jointIndex < animation.Joints.size(); jointIndex++) {
			
			// Get the actual index to the animated joint
			uint32_t actualIndex = animation.jointToActualJoint[jointIndex];

			// The actual index must be lower than the hierarchy size
			if (actualIndex < skeleton.Hierarchy.size()) {

				// Calculate the transformation
				XMMATRIX tempMatrix;
				CalculateJointMatrix(jointIndex, animation, info.timePos[layerIndex], tempMatrix);

				if (blendCheck[actualIndex] == true) {

						CalculateBlendMatrices(XMLoadFloat4x4(&bucketTransform[actualIndex]), tempMatrix, info.blendFactor[layerIndex], bucketTransform[actualIndex]);
						if (!threaded)
							CalculateBlendMatrices(XMLoadFloat4x4(&bucketTransformUnThreaded[actualIndex]), tempMatrix, info.blendFactor[layerIndex], bucketTransformUnThreaded[actualIndex]);
				}

				else {

					XMStoreFloat4x4(&bucketTransform[actualIndex], tempMatrix);
					if (!threaded)
						XMStoreFloat4x4(&bucketTransformUnThreaded[actualIndex], tempMatrix);
				}

				blendCheck[actualIndex] = true;
			}
		}

		if (info.looping[layerIndex] == true) {

			if (info.timePos[layerIndex] > (float)animation.Length)
				info.timePos[layerIndex] = 0.0f;

		}
	}

	for (size_t i = 0; i < skeleton.Hierarchy.size(); i++) {

		// Create a reference to the currenct joint to be processed
		const Joint &b = skeleton.Hierarchy[i];

		// Create the matrix by multiplying the joint global transformation with the inverse bind pose
		
		if(blendCheck[i] == true){
		XMStoreFloat4x4(&bucketTransform[i], XMMatrixTranspose(b.inverseBindPoseMatrix * XMLoadFloat4x4(&bucketTransform[i])));
		}
		else {
		XMStoreFloat4x4(&bucketTransform[i], XMMatrixIdentity());
		}
		if (!threaded)
		{
			if (blendCheck[i] == true) {
				XMStoreFloat4x4(&bucketTransformUnThreaded[i], XMMatrixTranspose(b.inverseBindPoseMatrix * XMLoadFloat4x4(&bucketTransformUnThreaded[i])));
			}
			else {
				XMStoreFloat4x4(&bucketTransformUnThreaded[i], XMMatrixIdentity());
			}
		}
	}
}

void SE::Core::AnimationSystem::CalculateBlendMatrices(const XMMATRIX& matrix1, const XMMATRIX& matrix2, float blendFactor, XMFLOAT4X4& out) {

	XMVECTOR mesh1_quaternion, mesh1_translation, mesh1_scale;
	XMVECTOR mesh2_quaternion, mesh2_translation, mesh2_scale;

	XMMatrixDecompose(&mesh1_scale, &mesh1_quaternion, &mesh1_translation, matrix1);
	XMMatrixDecompose(&mesh2_scale, &mesh2_quaternion, &mesh2_translation, matrix2);

	XMVECTOR kFirstScale = mesh1_scale;
	XMVECTOR kLastScale = mesh2_scale;

	XMVECTOR kFirstTranslation = mesh1_translation;
	XMVECTOR kLastTranslation = mesh2_translation;

	XMVECTOR kFirstQuaternion = mesh1_quaternion;
	XMVECTOR kLastQuaternion = mesh2_quaternion;

	XMVECTOR S = XMVectorLerp(kFirstScale, kLastScale, blendFactor);
	XMVECTOR T = XMVectorLerp(kFirstTranslation, kLastTranslation, blendFactor);
	XMVECTOR Q = XMQuaternionSlerp(kFirstQuaternion, kLastQuaternion, blendFactor);

	XMVECTOR zero = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);

	XMStoreFloat4x4(&out, XMMatrixAffineTransformation(S, zero, Q, T));

}

int SE::Core::AnimationSystem::FindJointIndex(Utilz::GUID skeleton, Utilz::GUID jointNameToFind) {

	// Get the skeleton
	auto& skel = skeletons[skeleton];

	// Loop through the hierarchy
	for (size_t jointIndex = 0; jointIndex < skel.Hierarchy.size(); jointIndex++) {

		if (skel.Hierarchy[jointIndex].jointName == jointNameToFind) {

			return jointIndex;
		}
	}
	
	return -1;
}

void SE::Core::AnimationSystem::GetJointMatrix(const Entity& entity, int jointIndex, DirectX::XMFLOAT4X4& matrix) {

	auto find = entityToBucketAndIndexInBucket.find(entity);

	if (find != entityToBucketAndIndexInBucket.end()) {

		// Get all the entity matrices
		auto& matrices = ((AnimationBucket*)pipelineToRenderBucket[find->second.bucket])->matrices[mapingIndex][find->second.index];

		// Get the transposed matrix
		DirectX::XMFLOAT4X4 transposed = matrices.jointMatrix[jointIndex];

		// Transpose it back so we can work with it
		XMStoreFloat4x4(&matrix, XMMatrixTranspose(XMLoadFloat4x4(&transposed)));
	
	}
}

unsigned int SE::Core::AnimationSystem::GetAnimationLength(const Utilz::GUID& guid) {

	auto find = animations.find(guid);

	if (find != animations.end()) {

		return animations[find->first].Length;
	}

	return 0;
}

void SE::Core::AnimationSystem::GetJointInverseBindPose(const Utilz::GUID& guid, int jointIndex, DirectX::XMMATRIX& matrix) {

	auto find = skeletons.find(guid);

	if (find != skeletons.end()) {

		matrix = skeletons[find->first].Hierarchy[jointIndex].inverseBindPoseMatrix;
	}

}

void* SE::Core::AnimationSystem::GetBucketBonePtr(const Entity& entity)
{
	const auto find = entityToBucketAndIndexInBucket.find(entity);
	if(find != entityToBucketAndIndexInBucket.end())
	{
		return ((AnimationBucket*)pipelineToRenderBucket[find->second.bucket])->matrices[updateIndex].data();
	}
	return nullptr;
}

void* SE::Core::AnimationSystem::GetBucketTransformPtr(const Entity& entity)
{
	const auto find = entityToBucketAndIndexInBucket.find(entity);
	if (find != entityToBucketAndIndexInBucket.end())
	{
		return ((AnimationBucket*)pipelineToRenderBucket[find->second.bucket])->transforms.data();
	}
	return nullptr;
}

void SE::Core::AnimationSystem::UpdateAnimation(const Animation& animation, const Skeleton& skeleton, float timePos, DirectX::XMFLOAT4X4* at) {
	StartProfile;
	
	// Open up a new XMFLOAT4x4 array to temporarily store the calculated joint transformations. Make on for the updated hierarchy as well
	std::vector<XMMATRIX> interpolatedJointTransforms;

	// Resize the vectors to match the joint hierarchy size
	interpolatedJointTransforms.resize(skeleton.Hierarchy.size());

	// Interpolate will sort out the interpolation for every joint's animation, thus returns a matrix for every iteration
	for (int i = 0; i < skeleton.Hierarchy.size(); i++) {

		CalculateJointMatrix(i, animation, timePos, interpolatedJointTransforms[i]); // check interpolations
	}
	
	//With all the calculated matrices at our disposal, let's update the transformations in the secondary joint array
	for (size_t i = 0; i < skeleton.Hierarchy.size(); i++) {

		// Create a reference to the currenct joint to be processed
		const Joint &b = skeleton.Hierarchy[i];

		// Create the matrix by multiplying the joint global transformation with the inverse bind pose
		XMStoreFloat4x4(at + i, XMMatrixTranspose(b.inverseBindPoseMatrix * interpolatedJointTransforms[i]));// *XMMatrixScaling(-1, 1, 1)));
	}

	StopProfile;
}

void SE::Core::AnimationSystem::CalculateJointMatrix(int jointIndex,const Animation& animation, float animTimePos, DirectX::XMMATRIX& out) const {

	StartProfile;
	// Animation has just started, so return the first keyframe
	int animationLength = animation.Length - 1;
	/*Something was called without any animation loaded*/
	if(animationLength == -1)
	{
		StopProfile;
		return;
	}
	auto& joint = animation.Joints[jointIndex];

	if(joint.Keyframes.size() > 0){

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

	}
	StopProfile;
}
static const SE::Utilz::GUID VS_SKINNED_DATA("VS_SKINNED_DATA");

SE::Core::RenderableManagerInstancing::RenderBucket * SE::Core::AnimationSystem::CreateBucket(Graphics::RenderJob & job)
{
	StartProfile;
	auto bucket = new AnimationBucket(job.pipeline);
	bucket->pipeline = job.pipeline;
	//	job.maxInstances = 256; Set from the outside
	auto hax = job.specialHaxxor;
	job.mappingFunc.push_back([this, bucket, hax](auto a, auto b) {
		renderer->GetPipelineHandler()->UpdateConstantBuffer(hax, &bucket->transforms[a], sizeof(DirectX::XMFLOAT4X4) * b);
	});
	job.mappingFunc.push_back([this, bucket, hax](auto a, auto b) {
		renderer->GetPipelineHandler()->UpdateConstantBuffer(VS_SKINNED_DATA, &bucket->matrices[mapingIndex][a], sizeof(JointMatrices) * b);
	});
	ProfileReturnConst( bucket);
}

void SE::Core::AnimationSystem::ReturnFirstFrameMatrix(const JointKeyFrame& joint, DirectX::XMMATRIX& out) const {
	
	StartProfile;
	XMVECTOR S = XMLoadFloat4(&joint.Keyframes[0].Scale);
	XMVECTOR T = XMLoadFloat4(&joint.Keyframes[0].Translation);
	XMVECTOR Q = XMLoadFloat4(&joint.Keyframes[0].RotationQuat);

	XMVECTOR zero = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f); // Origo?

	out = XMMatrixAffineTransformation(S, zero, Q, T);
	StopProfile;
}

void SE::Core::AnimationSystem::ReturnLastFrameMatrix(const JointKeyFrame& joint, const Animation& animation, DirectX::XMMATRIX& out) const {

	StartProfile;
	size_t animationLength = static_cast<size_t>(animation.Length - 1);

	XMVECTOR S = XMLoadFloat4(&joint.Keyframes[animationLength].Scale);
	XMVECTOR T = XMLoadFloat4(&joint.Keyframes[animationLength].Translation);
	XMVECTOR Q = XMLoadFloat4(&joint.Keyframes[animationLength].RotationQuat);

	XMVECTOR zero = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);

	out = XMMatrixAffineTransformation(S, zero, Q, T);
	StopProfile;
}

void SE::Core::AnimationSystem::Interpolate(const JointKeyFrame& joint, float animTimePos, DirectX::XMMATRIX& out) const
{
	StartProfile;
	// I am using an int here to truncate the animation timepose to know which matrices I am interested about
	// Ex. if time is 1.2, the returning frame is 1.
	int currentFrameIndex = animTimePos;

	float kFirst = joint.Keyframes[currentFrameIndex].TimePos;
	float kLast = joint.Keyframes[currentFrameIndex + 1].TimePos;

	// The interpolation percent will be mainly responsible of returning a slightly changed matrix
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

void SE::Core::AnimationSystem::AnimationBucket::AddEntity(const Entity & entity, const DirectX::XMFLOAT4X4 & transform, BucketAndID & bucketAndID)
{
	RenderBucket::AddEntity(entity, transform, bucketAndID);

	matrices[0].push_back(mats);
	matrices[1].push_back(mats);
}

void SE::Core::AnimationSystem::AnimationBucket::RemoveFromBucket(RenderableManagerInstancing * rm, size_t index, DirectX::XMFLOAT4X4 * transform)
{
	const auto last = matrices[0].size() - 1;
	matrices[1][index] = matrices[1][last];
	matrices[0][index] = matrices[0][last];
	
	matrices[0].pop_back();
	matrices[1].pop_back();

	RenderBucket::RemoveFromBucket(rm, index, transform);

}
