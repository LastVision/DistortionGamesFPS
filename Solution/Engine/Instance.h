#pragma once

#include "Defines.h"
#include "LightStructs.h"
#include <Matrix.h>
#include <StaticArray.h>
#include "TransformationNodeInstance.h"

namespace Prism
{
	class Animation;
	class Camera;
	class Light;
	class ModelAnimated;
	class ModelProxy;
	class InstancingHelper;
	enum class eOctreeType;

	class Instance
	{
	public:
		Instance(ModelProxy& aModel, const CU::Matrix44<float>& anOrientation, eOctreeType anOctreeType
			, const float& aObjectCullingRadius, bool aAlwaysRender = false, bool aCastShadow = true);
		~Instance();

		void Update(float aDelta);
		void Render(const Camera& aCamera, bool aIsDepthRender);
		void Render(const Camera& aCamera, InstancingHelper& aInstancingHelper, bool aIsDepthRender);

		CU::Vector3<float> GetPosition() const;

		void SetEffect(const std::string& aEffectFile);
		void SetScale(const CU::Vector3<float>& aScaleVector);

		void UpdateDirectionalLights(
			const CU::StaticArray<DirectionalLightData, NUMBER_OF_DIRECTIONAL_LIGHTS>& someDirectionalLightData);
		void UpdatePointLights(const CU::StaticArray<PointLightData, NUMBER_OF_POINT_LIGHTS>& somePointLightData);
		void UpdateSpotLights(const CU::StaticArray<SpotLightData, NUMBER_OF_SPOT_LIGHTS>& someSpotLightData);
		
		void SetAnimation(Animation* aAnimation);
		Animation* GetCurrentAnimation() const;
		bool IsAnimationDone();
		void ResetAnimationTime(float aTime);

		float GetObjectCullingRadius() const;
		eOctreeType GetOctreeType() const;

		void ActivateAlbedo(eOwnerType aOwner);

		void SetShouldRender(bool aStatus);
		bool GetShouldRender() const;

		void SetRenderThroughCulling(bool aStatus);
		bool GetRenderThroughCulling() const;

		bool GetAlwaysRender() const;
		bool GetCastShadow() const;

		ModelProxy& GetModel();
		const CU::Matrix44f& GetOrientation() const;
	private:
		void operator=(Instance&) = delete;

		void RenderModelAnimated(ModelAnimated* aModel, const CU::Matrix44<float>& aParent
			, const Camera& aCamera, TransformationNodeInstance& aHierarchy, bool aIsDepthRender);
		void BuildHierarchy(TransformationNodeInstance& aHierarchy, ModelAnimated* aModel);
		bool myHierarchyIsBuilt;

		ModelProxy& myProxy;
		const eOctreeType myOctreeType;
		const CU::Matrix44<float>& myOrientation;
		CU::Vector3<float> myScale;

		const float& myObjectCullingRadius;

		TransformationNodeInstance myHierarchy;
		Animation* myAnimation;
		float myTotalTime;
		CU::StaticArray<CU::Matrix44<float>, MAX_NR_OF_BONES> myBones;
		eOwnerType myOwnerType;

		CU::Vector4<float> mySelectionColor;

		bool myShouldRender;
		bool myRenderThroughCulling;
		bool myAlwaysRender;
		bool myCastShadow;
	};

	inline float Instance::GetObjectCullingRadius() const
	{
		return myObjectCullingRadius;
	}

	inline eOctreeType Instance::GetOctreeType() const
	{
		return myOctreeType;
	}

	inline const CU::Matrix44f& Instance::GetOrientation() const
	{
		return myOrientation;
	}
}