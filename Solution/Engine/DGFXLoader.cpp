#include "stdafx.h"

#include "Animation.h"
#include "AnimationNode.h"
#include "BaseModel.h"
#include <CommonHelper.h>
#include "DGFXLoader.h"
#include "Model.h"
#include "ModelAnimated.h"
#include "HierarchyBone.h"
#include "VertexDataWrapper.h"
#include "VertexIndexWrapper.h"
#include "Surface.h"
#include <TimerManager.h>

namespace Prism
{
	DGFXLoader::DGFXLoader()
	{
	}


	DGFXLoader::~DGFXLoader()
	{
		for (auto it = myModels.begin(); it != myModels.end(); ++it)
		{
			SAFE_DELETE(it->second);
		}
		myModels.clear();

		for (auto it = myModelsAnimated.begin(); it != myModelsAnimated.end(); ++it)
		{
			SAFE_DELETE(it->second);
		}
		myModelsAnimated.clear();
	}

	Model* DGFXLoader::LoadModel(const std::string& aFilePath)
	{
		if (myModels.find(aFilePath) != myModels.end())
		{
			return myModels[aFilePath];
		}

		std::string dgfxFile = CU::GetGeneratedDataFolderFilePath(aFilePath, "dgfx");

#ifndef RELEASE_BUILD
		if (CheckIfFbxIsNewer(dgfxFile) == true)
		{
			DL_MESSAGE_BOX("Found a FBX-File thats newer than the DGFX-File, did you forget to run the tool?", "Old DGFX", MB_ICONQUESTION);
		}
#endif


		CU::TimerManager::GetInstance()->StartTimer("LoadDGFX");

		std::fstream file;
		file.open(dgfxFile.c_str(), std::ios::in | std::ios::binary);
		DL_ASSERT_EXP(file.fail() == false, CU::Concatenate("Failed to open %s, did you forget to run the tool?", dgfxFile.c_str()));
		if (file.fail() == true)
		{
			assert(false && "FAILED TO OPEN DGFX-FILE");
			return nullptr;
		}

		Model* newModel = CreateModel(file);

		file.close();

		int elapsed = static_cast<int>(
			CU::TimerManager::GetInstance()->StopTimer("LoadDGFX").GetMilliseconds());
		RESOURCE_LOG("DGFX-Model \"%s\" took %d ms to load", dgfxFile.c_str(), elapsed);

		myModels[aFilePath] = newModel;

		return newModel;
	}

	ModelAnimated* DGFXLoader::LoadAnimatedModel(const std::string& aFilePath)
	{
		if (myModelsAnimated.find(aFilePath) != myModelsAnimated.end())
		{
			return myModelsAnimated[aFilePath];
		}

		std::string dgfxFile = CU::GetGeneratedDataFolderFilePath(aFilePath, "dgfx");

#ifndef RELEASE_BUILD
		if (CheckIfFbxIsNewer(dgfxFile) == true)
		{
			DL_MESSAGE_BOX("Found a FBX-File thats newer than the DGFX-File, did you forget to run the tool?", "Old DGFX", MB_ICONQUESTION);
		}
#endif

		CU::TimerManager::GetInstance()->StartTimer("LoadDGFXAnimated");

		std::fstream file;
		file.open(dgfxFile.c_str(), std::ios::in | std::ios::binary);
		DL_ASSERT_EXP(file.fail() == false, CU::Concatenate("Failed to open %s, did you forget to run the tool?", dgfxFile.c_str()));
		if (file.fail() == true)
		{
			assert(false && "FAILED TO OPEN DGFX-FILE");
			return nullptr;
		}

		ModelAnimated* newModel = CreateModelAnimated(aFilePath, file);

		file.close();

		int elapsed = static_cast<int>(
			CU::TimerManager::GetInstance()->StopTimer("LoadDGFXAnimated").GetMilliseconds());
		RESOURCE_LOG("Animated DGFX-Model \"%s\" took %d ms to load", dgfxFile.c_str(), elapsed);

		myModelsAnimated[aFilePath] = newModel;

		return newModel;
	}

	Animation* DGFXLoader::LoadAnimation(const std::string& aFilePath)
	{
		if (myAnimations.find(aFilePath) != myAnimations.end())
		{
			return myAnimations[aFilePath];
		}

		std::string dgfxFile = CU::GetGeneratedDataFolderFilePath(aFilePath, "dgfx");

#ifndef RELEASE_BUILD
		if (CheckIfFbxIsNewer(dgfxFile) == true)
		{
			DL_MESSAGE_BOX("Found a FBX-File thats newer than the DGFX-File, did you forget to run the tool?", "Old DGFX", MB_ICONQUESTION);
		}
#endif

		CU::TimerManager::GetInstance()->StartTimer("LoadAnimationDGFX");


		std::fstream stream;
		stream.open(dgfxFile.c_str(), std::ios::in | std::ios::binary);
		DL_ASSERT_EXP(stream.fail() == false, CU::Concatenate("Failed to open %s, did you forget to run the tool?", dgfxFile.c_str()));
		if (stream.fail() == true)
		{
			_wassert(L"FAILED TO OPEN DGFX-FILE", L"DGFXLoader.cpp", 159);
			return nullptr;
		}

		int fileVersion = -1;
		stream.read((char*)&fileVersion, sizeof(int));
		DL_ASSERT_EXP(fileVersion == DGFX_VERSION, "Found a old DGFX-Animation, try running the Converter again");
		if (fileVersion != DGFX_VERSION)
		{
			_wassert(L"FOUND OLD ANIMATION_DGFX", L"DGFXLoader.cpp", 173);
			return nullptr;
		}

		int isNullObject = -1;
		stream.read((char*)&isNullObject, sizeof(int));

		int isLodGroup = -1;
		stream.read((char*)&isLodGroup, sizeof(int));
		DL_ASSERT_EXP(isLodGroup == 0, "AnimatedModel cant be LOD'ed, not supported yet");

		int isAnimated = -1;
		stream.read((char*)&isAnimated, sizeof(int));

		if (isNullObject == 0)
		{
			_wassert(L"ANIMATION NEEDS TO BE A NULLOBJECT, BUT IT WASNT", L"DGFXLoader.cpp", 189);
		}

		Animation* animation = LoadAnimation(aFilePath, nullptr, stream);
		stream.close();

		int elapsed = static_cast<int>(
			CU::TimerManager::GetInstance()->StopTimer("LoadAnimationDGFX").GetMilliseconds());
		RESOURCE_LOG("DGFX-Animation \"%s\" took %d ms to load", dgfxFile.c_str(), elapsed);

		return animation;
	}

	Model* DGFXLoader::CreateModel(std::fstream& aStream)
	{
		Model* tempModel = new Model();

		int fileVersion = -1;
		aStream.read((char*)&fileVersion, sizeof(int));
		DL_ASSERT_EXP(fileVersion == DGFX_VERSION, "Found a old DGFX-Model, try running the Converter again");
		if (aStream.fail() == true)
		{
			assert(false && "Found a old DGFX-Model, RELEASE-ASSERT");
			return nullptr;
		}

		int isNullObject = -1;
		aStream.read((char*)&isNullObject, sizeof(int));

		int isLodGroup = -1;
		aStream.read((char*)&isLodGroup, sizeof(int));

		int isAnimated = -1;
		aStream.read((char*)&isAnimated, sizeof(int));

		if (isNullObject == 0)
		{
			tempModel->myIsNULLObject = false;

			VertexIndexWrapper* indexWrapper = new VertexIndexWrapper();
			VertexDataWrapper* vertexData = new VertexDataWrapper();
			Surface* surface = new Surface();

			LoadData(indexWrapper, vertexData, tempModel->myVertexFormat, *surface
				, aStream);

			tempModel->mySurfaces.Add(surface);
			tempModel->myIndexBaseData = indexWrapper;
			tempModel->myVertexBaseData = vertexData;

			CU::Matrix44<float> matrix;
			aStream.read((char*)&tempModel->myOrientation.myMatrix[0], sizeof(float) * 16);
		}

		if (isLodGroup == 1)
		{
			LoadLodGroup(tempModel, aStream);
		}

		int childCount = 0;
		aStream.read((char*)&childCount, sizeof(int));
		for (int i = 0; i < childCount; ++i)
		{
			tempModel->AddChild(CreateModel(aStream));
		}

		return tempModel;
	}

	ModelAnimated* DGFXLoader::CreateModelAnimated(const std::string& aFBXPath, std::fstream& aStream)
	{
		ModelAnimated* tempModel = new ModelAnimated();

		int fileVersion = -1;
		aStream.read((char*)&fileVersion, sizeof(int));
		DL_ASSERT_EXP(fileVersion == DGFX_VERSION, "Found a old Animated DGFX-Model, try running the Converter again");
		if (aStream.fail() == true)
		{
			assert(false && "Found a old Animated DGFX-Model, RELEASE-ASSERT");
			return nullptr;
		}

		int isNullObject = -1;
		aStream.read((char*)&isNullObject, sizeof(int));

		int isLodGroup = 0;
		aStream.read((char*)&isLodGroup, sizeof(int));
		DL_ASSERT_EXP(isLodGroup == 0, "AnimatedModel cant be LOD'ed, not supported yet");

		int isAnimated = 0;
		aStream.read((char*)&isAnimated, sizeof(int));

		if (isNullObject == 0)
		{
			tempModel->myIsNULLObject = false;

			VertexIndexWrapper* indexWrapper = new VertexIndexWrapper();
			VertexDataWrapper* vertexData = new VertexDataWrapper();
			Surface* surface = new Surface();

			LoadData(indexWrapper, vertexData, tempModel->myVertexFormat, *surface
				, aStream);

			tempModel->mySurfaces.Add(surface);
			tempModel->myIndexBaseData = indexWrapper;
			tempModel->myVertexBaseData = vertexData;

			CU::Matrix44<float> matrix;
			aStream.read((char*)&tempModel->myOrientation.myMatrix[0], sizeof(float) * 16);
		}

		if (isAnimated == 1)
		{
			LoadAnimation(aFBXPath, tempModel, aStream);
		}

		int childCount = 0;
		aStream.read((char*)&childCount, sizeof(int));
		for (int i = 0; i < childCount; ++i)
		{
			tempModel->AddChild(CreateModelAnimated(aFBXPath, aStream));
		}

		return tempModel;
	}

	void DGFXLoader::LoadData(VertexIndexWrapper* aIndexWrapper, VertexDataWrapper* aVertexData
		, CU::GrowingArray<D3D11_INPUT_ELEMENT_DESC*>& someInputElements, Surface& aSurface
		, std::fstream& aStream)
	{
		int indexCount = 0;
		aStream.read((char*)&indexCount, sizeof(int)); //Index count

		unsigned int* indexData = new unsigned int[indexCount];
		aStream.read((char*)(indexData), sizeof(int) * indexCount); //All index data

		aIndexWrapper->myFormat = DXGI_FORMAT_R32_UINT;
		aIndexWrapper->myIndexData = (char*)indexData;
		aIndexWrapper->mySize = indexCount * sizeof(unsigned int);
		aIndexWrapper->myNumberOfIndices = indexCount;


		int vertexCount = 0;
		aStream.read((char*)&vertexCount, sizeof(int)); //Vertex count
		int stride = 0;
		aStream.read((char*)&stride, sizeof(int)); //Vertex stride

		int sizeOfBuffer = vertexCount * stride * sizeof(float);
		char* vertexRawData = new char[sizeOfBuffer];
		aStream.read(vertexRawData, sizeOfBuffer); //All vertex data

		aVertexData->myVertexData = vertexRawData;
		aVertexData->myNumberOfVertices = vertexCount;
		aVertexData->mySize = sizeOfBuffer;
		aVertexData->myStride = stride*sizeof(float);


		int layoutCount = 0;
		aStream.read((char*)&layoutCount, sizeof(int)); //Inputlayout element count

		for (int i = 0; i < layoutCount; ++i)
		{
			int byteOffset = -1;
			aStream.read((char*)&byteOffset, sizeof(int)); //Inputlayout element count

			int semanticIndex = -1;
			aStream.read((char*)&semanticIndex, sizeof(int)); //Inputlayout semantic index

			D3D11_INPUT_ELEMENT_DESC* desc = new D3D11_INPUT_ELEMENT_DESC();
			desc->SemanticIndex = semanticIndex;
			desc->AlignedByteOffset = byteOffset;
			desc->InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
			desc->InputSlot = 0;
			desc->InstanceDataStepRate = 0;

			int type = -1;
			aStream.read((char*)&type, sizeof(int)); //semanticName lenght

			if (type == eVertexLayout::VERTEX_POS)
			{
				desc->SemanticName = "POSITION";
				desc->Format = DXGI_FORMAT_R32G32B32_FLOAT;
			}
			else if (type == eVertexLayout::VERTEX_NORMAL)
			{
				desc->SemanticName = "NORMAL";
				desc->Format = DXGI_FORMAT_R32G32B32_FLOAT;
			}
			else if (type == eVertexLayout::VERTEX_UV)
			{
				desc->SemanticName = "TEXCOORD";
				desc->Format = DXGI_FORMAT_R32G32_FLOAT;
			}
			else if (type == eVertexLayout::VERTEX_BINORMAL)
			{
				desc->SemanticName = "BINORMAL";
				desc->Format = DXGI_FORMAT_R32G32B32_FLOAT;
			}
			else if (type == eVertexLayout::VERTEX_TANGENT)
			{
				desc->SemanticName = "TANGENT";
				desc->Format = DXGI_FORMAT_R32G32B32_FLOAT;
			}
			else if (type == eVertexLayout::VERTEX_SKINWEIGHTS)
			{
				desc->SemanticName = "WEIGHTS";
				desc->Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
			}
			else if (type == eVertexLayout::VERTEX_BONEID)
			{
				desc->SemanticName = "BONES";
				desc->Format = DXGI_FORMAT_R32G32B32A32_UINT;
			}
			else if (type == eVertexLayout::VERTEX_COLOR)
			{
				desc->SemanticName = "COLOR";
				desc->Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
			}
			else
			{
				DL_ASSERT("Found a invalid InputElement while loading DGFX");
				assert(false && "Found a invalid InputElement while loading DGFX, RELEASE-ASSERT");
			}

			someInputElements.Add(desc);
		}

		aSurface.SetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		aSurface.SetIndexCount(indexCount);
		aSurface.SetVertexStart(0);
		aSurface.SetIndexStart(0);
		aSurface.SetVertexCount(vertexCount);

		int textureCount = 0;
		aStream.read((char*)&textureCount, sizeof(int)); //numberOfTextures
		for (int i = 0; i < textureCount; ++i)
		{
			int textureType = -1;
			aStream.read((char*)&textureType, sizeof(int)); //textureType

			std::string resourceName;
			if (textureType == eTextureType::ALBEDO)
			{
				resourceName = "AlbedoTexture";
			}
			else if (textureType == eTextureType::NORMAL)
			{
				resourceName = "NormalTexture";
			}
			else if (textureType == eTextureType::ROUGHNESS)
			{
				resourceName = "RoughnessTexture";
			}
			else if (textureType == eTextureType::METALNESS)
			{
				resourceName = "MetalnessTexture";
			}
			else if (textureType == eTextureType::AMBIENT)
			{
				resourceName = "AOTexture";
			}
			else if (textureType == eTextureType::EMISSIVE)
			{
				resourceName = "EmissiveTexture";
			}
			else
			{
				DL_ASSERT("Found a invalid TextureType while loading DGFX");
				assert(false && "Found a invalid TextureType while loading DGFX, RELEASE-ASSERT");
			}


			int textureLenght = 0;
			aStream.read((char*)&textureLenght, sizeof(int)); //currentTexture.myFileName lenght
			char* texture = new char[textureLenght + 1];
			aStream.read(texture, sizeof(char) * textureLenght); //currentTexture.myFileName
			texture[textureLenght] = '\0';

			aSurface.SetTexture(resourceName, texture, false);
			delete texture;
		}
	}

	void DGFXLoader::LoadLodGroup(Model* aOutData, std::fstream& aStream)
	{
		Prism::LodGroup* lodGroup = new Prism::LodGroup();

		int lodCount = 0;
		aStream.read((char*)&lodCount, sizeof(int));

		lodGroup->myLods.Reserve(lodCount);
		aStream.read((char*)&lodGroup->myLods[0], sizeof(Prism::Lod) * lodCount);


		int threshHoldCount = 0;
		aStream.read((char*)&threshHoldCount, sizeof(int));

		lodGroup->myThreshHolds.Reserve(threshHoldCount);
		aStream.read((char*)&lodGroup->myThreshHolds[0], sizeof(double) * threshHoldCount);

		aOutData->SetLodGroup(lodGroup);
	}

	Animation* DGFXLoader::LoadAnimation(const std::string& aFBXPath, ModelAnimated* aOutData, std::fstream& aStream)
	{
		CU::Matrix44<float> bindMatrix;
		aStream.read((char*)&bindMatrix.myMatrix[0], sizeof(float) * 16);

		HierarchyBone rootBone;
		LoadBoneHierarchy(rootBone, aStream);

		Animation* newAnimation = new Animation();

		int nrOfbones = 0;
		aStream.read((char*)&nrOfbones, sizeof(int));

		for (int i = 0; i < nrOfbones; ++i)
		{
			int boneNameLenght = 0;
			aStream.read((char*)&boneNameLenght, sizeof(int));

			char* boneName = new char[boneNameLenght+1];
			aStream.read(boneName, sizeof(char) * boneNameLenght);
			boneName[boneNameLenght] = '\0';

			CU::Matrix44<float> boneMatrix;
			aStream.read((char*)&boneMatrix.myMatrix[0], sizeof(float) * 16);

			CU::Matrix44<float> boneBindMatrix;
			aStream.read((char*)&boneBindMatrix.myMatrix[0], sizeof(float) * 16);


			int nrOfFrames = 0;
			aStream.read((char*)&nrOfFrames, sizeof(int));
			if (nrOfFrames > 0)
			{
				AnimationNode* newNode = new AnimationNode(nrOfFrames);
				newNode->myValues.Reserve(nrOfFrames);

				aStream.read((char*)&newNode->myValues[0], sizeof(AnimationNodeValue) * nrOfFrames);
				newNode->CalculateEndTime();

				newNode->myBoneName = boneName;
				newAnimation->AddAnimation(newNode);
			}

			newAnimation->SetBoneMatrix(boneMatrix, i);
			newAnimation->SetBoneBindPose(boneBindMatrix, i);
			newAnimation->AddBoneName(boneName);

			newAnimation->SetBindMatrix(bindMatrix);
			delete[] boneName;
		}

		newAnimation->SetHierarchy(rootBone);

		float animationLenght = 0.f;
		aStream.read((char*)&animationLenght, sizeof(float));

		newAnimation->SetAnimationLenght(animationLenght);

		if (aOutData != nullptr)
		{
			aOutData->myAnimation = newAnimation;
		}

		myAnimations[aFBXPath] = newAnimation;
		return newAnimation;
	}

	void DGFXLoader::LoadBoneHierarchy(HierarchyBone& aOutBone, std::fstream& aStream)
	{
		aStream.read((char*)&aOutBone.myBoneID, sizeof(int));

		int boneNameLenght = 0;
		aStream.read((char*)&boneNameLenght, sizeof(int));

		char* boneName = new char[boneNameLenght+1];
		aStream.read(boneName, sizeof(char) * boneNameLenght);
		boneName[boneNameLenght] = '\0';
		aOutBone.myBoneName = boneName;
		
		int nrOfChildren = 0;
		aStream.read((char*)&nrOfChildren, sizeof(int));

		if (nrOfChildren > 0)
		{
			aOutBone.myChildren.Init(nrOfChildren);
			for (int i = 0; i < nrOfChildren; ++i)
			{
				HierarchyBone child;
				LoadBoneHierarchy(child, aStream);
				child.myParent = &aOutBone;
				aOutBone.myChildren.Add(child);
			}
		}

		delete[] boneName;
	}

	bool DGFXLoader::CheckIfFbxIsNewer(const std::string& aDGFXPath)
	{
		std::string fbxPath = CU::GetRealDataFolderFilePath(aDGFXPath, "fbx");

		HANDLE dgfxHandle;
		HANDLE fbxHandle;

		dgfxHandle = CreateFile(aDGFXPath.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL
			, OPEN_EXISTING, 0, NULL);

		fbxHandle = CreateFile(fbxPath.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL
			, OPEN_EXISTING, 0, NULL);

		FILETIME dgfxTime;
		FILETIME fbxTime;

		GetFileTime(dgfxHandle, NULL, NULL, &dgfxTime);
		GetFileTime(fbxHandle, NULL, NULL, &fbxTime);

		if (CompareFileTime(&dgfxTime, &fbxTime) == -1)
		{
			return true;
		}

		return false;
	}
}