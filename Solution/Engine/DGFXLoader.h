#pragma once

#include "IModelFactory.h"
#include <fstream>
#include <string>
#include <unordered_map>

namespace Prism
{
	class Animation;
	class Model;
	class ModelAnimated;
	class Effect;
	class HierarchyBone;


	class DGFXLoader : public IModelFactory
	{
	public:
		DGFXLoader();
		~DGFXLoader();

		Model* LoadModel(const std::string& aFilePath) override;
		ModelAnimated* LoadAnimatedModel(const std::string& aFilePath) override;
		Animation* LoadAnimation(const std::string& aFilePath) override;
		void GetHierarchyToBone(const std::string& aAnimationPath, const std::string& aBoneName, GUIBone& aBoneOut) override;


		bool CheckIfFbxIsNewer(const std::string& aDGFXPath);
	private:
		enum eVertexLayout
		{
			VERTEX_POS,
			VERTEX_NORMAL,
			VERTEX_UV,
			VERTEX_BINORMAL,
			VERTEX_TANGENT,
			VERTEX_SKINWEIGHTS,
			VERTEX_BONEID,
			VERTEX_COLOR,
		};
		enum eTextureType
		{
			ALBEDO,
			NORMAL,
			ROUGHNESS,
			METALNESS,
			AMBIENT,
			EMISSIVE,
			NR_OF_TEXTURETYPES,
		};

		Model* CreateModelHeader(std::ifstream& aStream);
		Model* CreateModel(std::ifstream& aStream, Model* aModel = nullptr);
		ModelAnimated* CreateModelAnimatedHeader(std::ifstream& aStream);
		ModelAnimated* CreateModelAnimated(const std::string& aFBXPath, std::ifstream& aStream, ModelAnimated* aModelAnimated = nullptr);

		void LoadData(VertexIndexWrapper* aIndexWrapper, VertexDataWrapper* aVertexData
			, CU::GrowingArray<D3D11_INPUT_ELEMENT_DESC*>& someInputElements, Surface& aSurface
			, std::ifstream& aStream);

		void LoadLodGroup(Model* aOutData, std::ifstream& aStream);

		Animation* LoadAnimation(const std::string& aFBXPath, ModelAnimated* aOutData, std::ifstream& aStream);
		void LoadBoneHierarchy(HierarchyBone& aOutBone, std::ifstream& aStream);

	

		std::unordered_map<std::string, Model*> myModels;
		std::unordered_map<std::string, ModelAnimated*> myModelsAnimated;
		std::unordered_map<std::string, Animation*> myAnimations;
	};

}