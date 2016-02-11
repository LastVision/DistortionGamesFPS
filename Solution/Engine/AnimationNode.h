#pragma once

namespace Prism
{
	struct AnimationNodeValue
	{
		float myTime;
		CU::Matrix44<float> myMatrix;
	};

	class AnimationNode
	{
		friend class Animation;
		friend class FBXFactory;
		friend class DGFXLoader;
	public:
		AnimationNode(int aNumOfValues);
		~AnimationNode();

		void AddValue(const AnimationNodeValue& aFrame);
		bool HasValues() const;
		void CalculateEndTime();

		CU::Matrix44<float> GetCurrentMatrix(float aTime);
		float GetAnimationLenght();

	private:
		CU::GrowingArray<AnimationNodeValue> myValues;
		float myEndTime;
		std::string myBoneName;
	};
}