#pragma once

#include "IModelFactory.h"
#include <string>
#include <vector>
#include <unordered_map>
#include <GrowingArray.h>
#include <Vector.h>
#include <Matrix.h>

struct AnimationData;
struct Bone;
struct ModelData;
struct D3D11_INPUT_ELEMENT_DESC;

class FBXLoader;
class FbxModelData;

namespace Prism
{
	class Animation;
	class Model;
	class ModelAnimated;
	class Effect;
	class HierarchyBone;
	class Surface;
	struct LodGroup;
	struct VertexIndexWrapper;
	struct VertexDataWrapper;

	class FBXFactory : public IModelFactory
	{
	public:
		FBXFactory();
		~FBXFactory();

		Model* LoadModel(const std::string& aFilePath) override;
		ModelAnimated* LoadAnimatedModel(const std::string& aFilePath) override;
		Animation* LoadAnimation(const std::string& aFilePath) override;
		void GetHierarchyToBone(const std::string& aAnimationPath, const std::string& aBoneName, GUIBone& aBoneOut) override;
		void LoadModelForRadiusCalc(const char* aFilePath, CU::GrowingArray<CU::Vector3<float>>& someVerticesOut);

		void ConvertToDGFX(const char* aInputPath, const char* aOutputPath);
		void ConvertToDGFX(const char* aInputPath, const char* aOutputPath, CU::GrowingArray<std::string>& someOutErrors);
		
	private:
		void LoadData(VertexIndexWrapper* aIndexWrapper, VertexDataWrapper* aVertexData
			, CU::GrowingArray<D3D11_INPUT_ELEMENT_DESC*>& someInputElements, Surface& aSurface
			, ModelData* someData);

		void FillAnimationData(FbxModelData* someData, ModelAnimated* outData);
		Animation* FillBoneAnimationData(const std::string& aFBXPath, FbxModelData* someData, ModelAnimated* aOutData);
		void BuildBoneHierarchy(Bone& aBone, AnimationData* aAnimationData, HierarchyBone& aOutBone);

		Model* CreateModel(FbxModelData* someModelData);
		ModelAnimated* CreateModelAnimated(const std::string& aFBXPath, FbxModelData* someModelData);

		void CreateModelForRadiusCalc(FbxModelData* someModelData, CU::GrowingArray<CU::Vector3<float>>& someVerticesOut
			, const CU::Matrix44<float>& aParentOrientation = CU::Matrix44<float>());
		void FillDataForRadiusCalc(ModelData* aModelData, CU::GrowingArray<CU::Vector3<float>>& someVerticesOut
			, const CU::Matrix44<float>& aOrientation);

		void SaveHeaderToFile(FbxModelData* aModelData, std::fstream& aStream);
		void SaveModelToFile(FbxModelData* aModelData, std::fstream& aStream);
		void SaveModelDataToFile(ModelData* aData, std::fstream& aStream);
		void SaveLodGroupToFile(Prism::LodGroup* aGroup, std::fstream& aStream);
		void SaveAnimationToFile(FbxModelData* aModeldata, std::fstream& aStream);
		void SaveBoneHierarchyToFile(Bone& aBone, AnimationData* aAnimationData, std::fstream& aStream);
		void RemoveExtraVertices(ModelData* aData);

		FBXLoader *myLoader;

		struct FBXData
		{
			FBXData() :myData(nullptr){}
			~FBXData();
			FbxModelData* myData;
			std::string myPath;
		};
		std::vector<FBXData*> myFBXData;
		std::unordered_map<std::string, Model*> myModels;
		std::unordered_map<std::string, ModelAnimated*> myModelsAnimated;
		std::unordered_map<std::string, Animation*> myAnimations;
	};
}