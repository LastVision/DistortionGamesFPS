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

		Model* CreateModel(std::fstream& aStream);
		ModelAnimated* CreateModelAnimated(const std::string& aFBXPath, std::fstream& aStream);

		void LoadData(VertexIndexWrapper* aIndexWrapper, VertexDataWrapper* aVertexData
			, CU::GrowingArray<D3D11_INPUT_ELEMENT_DESC*>& someInputElements, Surface& aSurface
			, std::fstream& aStream);

		void LoadLodGroup(Model* aOutData, std::fstream& aStream);

		Animation* LoadAnimation(const std::string& aFBXPath, ModelAnimated* aOutData, std::fstream& aStream);
		void LoadBoneHierarchy(HierarchyBone& aOutBone, std::fstream& aStream);

	

		std::unordered_map<std::string, Model*> myModels;
		std::unordered_map<std::string, ModelAnimated*> myModelsAnimated;
		std::unordered_map<std::string, Animation*> myAnimations;
	};

}