#include "stdafx.h"

#include "Animation.h"
#include "AnimationCurve.h"
#include "AnimationNode.h"
#include "AnimationSystem.h"
#include <CommonHelper.h>
#include "FBXFactory.h"
#include "FBX/FbxLoader.h"
#include "HierarchyBone.h"
#include "Matrix44.h"
#include "Model.h"
#include "ModelAnimated.h"
#include "Surface.h"
#include "IndexBufferWrapper.h"
#include "VertexBufferWrapper.h"
#include "VertexDataWrapper.h"
#include "VertexIndexWrapper.h"
#include <TimerManager.h>
#include "TransformationNodeCurves.h"

namespace Prism
{
	FBXFactory::FBXData::~FBXData()
	{
		delete myData;
	}

	FBXFactory::FBXFactory()
	{
		CU::Matrix44f test;
		test.SetPos(CU::Vector3f(1, 1, 1));
		CU::Matrix44f resultMatrix;

		myLoader = new FBXLoader();
	}

	FBXFactory::~FBXFactory()
	{
		for (unsigned int i = 0; i < myFBXData.size(); ++i)
		{
			delete myFBXData[i];
			myFBXData[i] = nullptr;
		}

		for (auto it = myModels.begin(); it != myModels.end(); ++it)
		{
			delete it->second;
			it->second = nullptr;
		}
		myModels.clear();

		for (auto it = myModelsAnimated.begin(); it != myModelsAnimated.end(); ++it)
		{
			delete it->second;
			it->second = nullptr;
		}
		myModelsAnimated.clear();

		delete myLoader;
	}

	void FBXFactory::LoadData(VertexIndexWrapper* aIndexWrapper, VertexDataWrapper* aVertexData
		, CU::GrowingArray<D3D11_INPUT_ELEMENT_DESC*>& someInputElements, Surface& aSurface
		, ModelData* someData)
	{
		aIndexWrapper->myFormat = DXGI_FORMAT_R32_UINT;
		unsigned int* indexData = new unsigned int[someData->myIndexCount];
		memcpy(indexData, someData->myIndicies, someData->myIndexCount*sizeof(unsigned int));
		aIndexWrapper->myIndexData = (char*)indexData;
		aIndexWrapper->mySize = someData->myIndexCount*sizeof(unsigned int);
		aIndexWrapper->myNumberOfIndices = someData->myIndexCount;

		int sizeOfBuffer = someData->myVertexCount*someData->myVertexStride*sizeof(float);
		char* vertexRawData = new char[sizeOfBuffer];
		memcpy(vertexRawData, someData->myVertexBuffer, sizeOfBuffer);
		aVertexData->myVertexData = vertexRawData;
		aVertexData->myNumberOfVertices = someData->myVertexCount;
		aVertexData->mySize = sizeOfBuffer;
		aVertexData->myStride = someData->myVertexStride*sizeof(float);

		for (int i = 0; i < someData->myLayout.Size(); ++i)
		{
			auto currentLayout = someData->myLayout[i];
			D3D11_INPUT_ELEMENT_DESC* desc = new D3D11_INPUT_ELEMENT_DESC();
			desc->SemanticIndex = currentLayout.mySemanticIndex;
			desc->AlignedByteOffset = currentLayout.myOffset;
			desc->InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
			desc->InputSlot = 0;
			desc->InstanceDataStepRate = 0;

			if (currentLayout.myType == ModelData::VERTEX_POS)
			{
				desc->SemanticName = "POSITION";
				desc->Format = DXGI_FORMAT_R32G32B32_FLOAT;
			}
			else if (currentLayout.myType == ModelData::VERTEX_NORMAL)
			{
				desc->SemanticName = "NORMAL";
				desc->Format = DXGI_FORMAT_R32G32B32_FLOAT;
			}
			else if (currentLayout.myType == ModelData::VERTEX_UV)
			{
				desc->SemanticName = "TEXCOORD";
				desc->Format = DXGI_FORMAT_R32G32_FLOAT;
			}
			else if (currentLayout.myType == ModelData::VERTEX_BINORMAL)
			{
				desc->SemanticName = "BINORMAL";
				desc->Format = DXGI_FORMAT_R32G32B32_FLOAT;
			}
			else if (currentLayout.myType == ModelData::VERTEX_TANGENT)
			{
				desc->SemanticName = "TANGENT";
				desc->Format = DXGI_FORMAT_R32G32B32_FLOAT;
			}
			else if (currentLayout.myType == ModelData::VERTEX_SKINWEIGHTS)
			{
				desc->SemanticName = "WEIGHTS";
				desc->Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
			}
			else if (currentLayout.myType == ModelData::VERTEX_BONEID)
			{
				desc->SemanticName = "BONES";
				desc->Format = DXGI_FORMAT_R32G32B32A32_UINT;
			}
			else if (currentLayout.myType == ModelData::VERTEX_COLOR)
			{
				desc->SemanticName = "COLOR";
				desc->Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
			}
			someInputElements.Add(desc);
		}

		aSurface.SetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		aSurface.SetIndexCount(someData->myIndexCount);
		aSurface.SetVertexStart(0);
		aSurface.SetIndexStart(0);
		aSurface.SetVertexCount(someData->myVertexCount);
		bool useSRGB = true;
		for (unsigned int i = 0; i < someData->myTextures.size(); ++i)
		{
			auto& currentTexture = someData->myTextures[i];

			std::string resourceName;
			if (currentTexture.myType == ALBEDO)
			{
				resourceName = "AlbedoTexture";
			}
			if (currentTexture.myType == NORMAL)
			{
				resourceName = "NormalTexture";
				useSRGB = false;
			}
			if (currentTexture.myType == ROUGHNESS)
			{
				resourceName = "RoughnessTexture";
			}
			if (currentTexture.myType == METALNESS)
			{
				resourceName = "MetalnessTexture";
			}
			if (currentTexture.myType == AMBIENT)
			{
				resourceName = "AOTexture";
			}
			if (currentTexture.myType == FBXTextureType::EMISSIVE)
			{
				resourceName = "EmissiveTexture";
			}

			aSurface.SetTexture(resourceName, currentTexture.myFileName, useSRGB);
		}
	}

	void FBXFactory::FillAnimationData(FbxModelData* someData, ModelAnimated* outData)
	{
		TransformationNodeCurves* nodeCurves = new TransformationNodeCurves();
		Prism::KeyFrame frame;
		fbxsdk::FbxAnimCurve* fbxCurve = nullptr;

		float startTime = FLT_MAX;
		float stopTime = FLT_MIN;

		//RotationCurves
		for (int i = 0; i < 3; ++i)
		{
			AnimationCurve* curve = new AnimationCurve();
			fbxCurve = someData->myAnimationCurves->myRotationCurve[i];
			int keyCount = fbxCurve->KeyGetCount();

			for (int j = 0; j < keyCount; ++j)
			{
				frame.myTime = static_cast<float>(fbxCurve->KeyGetTime(j).GetSecondDouble());
				frame.myValue = fbxCurve->KeyGetValue(j);
				curve->AddKeyFrame(frame);
			}

			nodeCurves->SetRotationCurve(i, curve);
			curve->FinalizeCurve();

			fbxsdk::FbxTimeSpan span;
			fbxCurve->GetTimeInterval(span);
			float start = static_cast<float>(span.GetStart().GetSecondDouble());
			float stop = static_cast<float>(span.GetStop().GetSecondDouble());

			startTime = fminf(start, startTime);
			stopTime = fmaxf(stop, stopTime);
		}

		//TransalationCurves
		for (int i = 0; i < 3; ++i)
		{
			AnimationCurve* curve = new AnimationCurve();
			fbxCurve = someData->myAnimationCurves->myTtranslationCurve[i];
			int keyCount = fbxCurve->KeyGetCount();

			for (int j = 0; j < keyCount; ++j)
			{
				frame.myTime = static_cast<float>(fbxCurve->KeyGetTime(j).GetSecondDouble());
				frame.myValue = fbxCurve->KeyGetValue(j);
				curve->AddKeyFrame(frame);
			}

			curve->FinalizeCurve();
			nodeCurves->SetTranslationCurve(i, curve);

			fbxsdk::FbxTimeSpan span;
			fbxCurve->GetTimeInterval(span);
			float start = static_cast<float>(span.GetStart().GetSecondDouble());
			float stop = static_cast<float>(span.GetStop().GetSecondDouble());

			startTime = fminf(start, startTime);
			stopTime = fmaxf(stop, stopTime);
		}

		//we are not reading KeyFrameCount at the moment, using -1 until we need the real count!
		nodeCurves->Init(AnimationType::RELATIVE_TRANSFORM, -1, static_cast<int>(someData->myFPS), startTime, stopTime);
		outData->myTransformation = nodeCurves;
	}

	Animation* FBXFactory::FillBoneAnimationData(const std::string& aFBXPath, FbxModelData* someData, ModelAnimated* aOutData)
	{
		auto loadedAnimation = someData->myAnimation;
		float animationLenght = 0.f;

		Animation* newAnimation = new Animation();

		HierarchyBone rootBone;
		BuildBoneHierarchy(loadedAnimation->myBones[loadedAnimation->myRootBone], loadedAnimation, rootBone);

		int nrOfbones = static_cast<int>(someData->myAnimation->myBones.size());
		for (int i = 0; i < nrOfbones; ++i)
		{
			Bone& currentBone = someData->myAnimation->myBones[i];

			AnimationNode* newNode = new AnimationNode(currentBone.myFrames.size());

			int nrOfFrames = currentBone.myFrames.size();
			for (int j = 0; j < nrOfFrames; ++j)
			{
				AnimationNodeValue newValue;
				auto currentFrame = currentBone.myFrames[j];

				newValue.myTime = currentFrame.myTime;
				newValue.myMatrix = currentFrame.myMatrix;

				newNode->AddValue(newValue);
			}

			newNode->myBoneName = currentBone.myName;
			newAnimation->AddAnimation(newNode);

			newAnimation->SetBoneMatrix(currentBone.myBaseOrientation, i);
			newAnimation->SetBoneBindPose(currentBone.myBindMatrix, i);
			newAnimation->AddBoneName(currentBone.myName);

			newAnimation->SetBindMatrix(loadedAnimation->myBindMatrix);
		}

		newAnimation->SetHierarchy(rootBone);
		animationLenght = someData->myAnimation->myBones[0].myAnimationTime;
		newAnimation->SetAnimationLenght(animationLenght);

		if (aOutData != nullptr)
		{
			aOutData->myAnimation = newAnimation;
		}

		myAnimations[aFBXPath] = newAnimation;
		return newAnimation;
	}

	void FBXFactory::BuildBoneHierarchy(Bone& aBone, AnimationData* aAnimationData, HierarchyBone& aOutBone)
	{
		aOutBone.myBoneID = aBone.myId;
		aOutBone.myBoneName = aBone.myName;
		const int nrOfChildren = static_cast<int>(aBone.myChilds.size());

		if (nrOfChildren > 0)
		{
			aOutBone.myChildren.Init(nrOfChildren);
			for (int i = 0; i < nrOfChildren; ++i)
			{
				HierarchyBone child;
				BuildBoneHierarchy(aAnimationData->myBones[aBone.myChilds[i]], aAnimationData, child);
				child.myParent = &aOutBone;
				aOutBone.myChildren.Add(child);
			}
		}
	}

	Model* FBXFactory::CreateModel(FbxModelData* someModelData)
	{
		Model* tempModel = new Model();
		tempModel->SetLodGroup(someModelData->myLodGroup);

		if (someModelData->myData)
		{
			tempModel->myIsNULLObject = false;

			VertexIndexWrapper* indexWrapper = new VertexIndexWrapper();
			VertexDataWrapper* vertexData = new VertexDataWrapper();
			Surface* surface = new Surface();

			LoadData(indexWrapper, vertexData, tempModel->myVertexFormat, *surface
				, someModelData->myData);

			tempModel->mySurfaces.Add(surface);
			tempModel->myIndexBaseData = indexWrapper;
			tempModel->myVertexBaseData = vertexData;
			tempModel->myOrientation = someModelData->myOrientation;
		}
		for (int i = 0; i < someModelData->myChildren.Size(); ++i)
		{
			auto currentChild = someModelData->myChildren[i];
			tempModel->AddChild(CreateModel(currentChild));
		}
		return tempModel;
	}

	ModelAnimated* FBXFactory::CreateModelAnimated(const std::string& aFBXPath, FbxModelData* someModelData)
	{
		ModelAnimated* tempModel = new ModelAnimated();

		if (someModelData->myData)
		{
			tempModel->myIsNULLObject = false;

			VertexIndexWrapper* indexWrapper = new VertexIndexWrapper();
			VertexDataWrapper* vertexData = new VertexDataWrapper();
			Surface* surface = new Surface();

			LoadData(indexWrapper, vertexData, tempModel->myVertexFormat, *surface
				, someModelData->myData);

			tempModel->mySurfaces.Add(surface);
			tempModel->myIndexBaseData = indexWrapper;
			tempModel->myVertexBaseData = vertexData;
			tempModel->myOrientation = someModelData->myOrientation;
		}

		if (someModelData->myAnimationCurves != nullptr
			&& someModelData->myAnimationCurves->myRotationCurve[0] != nullptr
			&& someModelData->myAnimationCurves->myTtranslationCurve[0] != nullptr)
		{
			FillAnimationData(someModelData, tempModel);
		}

		if (someModelData->myAnimation != nullptr && someModelData->myAnimation->myRootBone != -1
			&& someModelData->myAnimation->myBones.size() > 0)
		{
			FillBoneAnimationData(aFBXPath, someModelData, tempModel);
		}

		for (int i = 0; i < someModelData->myChildren.Size(); ++i)
		{
			auto currentChild = someModelData->myChildren[i];
			tempModel->AddChild(CreateModelAnimated(aFBXPath, currentChild));
		}
		return tempModel;
	}

	Model* FBXFactory::LoadModel(const std::string& aFilePath)
	{
#ifndef DLL_EXPORT
		if (myModels.find(aFilePath) != myModels.end())
		{
			return myModels[aFilePath];
		}
#endif
		//CU::TimerManager::GetInstance()->StartTimer("LoadModel");
		FBXData* found = 0;
		for (FBXData* data : myFBXData)
		{
			if (data->myPath.compare(aFilePath) == 0)
			{
				found = data;
				break;
			}
		}

		FbxModelData* modelData = nullptr;
#ifndef DLL_EXPORT
		if (found)
		{
			modelData = found->myData;
		}
		else
		{
			CU::GrowingArray<std::string> errors(16);
			FBXData* data = new FBXData();
			FbxModelData* fbxModelData = myLoader->loadModel(aFilePath.c_str(), errors);
			data->myData = fbxModelData;
			data->myPath = aFilePath;
			myFBXData.push_back(data);
			modelData = data->myData;
		}
#else
		CU::GrowingArray<std::string> errors(16);
		FBXData* data = new FBXData();
		FbxModelData* fbxModelData = myLoader->loadModel(aFilePath.c_str(),	errors);
		data->myData = fbxModelData;
		data->myPath = aFilePath;
		myFBXData.push_back(data);
		modelData = data->myData;
#endif


		Model* returnModel = CreateModel(modelData);
		myModels[aFilePath] = returnModel;

		//int elapsed = static_cast<int>(
		//	CU::TimerManager::GetInstance()->StopTimer("LoadModel").GetMilliseconds());

		//if (elapsed > 700)
		//{
		//	RESOURCE_LOG("FBX-Model \"%s\" took %d ms to load!!!", aFilePath, elapsed);
		//}
		//else
		//{
		//	RESOURCE_LOG("FBX-Model \"%s\" took %d ms to load", aFilePath, elapsed);
		//}

		return returnModel;
	}

	ModelAnimated* FBXFactory::LoadAnimatedModel(const std::string& aFilePath)
	{
#ifndef DLL_EXPORT
		if (myModelsAnimated.find(aFilePath) != myModelsAnimated.end())
		{
			return myModelsAnimated[aFilePath];
		}
#endif

		//CU::TimerManager::GetInstance()->StartTimer("LoadModelAnimated");
		FBXData* found = 0;
		for (FBXData* data : myFBXData)
		{
			if (data->myPath.compare(aFilePath) == 0)
			{
				found = data;
				break;
			}
		}

		FbxModelData* modelData = nullptr;

#ifndef DLL_EXPORT
		if (found)
		{
			modelData = found->myData;
		}
		else
		{
			CU::GrowingArray<std::string> errors(16);
			FBXData* data = new FBXData();
			FbxModelData* fbxModelData = myLoader->loadModel(aFilePath.c_str(), errors);
			data->myData = fbxModelData;
			data->myPath = aFilePath;
			myFBXData.push_back(data);
			modelData = data->myData;
		}
#else
		CU::GrowingArray<std::string> errors(16);
		FBXData* data = new FBXData();
		FbxModelData* fbxModelData = myLoader->loadModel(aFilePath.c_str(), errors);
		data->myData = fbxModelData;
		data->myPath = aFilePath;
		myFBXData.push_back(data);
		modelData = data->myData;
#endif

		ModelAnimated* returnModel = CreateModelAnimated(aFilePath, modelData);
		myModelsAnimated[aFilePath] = returnModel;

		//int elapsed = static_cast<int>(
		//	CU::TimerManager::GetInstance()->StopTimer("LoadModelAnimated").GetMilliseconds());

		//if (elapsed > 700)
		//{
		//	RESOURCE_LOG("Animated FBX-Model \"%s\" took %d ms to load!!!", aFilePath, elapsed);
		//}
		//else
		//{
		//	RESOURCE_LOG("Animated FBX-Model \"%s\" took %d ms to load", aFilePath, elapsed);
		//}

		return returnModel;
	}

	Animation* FBXFactory::LoadAnimation(const std::string& aFilePath)
	{
		if (myAnimations.find(aFilePath) != myAnimations.end())
		{
			return myAnimations[aFilePath];
		}

		//CU::TimerManager::GetInstance()->StartTimer("LoadAnimationFBX");
		FBXData* found = 0;
		for (FBXData* data : myFBXData)
		{
			if (data->myPath.compare(aFilePath) == 0)
			{
				found = data;
				break;
			}
		}

		FbxModelData* modelData = nullptr;

		if (found)
		{
			modelData = found->myData;
		}
		else
		{
			CU::GrowingArray<std::string> errors(16);
			FBXData* data = new FBXData();
			FbxModelData* fbxModelData = myLoader->loadModel(aFilePath.c_str(), errors);
			data->myData = fbxModelData;
			data->myPath = aFilePath;
			myFBXData.push_back(data);
			modelData = data->myData;
		}

		Animation* animation = nullptr;
		if (modelData->myAnimation != nullptr && modelData->myAnimation->myRootBone != -1
			&& modelData->myAnimation->myBones.size() > 0)
		{
			animation = FillBoneAnimationData(aFilePath, modelData, nullptr);
		}

		DL_ASSERT_EXP(animation != nullptr, "Failed to load animation, please tell Niklas or Daniel");

		//int elapsed = static_cast<int>(
		//	CU::TimerManager::GetInstance()->StopTimer("LoadAnimationFBX").GetMilliseconds());
		//RESOURCE_LOG("FBX-Animation \"%s\" took %d ms to load", aFilePath, elapsed);
		return animation;
	}

	void FBXFactory::LoadModelForRadiusCalc(const char* aFilePath
		, CU::GrowingArray<CU::Vector3<float>>& someVerticesOut)
	{
		CU::GrowingArray<std::string> errors(16);
		FbxModelData* fbxModelData = myLoader->loadModel(aFilePath, errors);

		CreateModelForRadiusCalc(fbxModelData, someVerticesOut);
		delete fbxModelData;
	}

	void FBXFactory::ConvertToDGFX(const char* aInputPath, const char* aOutputPath)
	{
		CU::GrowingArray<std::string> errors(16);

		ConvertToDGFX(aInputPath, aOutputPath, errors);
	}

	void FBXFactory::ConvertToDGFX(const char* aInputPath, const char* aOutputPath, CU::GrowingArray<std::string>& someOutErrors)
	{
		//CU::TimerManager::GetInstance()->StartTimer("ConvertDGFX");

		FBXData* data = new FBXData();
		FbxModelData* fbxModelData = myLoader->loadModel(aInputPath, someOutErrors);
		data->myData = fbxModelData;
		data->myPath = aInputPath;

		CU::BuildFoldersInPath(aOutputPath);
		std::fstream file;
		file.open(aOutputPath, std::ios::out | std::ios::binary);

		SaveHeaderToFile(fbxModelData, file);
		SaveModelToFile(fbxModelData, file);
		file.close();

		//int elapsed = static_cast<int>(
		//	CU::TimerManager::GetInstance()->StopTimer("ConvertDGFX").GetMilliseconds());
		//RESOURCE_LOG("Converting FBX->DGFX \"%s\" took %d ms", aOutputPath, elapsed);

		delete data;
	}


	void FBXFactory::CreateModelForRadiusCalc(FbxModelData* someModelData, CU::GrowingArray<CU::Vector3<float>>& someVerticesOut
		, const CU::Matrix44<float>& aParentOrientation)
	{
		CU::Matrix44<float> orientation = someModelData->myOrientation * aParentOrientation;
		orientation.NormalizeRotationVectors();
		if (someModelData->myData)
		{
			FillDataForRadiusCalc(someModelData->myData, someVerticesOut, orientation);
		}
		for (int i = 0; i < someModelData->myChildren.Size(); ++i)
		{
			auto currentChild = someModelData->myChildren[i];
			CreateModelForRadiusCalc(currentChild, someVerticesOut, orientation);
		}
	}

	void FBXFactory::FillDataForRadiusCalc(ModelData* aModelData, CU::GrowingArray<CU::Vector3<float>>& someVerticesOut
		, const CU::Matrix44<float>& aOrientation)
	{
		int sizeOfBuffer = aModelData->myVertexCount*aModelData->myVertexStride*sizeof(float);
		char* vertexRawData = new char[sizeOfBuffer];
		memcpy(vertexRawData, aModelData->myVertexBuffer, sizeOfBuffer);

		for (int i = 0; i < sizeOfBuffer - 3 * 4; i += aModelData->myVertexStride*sizeof(float))
		{
			CU::Vector3<float> position;
			memcpy(&position, vertexRawData + i, 3 * 4);
			position = position * aOrientation;
			someVerticesOut.Add(position);
		}
		delete vertexRawData;
	}

	void FBXFactory::SaveHeaderToFile(FbxModelData* aModelData, std::fstream& aStream)
	{
		int version = DGFX_VERSION;
		aStream.write((char*)&version, sizeof(int)); //DGFX-Version

		aStream.write((char*)&aModelData->myRadius, sizeof(float));
	}

	void FBXFactory::SaveModelToFile(FbxModelData* aModelData, std::fstream& aStream)
	{
		int isNullObject = 1;
		if (aModelData->myData)
		{
			isNullObject = 0;
		}
		aStream.write((char*)&isNullObject, sizeof(int)); //isNullObject


		int isLodGroup = 0;
		if (aModelData->myLodGroup)
		{
			isLodGroup = 1;
		}
		aStream.write((char*)&isLodGroup, sizeof(int)); //isLodGroup

		int isAnimated = 0;
		if (aModelData->myAnimation != nullptr && aModelData->myAnimation->myRootBone != -1
			&& aModelData->myAnimation->myBones.size() > 0)
		{
			isAnimated = 1;
		}
		aStream.write((char*)&isAnimated, sizeof(int)); //isAnimated


		if (isNullObject == 0)
		{
			SaveModelDataToFile(aModelData->myData, aStream);
			aStream.write((char*)&aModelData->myOrientation.myMatrix[0], sizeof(float) * 16);
		}

		if (isLodGroup == 1)
		{
			SaveLodGroupToFile(aModelData->myLodGroup, aStream);
		}

		if (isAnimated == 1)
		{
			SaveAnimationToFile(aModelData, aStream);
		}

		int childCount = aModelData->myChildren.Size();
		aStream.write((char*)&childCount, sizeof(int)); //childCount
		for (int i = 0; i < aModelData->myChildren.Size(); ++i)
		{
			auto currentChild = aModelData->myChildren[i];
			SaveModelToFile(currentChild, aStream);
		}
	}

	void FBXFactory::SaveModelDataToFile(ModelData* aData, std::fstream& aStream)
	{
#ifdef DGFX_REMOVE_EXTRA_VERTICES
		RemoveExtraVertices(aData);
#endif

		aStream.write((char*)(&aData->myIndexCount), sizeof(int)); //Index count
		aStream.write((char*)(aData->myIndicies), sizeof(int) * aData->myIndexCount); //All index data


		aStream.write((char*)(&aData->myVertexCount), sizeof(int)); //Vertex count
		aStream.write((char*)(&aData->myVertexStride), sizeof(int)); //Stride size
		aStream.write((char*)(aData->myVertexBuffer), sizeof(float) * aData->myVertexCount * aData->myVertexStride); //All vertex data


		int layoutCount = aData->myLayout.Size();
		aStream.write((char*)(&layoutCount), sizeof(int)); //Inputlayout element count

		for (int i = 0; i < aData->myLayout.Size(); ++i)
		{
			auto currentLayout = aData->myLayout[i];

			aStream.write((char*)&currentLayout.myOffset, sizeof(int)); //desc->AlignedByteOffset
			aStream.write((char*)&currentLayout.mySemanticIndex, sizeof(int)); //desc->mySemanticIndex
			aStream.write((char*)&currentLayout.myType, sizeof(int)); // ModelData::Layout::LayoutType (as an INT)
		}

		int textureCount = aData->myTextures.size();
		aStream.write((char*)&textureCount, sizeof(int)); //numberOfTextures

		for (unsigned int i = 0; i < aData->myTextures.size(); ++i)
		{
			auto& currentTexture = aData->myTextures[i];

			aStream.write((char*)&currentTexture.myType, sizeof(int)); //textureType

			int textureLenght = currentTexture.myFileName.length();
			aStream.write((char*)&textureLenght, sizeof(int)); //currentTexture.myFileName lenght
			const char* texture = currentTexture.myFileName.c_str();
			aStream.write(texture, sizeof(char) * textureLenght); //currentTexture.myFileName
		}
	}

	void FBXFactory::SaveLodGroupToFile(LodGroup* aGroup, std::fstream& aStream)
	{
		const int lodCount = aGroup->myLods.Size();
		Prism::Lod* lods = new Prism::Lod[lodCount];

		for (int i = 0; i < lodCount; ++i)
		{
			lods[i].myLevel = aGroup->myLods[i].myLevel;
			lods[i].myUseLod = aGroup->myLods[i].myUseLod;
		}

		aStream.write((char*)&lodCount, sizeof(int)); //Number of lods
		aStream.write((char*)lods, sizeof(Prism::Lod) * lodCount);


		const int threshHoldCount = aGroup->myThreshHolds.Size();
		aStream.write((char*)&threshHoldCount, sizeof(int));
		aStream.write((char*)&aGroup->myThreshHolds[0], sizeof(double) * threshHoldCount);

		delete[] lods;
	}

	void FBXFactory::SaveAnimationToFile(FbxModelData* aModelData, std::fstream& aStream)
	{
		auto loadedAnimation = aModelData->myAnimation;

		aStream.write((char*)&loadedAnimation->myBindMatrix.myMatrix[0], sizeof(float) * 16);

		SaveBoneHierarchyToFile(loadedAnimation->myBones[loadedAnimation->myRootBone], loadedAnimation, aStream);


		int nrOfbones = static_cast<int>(aModelData->myAnimation->myBones.size());
		aStream.write((char*)&nrOfbones, sizeof(int));
		for (int i = 0; i < nrOfbones; ++i)
		{
			Bone& currentBone = aModelData->myAnimation->myBones[i];

			int boneNameLenght = currentBone.myName.length();
			aStream.write((char*)&boneNameLenght, sizeof(int));
			const char* boneName = currentBone.myName.c_str();
			aStream.write(boneName, sizeof(char) * boneNameLenght);


			aStream.write((char*)&currentBone.myBaseOrientation.myMatrix[0], sizeof(float) * 16);
			aStream.write((char*)&currentBone.myBindMatrix.myMatrix[0], sizeof(float) * 16);



			int nrOfFrames = currentBone.myFrames.size();
			aStream.write((char*)&nrOfFrames, sizeof(int));

			AnimationNodeValue* nodeValues = new AnimationNodeValue[nrOfFrames];
			for (int j = 0; j < nrOfFrames; ++j)
			{
				auto currentFrame = currentBone.myFrames[j];


				nodeValues[j].myTime = currentFrame.myTime;
				nodeValues[j].myMatrix = currentFrame.myMatrix;
			}

			aStream.write((char*)nodeValues, sizeof(AnimationNodeValue) * nrOfFrames);
			delete[] nodeValues;
		}


		float animationLenght = aModelData->myAnimation->myBones[0].myAnimationTime;
		aStream.write((char*)&animationLenght, sizeof(float));
	}

	void FBXFactory::SaveBoneHierarchyToFile(Bone& aBone, AnimationData* aAnimationData, std::fstream& aStream)
	{
		aStream.write((char*)&aBone.myId, sizeof(int)); //BoneID

		int boneNameLenght = aBone.myName.length();
		aStream.write((char*)&boneNameLenght, sizeof(int)); //BoneNameLenght
		const char* boneName = aBone.myName.c_str();
		aStream.write(boneName, sizeof(char) * boneNameLenght); //BoneName


		const int nrOfChildren = static_cast<int>(aBone.myChilds.size());
		aStream.write((char*)&nrOfChildren, sizeof(int)); //nrOfChildren

		if (nrOfChildren > 0)
		{
			for (int i = 0; i < nrOfChildren; ++i)
			{
				SaveBoneHierarchyToFile(aAnimationData->myBones[aBone.myChilds[i]], aAnimationData, aStream);
			}
		}
	}

	void FBXFactory::RemoveExtraVertices(ModelData* aData)
	{
		int indexCount = aData->myIndexCount;
		unsigned int* indices = aData->myIndicies;

		int vertexCount = aData->myVertexCount;
		int vertexStride = aData->myVertexStride;
		float* vertices = aData->myVertexBuffer;
		int newVertexCount = 0;
		float* newVertices = new float[vertexCount*vertexStride];

		int* indexTranslater = new int[indexCount];
		unsigned int* newIndices = new unsigned int[indexCount];

		for (int i = 0; i < indexCount; ++i)
		{
			bool doCopy = true;
			int index = indices[i];
			float* vertex = vertices + (index * vertexStride);

			for (int j = 0; j < newVertexCount; ++j)
			{
				float* newVertex = newVertices + (j * vertexStride);
				if (memcmp(newVertex, vertex, vertexStride * sizeof(float)) == 0)
				{
					indexTranslater[i] = j;
					doCopy = false;
					break;
				}
			}

			if (doCopy == true)
			{
				indexTranslater[i] = newVertexCount;

				float* newDest = newVertices + (newVertexCount * vertexStride);
				memcpy(newDest, vertex, vertexStride * sizeof(float));
				++newVertexCount;
			}
		}


		for (int i = 0; i < indexCount; ++i)
		{
			int oldIndex = indices[i];
			int translatedIndex = indexTranslater[oldIndex];

			newIndices[i] = translatedIndex;
		}

		delete[] indices;
		delete[] vertices;
		delete[] indexTranslater;

		aData->myIndicies = newIndices;

		aData->myVertexCount = newVertexCount;
		aData->myVertexBuffer = newVertices;
	}
}