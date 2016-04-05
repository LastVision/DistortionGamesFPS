#include "../stdafx.h"


#include "FbxLoader.h"
#include <Vector4.h>
#include <Matrix44.h>
#include <DL_Assert.h>
#include <CommonHelper.h>
#include <XMLReader.h>
#pragma warning(disable : 4239 4244)

bool CheckFileExists(const std::string& aFile)
{
	std::ifstream stream(aFile);
	bool found = false;
	if (stream.good())
	{
		found = true;
	}
	stream.close();

	return found;
}

FBXLoader::FBXLoader()
{
	myFbxManager = FbxManager::Create();

	// Create the IO settings object.
	FbxIOSettings *ios = FbxIOSettings::Create(myFbxManager, IOSROOT);
	myFbxManager->SetIOSettings(ios);
}

FBXLoader::~FBXLoader()
{
	// Destroy the SDK manager and all the other objects it was handling.
	myFbxManager->Destroy();
}

FbxScene* FBXLoader::LoadScene(const char* aFile)
{
	// Create an importer using the SDK manager.
	FbxImporter* lImporter = FbxImporter::Create(myFbxManager, "");

	// Use the first argument as the filename for the importer.
	if (!lImporter->Initialize(aFile, -1, myFbxManager->GetIOSettings()))
	{
		printf("Call to FbxImporter::Initialize() failed.\n");
		printf("Error returned: %s\n\n", lImporter->GetStatus().GetErrorString());
		std::string errorMessage = "Could not find fbx file: " + std::string(aFile);
		DL_PRINT(errorMessage.c_str());
		MessageBoxA(nullptr, errorMessage.c_str(), "ERROR", 0);
		//DL_ASSERT("Fbx file not found. Check the debug logger!");
	}

	// Create a new scene so that it can be populated by the imported file.
	FbxScene* lScene = FbxScene::Create(myFbxManager, "myScene");

	// Import the contents of the file into the scene.
	lImporter->Import(lScene);

	// The file is imported; so get rid of the importer.a
	lImporter->Destroy();

	FbxGeometryConverter lGeomConverter(myFbxManager);

	lGeomConverter.Triangulate(lScene, /*replace*/true);

	//FbxMesh mesh;
	//mesh.GenerateTangentsData(
	//lGeomConverter.Comp
	//lGeomConverter.tr
	// Split meshes per material, so that we only have one material per mesh (for VBO support)
	lGeomConverter.SplitMeshesPerMaterial(lScene, /*replace*/true);
	return lScene;
}

// Get the geometry offset to a node. It is never inherited by the children.
FbxAMatrix GetGeometry(FbxNode* pNode)
{
	const FbxVector4 lT = pNode->GetGeometricTranslation(FbxNode::eSourcePivot);// (FbxNode::eSourcePivot);
	const FbxVector4 lR = pNode->GetGeometricRotation(FbxNode::eSourcePivot);
	const FbxVector4 lS = pNode->GetGeometricScaling(FbxNode::eSourcePivot);

	return FbxAMatrix(lT, lR, lS);
}

// Get the geometry offset to a node. It is never inherited by the children.
FbxAMatrix GetRotaionPivot(FbxNode* pNode)
{
	const FbxVector4 lT = pNode->GetRotationPivot(FbxNode::eSourcePivot);// (FbxNode::eSourcePivot);
	const FbxVector4 lR(0, 0, 0, 1);
	const FbxVector4 lS(1, 1, 1, 1);

	return FbxAMatrix(lT, lR, lS);
}

#define TRIANGLE_VERTEX_COUNT 3
#define VERTEX_STRIDE 4
#define NORMAL_STRIDE 4
#define BINORMAL_STRIDE 4
#define TANGENT_STRIDE 4
#define SKINWEIGHT_STRIDE 4
#define BONEID_STRIDE 4
#define UV_STRIDE 2
#define COLOR_STRIDE 4

FbxDouble3 GetMaterialProperty(const FbxSurfaceMaterial * pMaterial,
	const char * pPropertyName,
	const char * pFactorPropertyName,
	std::string & pTextureName)
{
	FbxDouble3 lResult(0, 0, 0);
	const FbxProperty lProperty = pMaterial->FindProperty(pPropertyName);
	const FbxProperty lFactorProperty = pMaterial->FindProperty(pFactorPropertyName);
	if (lProperty.IsValid() && lFactorProperty.IsValid())
	{
		lResult = lProperty.Get<FbxDouble3>();
		double lFactor = lFactorProperty.Get<FbxDouble>();
		if (lFactor != 1)
		{
			lResult[0] *= lFactor;
			lResult[1] *= lFactor;
			lResult[2] *= lFactor;
		}
	}

	if (lProperty.IsValid())
	{
		const int lTextureCount = lProperty.GetSrcObjectCount<FbxFileTexture>();
		if (lTextureCount)
		{
			const FbxFileTexture* lTexture = lProperty.GetSrcObject<FbxFileTexture>();
			if (lTexture)
			{
				pTextureName = lTexture->GetFileName();
			}
		}
	}

	return lResult;
}

// Get the matrix of the given pose
FbxAMatrix GetPoseMatrix(FbxPose* pPose, int pNodeIndex)
{
	FbxAMatrix lPoseMatrix;
	FbxMatrix lMatrix = pPose->GetMatrix(pNodeIndex);

	memcpy((double*)lPoseMatrix, (double*)lMatrix, sizeof(lMatrix.mData));

	return lPoseMatrix;
}

// Get the global position of the node for the current pose.
// If the specified node is not part of the pose or no pose is specified, get its
// global position at the current time.
FbxAMatrix GetGlobalPosition(FbxNode* pNode, const FbxTime& pTime, FbxPose* pPose, FbxAMatrix* pParentGlobalPosition)
{
	FbxAMatrix lGlobalPosition;
	bool        lPositionFound = false;

	if (pPose)
	{
		int lNodeIndex = pPose->Find(pNode);

		if (lNodeIndex > -1)
		{
			// The bind pose is always a global matrix.
			// If we have a rest pose, we need to check if it is
			// stored in global or local space.
			if (pPose->IsBindPose() || !pPose->IsLocalMatrix(lNodeIndex))
			{
				lGlobalPosition = GetPoseMatrix(pPose, lNodeIndex);
			}
			else
			{
				// We have a local matrix, we need to convert it to
				// a global space matrix.
				FbxAMatrix lParentGlobalPosition;

				if (pParentGlobalPosition)
				{
					lParentGlobalPosition = *pParentGlobalPosition;
				}
				else
				{
					if (pNode->GetParent())
					{
						//lParentGlobalPosition = GetGlobalPosition(pNode->GetParent(), pTime, pPose);
					}
				}

				FbxAMatrix lLocalPosition = GetPoseMatrix(pPose, lNodeIndex);
				lGlobalPosition = lParentGlobalPosition * lLocalPosition;
			}

			lPositionFound = true;
		}
	}

	if (!lPositionFound)
	{
		// There is no pose entry for that node, get the current global position instead.

		// Ideally this would use parent global position and local position to compute the global position.
		// Unfortunately the equation 
		//    lGlobalPosition = pParentGlobalPosition * lLocalPosition
		// does not hold when inheritance type is other than "Parent" (RSrs).
		// To compute the parent rotation and scaling is tricky in the RrSs and Rrs cases.
		lGlobalPosition = pNode->EvaluateGlobalTransform(pTime);
	}

	return lGlobalPosition;
}


//Compute the transform matrix that the cluster will transform the vertex.
void ComputeClusterDeformation(FbxAMatrix& pGlobalPosition,
	FbxMesh* pMesh,
	FbxCluster* pCluster,
	FbxAMatrix& pVertexTransformMatrix,
	FbxTime pTime,
	FbxPose* pPose)
{
	pPose;
	pTime;
	FbxCluster::ELinkMode lClusterMode = pCluster->GetLinkMode();

	FbxAMatrix lReferenceGlobalInitPosition;
	FbxAMatrix lReferenceGlobalCurrentPosition;
	FbxAMatrix lAssociateGlobalInitPosition;
	FbxAMatrix lAssociateGlobalCurrentPosition;
	FbxAMatrix lClusterGlobalInitPosition;
	FbxAMatrix lClusterGlobalCurrentPosition;

	FbxAMatrix lReferenceGeometry;
	FbxAMatrix lAssociateGeometry;
	FbxAMatrix lClusterGeometry;

	FbxAMatrix lClusterRelativeInitPosition;
	FbxAMatrix lClusterRelativeCurrentPositionInverse;

	if (lClusterMode == FbxCluster::eAdditive && pCluster->GetAssociateModel())
	{
		pCluster->GetTransformAssociateModelMatrix(lAssociateGlobalInitPosition);
		// Geometric transform of the model
		lAssociateGeometry = GetGeometry(pCluster->GetAssociateModel());
		lAssociateGlobalInitPosition *= lAssociateGeometry;
		//lAssociateGlobalCurrentPosition = GetGlobalPosition(pCluster->GetAssociateModel(), pTime, pPose);

		pCluster->GetTransformMatrix(lReferenceGlobalInitPosition);
		// Multiply lReferenceGlobalInitPosition by Geometric Transformation
		lReferenceGeometry = GetGeometry(pMesh->GetNode());
		lReferenceGlobalInitPosition *= lReferenceGeometry;
		lReferenceGlobalCurrentPosition = pGlobalPosition;

		// Get the link initial global position and the link current global position.
		pCluster->GetTransformLinkMatrix(lClusterGlobalInitPosition);
		// Multiply lClusterGlobalInitPosition by Geometric Transformation
		lClusterGeometry = GetGeometry(pCluster->GetLink());
		lClusterGlobalInitPosition *= lClusterGeometry;
		//lClusterGlobalCurrentPosition = GetGlobalPosition(pCluster->GetLink(), pTime, pPose);

		// Compute the shift of the link relative to the reference.
		//ModelM-1 * AssoM * AssoGX-1 * LinkGX * LinkM-1*ModelM
		pVertexTransformMatrix = lReferenceGlobalInitPosition.Inverse() * lAssociateGlobalInitPosition * lAssociateGlobalCurrentPosition.Inverse() *
			lClusterGlobalCurrentPosition * lClusterGlobalInitPosition.Inverse() * lReferenceGlobalInitPosition;
	}
	else
	{
		pCluster->GetTransformMatrix(lReferenceGlobalInitPosition);
		lReferenceGlobalCurrentPosition = pGlobalPosition;
		// Multiply lReferenceGlobalInitPosition by Geometric Transformation
		lReferenceGeometry = GetGeometry(pMesh->GetNode());
		lReferenceGlobalInitPosition *= lReferenceGeometry;

		// Get the link initial global position and the link current global position.
		pCluster->GetTransformLinkMatrix(lClusterGlobalInitPosition);
		//lClusterGlobalCurrentPosition = GetGlobalPosition(pCluster->GetLink(), pTime, pPose);

		// Compute the initial position of the link relative to the reference.
		lClusterRelativeInitPosition = lClusterGlobalInitPosition.Inverse() * lReferenceGlobalInitPosition;

		// Compute the current position of the link relative to the reference.
		lClusterRelativeCurrentPositionInverse = lReferenceGlobalCurrentPosition.Inverse() * lClusterGlobalCurrentPosition;

		// Compute the shift of the link relative to the reference.
		pVertexTransformMatrix = lClusterRelativeCurrentPositionInverse * lClusterRelativeInitPosition;
	}
}


// Deform the vertex array in classic linear way.
void ComputeLinearDeformation(FbxAMatrix& pGlobalPosition,
	FbxMesh* pMesh,
	FbxVector4* someWeights,
	FbxVectorTemplate4<int>* someBones,
	AnimationData* aAnimation)
{
	aAnimation;
	pGlobalPosition;
	// All the links must have the same link mode.
	FbxCluster::ELinkMode lClusterMode = ((FbxSkin*)pMesh->GetDeformer(0, FbxDeformer::eSkin))->GetCluster(0)->GetLinkMode();
	lClusterMode;
	int lVertexCount = pMesh->GetControlPointsCount();
	for (int i = 0; i < lVertexCount; ++i)
	{
		someBones[i][0] = -1;
		someBones[i][1] = -1;
		someBones[i][2] = -1;
		someBones[i][3] = -1;

		someWeights[i][0] = 0.0;
		someWeights[i][1] = 0.0;
		someWeights[i][2] = 0.0;
		someWeights[i][3] = 0.0;
	}

	// For all skins and all clusters, accumulate their deformation and weight
	// on each vertices and store them in lClusterDeformation and lClusterWeight.
	int lSkinCount = pMesh->GetDeformerCount(FbxDeformer::eSkin);
	for (int lSkinIndex = 0; lSkinIndex < lSkinCount; ++lSkinIndex)
	{
		FbxSkin * lSkinDeformer = (FbxSkin *)pMesh->GetDeformer(lSkinIndex, FbxDeformer::eSkin);

		int lClusterCount = lSkinDeformer->GetClusterCount();
		for (int lClusterIndex = 0; lClusterIndex < lClusterCount; ++lClusterIndex)
		{
			FbxCluster* lCluster = lSkinDeformer->GetCluster(lClusterIndex);
			if (!lCluster->GetLink())
				continue;

			std::string boneName = lCluster->GetLink()->GetName();

			int boneId = (int)lCluster->GetLink()->GetUserDataPtr();
			FbxAMatrix lVertexTransformMatrix;

			int lVertexIndexCount = lCluster->GetControlPointIndicesCount();
			for (int k = 0; k < lVertexIndexCount; ++k)
			{
				int lIndex = lCluster->GetControlPointIndices()[k];

				// Sometimes, the mesh can have less points than at the time of the skinning
				// because a smooth operator was active when skinning but has been deactivated during export.
				if (lIndex >= lVertexCount)
					continue;

				double lWeight = lCluster->GetControlPointWeights()[k];

				if (lWeight == 0.0)
				{
					continue;
				}

				bool foundSpot = false;
				for (int spot = 0; spot < 4; ++spot)
				{
					if (someBones[lIndex][spot] == -1)
					{
						someBones[lIndex][spot] = boneId;
						someWeights[lIndex][spot] = lWeight;
						foundSpot = true;
						break;
					}
				}

				/*if (lClusterMode == FbxCluster::eAdditive)
				{

				// Set the link to 1.0 just to know this vertex is influenced by a link.
				lClusterWeight[lIndex] = 1.0;
				}
				else // lLinkMode == FbxCluster::eNormalize || lLinkMode == FbxCluster::eTotalOne
				{
				// Add to the sum of weights to either normalize or complete the vertex.
				lClusterWeight[lIndex] += lWeight;
				}*/
			}//For each vertex			
		}//lClusterCount
	}

	/*for(int i = 0;i < lVertexCount;++i)
	{
	if(someBones[i][0] == -1)
	{
	someBones[i][0] = 0;
	}
	if(someBones[i][1] == -1)
	{
	someBones[i][1] = 0;
	}
	if(someBones[i][2] == -1)
	{
	someBones[i][2] = 0;
	}
	if(someBones[i][3] == -1)
	{
	someBones[i][3] = 0;
	}

	}*/

	//Actually deform each vertices here by information stored in lClusterDeformation and lClusterWeight
	/*for (int i = 0; i < lVertexCount; i++)
	{
	FbxVector4 lSrcVertex = pVertexArray[i];
	FbxVector4& lDstVertex = pVertexArray[i];
	double lWeight = lClusterWeight[i];

	// Deform the vertex if there was at least a link with an influence on the vertex,
	if (lWeight != 0.0)
	{
	lDstVertex = lClusterDeformation[i].MultT(lSrcVertex);
	if (lClusterMode == FbxCluster::eNormalize)
	{
	// In the normalized link mode, a vertex is always totally influenced by the links.
	lDstVertex /= lWeight;
	}
	else if (lClusterMode == FbxCluster::eTotalOne)
	{
	// In the total 1 link mode, a vertex can be partially influenced by the links.
	lSrcVertex *= (1.0 - lWeight);
	lDstVertex += lSrcVertex;
	}
	}
	}

	delete [] lClusterDeformation;
	delete [] lClusterWeight;*/
}


bool FillData(const char* aFilePath, ModelData* someData, FbxNode* aNode, AnimationData* aAnimation, CU::GrowingArray<std::string>& someOutErrors)
{
	FbxMesh* mesh = aNode->GetMesh();
	if (mesh == nullptr || !aNode)
		return false;

	const int lPolygonCount = mesh->GetPolygonCount();

	// Count the polygon count of each material
	FbxLayerElementArrayTemplate<int>* lMaterialIndice = NULL;
	FbxGeometryElement::EMappingMode lMaterialMappingMode = FbxGeometryElement::eNone;
	if (mesh->GetElementMaterial())
	{
		lMaterialIndice = &mesh->GetElementMaterial()->GetIndexArray();
		lMaterialMappingMode = mesh->GetElementMaterial()->GetMappingMode();
		if (lMaterialIndice && lMaterialMappingMode == FbxGeometryElement::eByPolygon)
		{
			FBX_ASSERT(lMaterialIndice->GetCount() == lPolygonCount);
			if (lMaterialIndice->GetCount() == lPolygonCount)
			{
				// Count the faces of each material
				for (int lPolygonIndex = 0; lPolygonIndex < lPolygonCount; ++lPolygonIndex)
				{
					const int lMaterialIndex = lMaterialIndice->GetAt(lPolygonIndex);
					lMaterialIndex;
					/*if (someData->mSubMeshes[lMaterialIndex] == NULL)
					{
					someData->mSubMeshes[lMaterialIndex] = new ModelData::SubMesh;
					}
					someData->mSubMeshes[lMaterialIndex]->TriangleCount += 1;*/
				}

				// Make sure we have no "holes" (NULL) in the mSubMeshes table. This can happen
				// if, in the loop above, we resized the mSubMeshes by more than one slot.

				/*for (int i = 0; i < someData->mSubMeshes.Count(); i++)
				{
				if (someData->mSubMeshes[i] == NULL)
				someData->mSubMeshes[i] = new ModelData::SubMesh;
				}*/

				// Record the offset (how many vertex)
				const int lMaterialCount = someData->mSubMeshes.Size();
				lMaterialCount;
				int lOffset = 0;
				lOffset;
				/*for (int lIndex = 0; lIndex < lMaterialCount; ++lIndex)
				{
				someData->mSubMeshes[lIndex]->IndexOffset = lOffset;
				lOffset += someData->mSubMeshes[lIndex]->TriangleCount * 3;
				// This will be used as counter in the following procedures, reset to zero
				someData->mSubMeshes[lIndex]->TriangleCount = 0;
				}*/
				FBX_ASSERT(lOffset == lPolygonCount * 3);
			}
		}
	}

	// All faces will use the same material.
	if (someData->mSubMeshes.Size() == 0)
	{
		if (someData->mSubMeshes.GetCapacity() == 0)
		{
			someData->mSubMeshes.Init(1);
		}
		//someData->mSubMeshes.RemoveAll();
		someData->mSubMeshes.DeleteAll();
		someData->mSubMeshes.Add(new ModelData::SubMesh());
	}



	bool hasNormalMap = false;

	const int lMaterialCount = aNode->GetMaterialCount();
	for (int lMaterialIndex = 0; lMaterialIndex < lMaterialCount; ++lMaterialIndex)
	{
		FbxSurfaceMaterial * lMaterial = aNode->GetMaterial(lMaterialIndex);
		if (lMaterial && !lMaterial->GetUserDataPtr())
		{
			TextureInfo albedoInfo;
			GetMaterialProperty(lMaterial, FbxSurfaceMaterial::sDiffuse, nullptr, albedoInfo.myFileName);
			albedoInfo.myType = FBXTextureType::ALBEDO;
			if (albedoInfo.myFileName.empty() == false)
			{
				if (CheckFileExists(albedoInfo.myFileName) == false)
				{
					someOutErrors.Add(CU::Concatenate("Failed to find texture: %s (FBX: %s)", albedoInfo.myFileName.c_str(), aFilePath));
				}

				someData->myTextures.push_back(albedoInfo);

				int index = albedoInfo.myFileName.find("_neutral.dds");
				if (index != std::string::npos)
				{
					std::string playerAlbedo(albedoInfo.myFileName.begin(), albedoInfo.myFileName.begin() + index);
					playerAlbedo += "_player.dds";

					std::string enemyAlbedo(albedoInfo.myFileName.begin(), albedoInfo.myFileName.begin() + index);
					enemyAlbedo += "_enemy.dds";

					if (CheckFileExists(playerAlbedo) == false)
					{
						someOutErrors.Add(CU::Concatenate("Failed to find texture: %s (FBX: %s)", playerAlbedo.c_str(), aFilePath));
					}
					if (CheckFileExists(enemyAlbedo) == false)
					{
						someOutErrors.Add(CU::Concatenate("Failed to find texture: %s (FBX: %s)", enemyAlbedo.c_str(), aFilePath));
					}
				}
			}

			TextureInfo normalInfo;
			GetMaterialProperty(lMaterial, FbxSurfaceMaterial::sNormalMap, nullptr, normalInfo.myFileName);
			hasNormalMap = false;
			normalInfo.myType = FBXTextureType::NORMAL;
			if (normalInfo.myFileName.empty() == false)
			{
				if (CheckFileExists(normalInfo.myFileName) == false)
				{
					someOutErrors.Add(CU::Concatenate("Failed to find texture: %s (FBX: %s)", normalInfo.myFileName.c_str(), aFilePath));
				}

				someData->myTextures.push_back(normalInfo);
				hasNormalMap = true;
			}

			TextureInfo roughnessInfo;
			GetMaterialProperty(lMaterial, FbxSurfaceMaterial::sSpecular, nullptr, roughnessInfo.myFileName);
			roughnessInfo.myType = FBXTextureType::ROUGHNESS;
			if (roughnessInfo.myFileName.empty() == false)
			{
				if (CheckFileExists(roughnessInfo.myFileName) == false)
				{
					someOutErrors.Add(CU::Concatenate("Failed to find texture: %s (FBX: %s)", roughnessInfo.myFileName.c_str(), aFilePath));
				}

				someData->myTextures.push_back(roughnessInfo);
			}

			TextureInfo metalnessInfo;
			GetMaterialProperty(lMaterial, FbxSurfaceMaterial::sReflection, nullptr, metalnessInfo.myFileName);
			metalnessInfo.myType = FBXTextureType::METALNESS;
			if (metalnessInfo.myFileName.empty() == false)
			{
				if (CheckFileExists(metalnessInfo.myFileName) == false)
				{
					someOutErrors.Add(CU::Concatenate("Failed to find texture: %s (FBX: %s)", metalnessInfo.myFileName.c_str(), aFilePath));
				}
			
				someData->myTextures.push_back(metalnessInfo);
			}

			TextureInfo ambientInfo;
			GetMaterialProperty(lMaterial, FbxSurfaceMaterial::sAmbient, nullptr, ambientInfo.myFileName);
			ambientInfo.myType = FBXTextureType::AMBIENT;
			if (ambientInfo.myFileName.empty() == false)
			{
				if (CheckFileExists(ambientInfo.myFileName) == false)
				{
					someOutErrors.Add(CU::Concatenate("Failed to find texture: %s (FBX: %s)", ambientInfo.myFileName.c_str(), aFilePath));
				}
				
				someData->myTextures.push_back(ambientInfo);
			}

			TextureInfo emissiveInfo;
			GetMaterialProperty(lMaterial, FbxSurfaceMaterial::sEmissive, nullptr, emissiveInfo.myFileName);
			emissiveInfo.myType = FBXTextureType::EMISSIVE;
			if (emissiveInfo.myFileName.empty() == false)
			{
				if (CheckFileExists(emissiveInfo.myFileName) == false)
				{
					someOutErrors.Add(CU::Concatenate("Failed to find texture: %s (FBX: %s)", emissiveInfo.myFileName.c_str(), aFilePath));
				}

				someData->myTextures.push_back(emissiveInfo);
			}
		}
	}

	// Congregate all the data of a mesh to be cached in VBOs.
	// If normal or UV is by polygon vertex, record all vertex attributes by polygon vertex.'

	someData->mHasNormal = mesh->GetElementNormalCount() > 0;
	someData->mHasUV = mesh->GetElementUVCount() > 0;
	someData->myHasBiNormal = mesh->GetElementBinormalCount() > 0;
	someData->myHasVertexColor = mesh->GetElementVertexColorCount() > 0;

	FbxSkin * lSkinDeformer = (FbxSkin *)mesh->GetDeformer(0, FbxDeformer::eSkin);
	someData->myHasSkinweights = lSkinDeformer != nullptr;

	if (hasNormalMap && someData->myHasBiNormal == false)
	{
		mesh->GenerateTangentsDataForAllUVSets();
		someData->myHasBiNormal = mesh->GetElementBinormalCount() > 0;
	}
	someData->myHasTangents = mesh->GetElementTangentCount() > 0;

	FbxGeometryElement::EMappingMode lNormalMappingMode = FbxGeometryElement::eNone;
	FbxGeometryElement::EMappingMode lUVMappingMode = FbxGeometryElement::eNone;
	if (someData->mHasNormal)
	{
		lNormalMappingMode = mesh->GetElementNormal(0)->GetMappingMode();
		if (lNormalMappingMode == FbxGeometryElement::eNone)
		{
			someData->mHasNormal = false;
		}
		if (someData->mHasNormal && lNormalMappingMode != FbxGeometryElement::eByControlPoint)
		{
			someData->mAllByControlPoint = false;
		}
	}
	if (someData->mHasUV)
	{
		lUVMappingMode = mesh->GetElementUV(0)->GetMappingMode();
		if (lUVMappingMode == FbxGeometryElement::eNone)
		{
			someData->mHasUV = false;
		}
		if (someData->mHasUV && lUVMappingMode != FbxGeometryElement::eByControlPoint)
		{
			someData->mAllByControlPoint = false;
		}
	}

	// Allocate the array memory, by control point or by polygon vertex.
	int lPolygonVertexCount = mesh->GetControlPointsCount();
	//if (!someData->my)
	{
		ModelData::Layout newLayout;
		newLayout.myType = ModelData::VERTEX_POS;
		newLayout.mySize = VERTEX_STRIDE;
		newLayout.myOffset = 0;
		newLayout.mySemanticIndex = 0;
		someData->myLayout.Add(newLayout);
		lPolygonVertexCount = lPolygonCount * TRIANGLE_VERTEX_COUNT;
	}
	int stride = VERTEX_STRIDE;
	size_t size = lPolygonVertexCount * VERTEX_STRIDE;
	//float * lVertices = new float[lPolygonVertexCount * VERTEX_STRIDE];
	unsigned int * lIndices = new unsigned int[lPolygonCount * TRIANGLE_VERTEX_COUNT];
	someData->myIndexCount = lPolygonCount * TRIANGLE_VERTEX_COUNT;
	//float * lNormals = NULL;
	if (someData->mHasNormal)
	{
		ModelData::Layout newLayout;
		newLayout.myType = ModelData::VERTEX_NORMAL;
		newLayout.mySize = NORMAL_STRIDE;
		newLayout.myOffset = stride * 4;
		newLayout.mySemanticIndex = 0;
		someData->myLayout.Add(newLayout);

		stride += NORMAL_STRIDE;
		size += lPolygonVertexCount * NORMAL_STRIDE;
		//lNormals = new float[lPolygonVertexCount * NORMAL_STRIDE];
	}
	//float * lUVs = NULL;
	FbxStringList lUVNames;
	mesh->GetUVSetNames(lUVNames);
	const char * lUVName = NULL;
	const char * lUV2Name = NULL;
	if (someData->mHasUV && lUVNames.GetCount())
	{
		ModelData::Layout newLayout;
		newLayout.myType = ModelData::VERTEX_UV;
		newLayout.mySize = UV_STRIDE;
		newLayout.myOffset = stride * 4;
		newLayout.mySemanticIndex = 0;
		someData->myLayout.Add(newLayout);

		stride += UV_STRIDE;
		size += lPolygonVertexCount * UV_STRIDE;
		//lUVs = new float[lPolygonVertexCount * UV_STRIDE];
		lUVName = lUVNames[0];
	}
	if (lUVNames.GetCount() > 1)
	{
		ModelData::Layout newLayout;
		newLayout.myType = ModelData::VERTEX_UV;
		newLayout.mySize = UV_STRIDE;
		newLayout.myOffset = stride * 4;
		newLayout.mySemanticIndex = 1;
		someData->myLayout.Add(newLayout);

		stride += UV_STRIDE;
		size += lPolygonVertexCount * UV_STRIDE;
		//lUVs = new float[lPolygonVertexCount * UV_STRIDE];
		lUV2Name = lUVNames[1];
	}

	if (someData->myHasBiNormal)
	{
		ModelData::Layout newLayout;
		newLayout.myType = ModelData::VERTEX_BINORMAL;
		newLayout.mySize = BINORMAL_STRIDE;
		newLayout.myOffset = stride * 4;
		newLayout.mySemanticIndex = 0;
		someData->myLayout.Add(newLayout);

		stride += BINORMAL_STRIDE;
		size += lPolygonVertexCount * BINORMAL_STRIDE;
		//lUVs = new float[lPolygonVertexCount * UV_STRIDE];
	}
				
	if (someData->myHasTangents)
	{
		ModelData::Layout newLayout;
		newLayout.myType = ModelData::VERTEX_TANGENT;
		newLayout.mySize = TANGENT_STRIDE;
		newLayout.myOffset = stride * 4;
		newLayout.mySemanticIndex = 0;
		someData->myLayout.Add(newLayout);

		stride += TANGENT_STRIDE;
		size += lPolygonVertexCount * TANGENT_STRIDE;
		//lUVs = new float[lPolygonVertexCount * UV_STRIDE];
	}

	if (someData->myHasSkinweights)
	{
		ModelData::Layout newLayout;
		newLayout.myType = ModelData::VERTEX_SKINWEIGHTS;
		newLayout.mySize = SKINWEIGHT_STRIDE;
		newLayout.myOffset = stride * 4;
		newLayout.mySemanticIndex = 0;
		someData->myLayout.Add(newLayout);

		stride += SKINWEIGHT_STRIDE;
		size += lPolygonVertexCount * SKINWEIGHT_STRIDE;

		newLayout.myType = ModelData::VERTEX_BONEID;
		newLayout.mySize = BONEID_STRIDE;
		newLayout.myOffset = stride * 4;
		newLayout.mySemanticIndex = 0;
		someData->myLayout.Add(newLayout);

		stride += BONEID_STRIDE;
		size += lPolygonVertexCount * BONEID_STRIDE;
		//lUVs = new float[lPolygonVertexCount * UV_STRIDE];
	}

	if (someData->myHasVertexColor)
	{
		ModelData::Layout newLayout;
		newLayout.myType = ModelData::VERTEX_COLOR;;
		newLayout.mySize = COLOR_STRIDE;
		newLayout.myOffset = stride * 4;
		newLayout.mySemanticIndex = 0;
		someData->myLayout.Add(newLayout);

		stride += COLOR_STRIDE;
		size += lPolygonVertexCount * COLOR_STRIDE;
	}

	float * lVertices = new float[size];

	FbxAMatrix globalPos;
	FbxVector4* weights = nullptr;
	FbxVectorTemplate4<int>* bones = nullptr;
	FbxTime time = static_cast<FbxLongLong>(0.0f);
	if (someData->myHasSkinweights)
	{
		weights = new FbxVector4[mesh->GetControlPointsCount()];
		bones = new FbxVectorTemplate4<int>[mesh->GetControlPointsCount()];
		ComputeLinearDeformation(globalPos, mesh, weights, bones, aAnimation);
	}

	const FbxGeometryElementBinormal * lBiNormalElement = NULL;
	const FbxGeometryElementTangent * lTangentElement = NULL;
	const FbxGeometryElementVertexColor * lColorElement = NULL;

	if (someData->myHasBiNormal)
	{
		lBiNormalElement = mesh->GetElementBinormal(0);
	}
	if (someData->myHasTangents)
	{
		lTangentElement = mesh->GetElementTangent(0);
	}
	if (someData->myHasVertexColor)
	{
		lColorElement = mesh->GetElementVertexColor(0);
	}

	// Populate the array with vertex attribute, if by control point.
	const FbxVector4 * lControlPoints = mesh->GetControlPoints();
	FbxVector4 lCurrentVertex;
	FbxVector4 lCurrentNormal;
	FbxVector4 lCurrentBiNormal;
	FbxVector4 lCurrentTangent;
	FbxVector2 lCurrentUV;
	FbxColor lCurrentColor;

#pragma region mappingISControlPoint
	if (someData->mAllByControlPoint)
	{
		const FbxGeometryElementNormal * lNormalElement = NULL;
		const FbxGeometryElementUV * lUVElement = NULL;
		if (someData->mHasNormal)
		{
			lNormalElement = mesh->GetElementNormal(0);
		}
		if (someData->mHasUV)
		{
			lUVElement = mesh->GetElementUV(0);
		}


		for (int lIndex = 0; lIndex < lPolygonVertexCount; ++lIndex)
		{
			int currentIndex = lIndex * stride;
			int addedSize = VERTEX_STRIDE;
			// Save the vertex position.
			lCurrentVertex = lControlPoints[lIndex];

			
			CU::Vector4<float> position(static_cast<float>(lCurrentVertex[0]),
				static_cast<float>(lCurrentVertex[1]),
				static_cast<float>(lCurrentVertex[2]),
				1);

			
			
			CU::Matrix44<float> fixMatrix;
			fixMatrix = CU::Matrix44<float>::CreateReflectionMatrixAboutAxis(CU::Vector3<float>(1, 0, 0));
			position = position*fixMatrix;

			lVertices[currentIndex] = position.x;
			lVertices[currentIndex + 1] = position.y;
			lVertices[currentIndex + 2] = position.z;
			lVertices[currentIndex + 3] = 1;


			// Save the normal.
			if (someData->mHasNormal)
			{
				int lNormalIndex = lIndex;
				if (lNormalElement->GetReferenceMode() == FbxLayerElement::eIndexToDirect)
				{
					lNormalIndex = lNormalElement->GetIndexArray().GetAt(lIndex);
				}
				lCurrentNormal = lNormalElement->GetDirectArray().GetAt(lNormalIndex);

				CU::Vector3<float> normal(static_cast<float>(lCurrentNormal[0]), static_cast<float>(lCurrentNormal[1]), static_cast<float>(lCurrentNormal[2]));
				normal = normal*CU::Matrix44<float>::CreateReflectionMatrixAboutAxis(CU::Vector3<float>(1, 0, 0));

				lVertices[currentIndex + addedSize] = normal.x;
				lVertices[currentIndex + addedSize + 1] = normal.y;
				lVertices[currentIndex + addedSize + 2] = normal.z;
				lVertices[currentIndex + addedSize + 3] = 0;
				addedSize += NORMAL_STRIDE;
			}

			// Save the UV.
			if (someData->mHasUV)
			{
				int lUVIndex = lIndex;
				if (lUVElement->GetReferenceMode() == FbxLayerElement::eIndexToDirect)
				{
					lUVIndex = lUVElement->GetIndexArray().GetAt(lIndex);
				}
				lCurrentUV = lUVElement->GetDirectArray().GetAt(lUVIndex);
				lVertices[currentIndex + addedSize] = static_cast<float>(lCurrentUV[0]);
				lVertices[currentIndex + addedSize + 1] = static_cast<float>(lCurrentUV[1])*-1.0f;
				addedSize += 2;
			}

			if (someData->myHasBiNormal)
			{
				int lBinormIndexIndex = lIndex;
				if (lBiNormalElement->GetReferenceMode() == FbxLayerElement::eIndexToDirect)
				{
					lBinormIndexIndex = lBiNormalElement->GetIndexArray().GetAt(lIndex);
				}

				lCurrentBiNormal = lBiNormalElement->GetDirectArray().GetAt(lBinormIndexIndex);
				//mesh->GetElementBinormal(lPolygonIndex, lVerticeIndex, lCurrentNormal);
				//lCurrentNormal = lCurrentNormal
				CU::Vector3<float> normal(static_cast<float>(lCurrentBiNormal[0]), static_cast<float>(lCurrentBiNormal[1]), static_cast<float>(lCurrentBiNormal[2]));
				normal = normal*CU::Matrix44<float>::CreateReflectionMatrixAboutAxis(CU::Vector3<float>(1, 0, 0));
				CU::Normalize(normal);
				lVertices[currentIndex + addedSize] = normal.x;
				lVertices[currentIndex + addedSize + 1] = normal.y;
				lVertices[currentIndex + addedSize + 2] = normal.z;
				lVertices[currentIndex + addedSize + 3] = 0;
				addedSize += BINORMAL_STRIDE;
			}

			if (someData->myHasTangents)
			{
				int lBinormIndexIndex = lIndex;
				if (lTangentElement->GetReferenceMode() == FbxLayerElement::eIndexToDirect)
				{
					lBinormIndexIndex = lTangentElement->GetIndexArray().GetAt(lIndex);
				}

				lCurrentTangent = lTangentElement->GetDirectArray().GetAt(lBinormIndexIndex);

				//lCurrentNormal = lCurrentNormal
				CU::Vector3<float> normal(static_cast<float>(lCurrentTangent[0]), static_cast<float>(lCurrentTangent[1]), static_cast<float>(lCurrentTangent[2]));
				normal = normal*CU::Matrix44<float>::CreateReflectionMatrixAboutAxis(CU::Vector3<float>(1, 0, 0));
				CU::Normalize(normal);
				lVertices[currentIndex + addedSize] = normal.x;
				lVertices[currentIndex + addedSize + 1] = normal.y;
				lVertices[currentIndex + addedSize + 2] = normal.z;
				lVertices[currentIndex + addedSize + 3] = 0;
				addedSize += TANGENT_STRIDE;
			}

			if (someData->myHasSkinweights)
			{
				FbxVector4 currentWeights = weights[lIndex];
				//currentWeights.Normalize();

				lVertices[currentIndex + addedSize] = currentWeights[0];
				lVertices[currentIndex + addedSize + 1] = currentWeights[1];
				lVertices[currentIndex + addedSize + 2] = currentWeights[2];
				lVertices[currentIndex + addedSize + 3] = currentWeights[3];
				addedSize += SKINWEIGHT_STRIDE;

				FbxVectorTemplate4<int> currentBones = bones[lIndex];

				lVertices[currentIndex + addedSize] = currentBones[0];
				lVertices[currentIndex + addedSize + 1] = currentBones[1];
				lVertices[currentIndex + addedSize + 2] = currentBones[2];
				lVertices[currentIndex + addedSize + 3] = currentBones[3];
				addedSize += BONEID_STRIDE;
			}
			
			if (someData->myHasVertexColor)
			{
				int lColorIndex = lIndex;
				if (lColorElement->GetReferenceMode() == FbxLayerElement::eIndexToDirect)
				{
					lColorIndex = lColorElement->GetIndexArray().GetAt(lIndex);
				}
				lCurrentColor = lColorElement->GetDirectArray().GetAt(lColorIndex);

				lVertices[currentIndex + addedSize] = lCurrentColor[0];
				lVertices[currentIndex + addedSize + 1] = lCurrentColor[1];
				lVertices[currentIndex + addedSize + 2] = lCurrentColor[2];
				lVertices[currentIndex + addedSize + 3] = lCurrentColor[3];
				addedSize += COLOR_STRIDE;
			}
		}

	}
#pragma endregion
	int lVertexCount = 0;
	for (int lPolygonIndex = 0; lPolygonIndex < lPolygonCount; ++lPolygonIndex)
	{
		// The material for current face.
		int lMaterialIndex = 0;
		if (lMaterialIndice && lMaterialMappingMode == FbxGeometryElement::eByPolygon)
		{
			lMaterialIndex = lMaterialIndice->GetAt(lPolygonIndex);
		}

		// Where should I save the vertex attribute index, according to the material
		const int lIndexOffset = someData->mSubMeshes[lMaterialIndex]->IndexOffset +
			someData->mSubMeshes[lMaterialIndex]->TriangleCount * 3;
		for (int lVerticeIndex = TRIANGLE_VERTEX_COUNT - 1; lVerticeIndex > -1; --lVerticeIndex)
		{
			const int lControlPointIndex = mesh->GetPolygonVertex(lPolygonIndex, lVerticeIndex);

			int vertexIndex = lIndexOffset + (TRIANGLE_VERTEX_COUNT - 1) - lVerticeIndex;
			if (someData->mAllByControlPoint)
			{
				lIndices[vertexIndex] = static_cast<unsigned int>(lControlPointIndex);
			}
			// Populate the array with vertex attribute, if by polygon vertex.
			else
			{
				lIndices[vertexIndex] = static_cast<unsigned int>(lVertexCount);

				lCurrentVertex = lControlPoints[lControlPointIndex];
				int addedSize = VERTEX_STRIDE;
				int currentIndex = lVertexCount * stride;

				CU::Vector4<float> position(static_cast<float>(lCurrentVertex[0]),
					static_cast<float>(lCurrentVertex[1]),
					static_cast<float>(lCurrentVertex[2]),
					1);

				static CU::Matrix44<float> fixMatrix(CU::Matrix44<float>::CreateReflectionMatrixAboutAxis(CU::Vector3<float>(1, 0, 0)));
				position = position*fixMatrix;

				lVertices[currentIndex] = position.x;
				lVertices[currentIndex + 1] = position.y;
				lVertices[currentIndex + 2] = position.z;
				lVertices[currentIndex + 3] = 0;

				if (someData->mHasNormal)
				{
					mesh->GetPolygonVertexNormal(lPolygonIndex, lVerticeIndex, lCurrentNormal);
					CU::Vector3<float> normal(static_cast<float>(lCurrentNormal[0]), static_cast<float>(lCurrentNormal[1]), static_cast<float>(lCurrentNormal[2]));
					normal = normal*CU::Matrix44<float>::CreateReflectionMatrixAboutAxis(CU::Vector3<float>(1, 0, 0));
					CU::Normalize(normal);
					lVertices[currentIndex + addedSize] = normal.x;
					lVertices[currentIndex + addedSize + 1] = normal.y;
					lVertices[currentIndex + addedSize + 2] = normal.z;
					lVertices[currentIndex + addedSize + 3] = 0;
					addedSize += NORMAL_STRIDE;
				}

				if (someData->mHasUV)
				{
					bool lUnmappedUV;
					mesh->GetPolygonVertexUV(lPolygonIndex, lVerticeIndex, lUVName, lCurrentUV, lUnmappedUV);
					lVertices[currentIndex + addedSize] = static_cast<float>(lCurrentUV[0]);
					lVertices[currentIndex + addedSize + 1] = static_cast<float>(lCurrentUV[1])*-1.0f;
					addedSize += UV_STRIDE;
				}
				if (lUV2Name != nullptr)
				{
					bool lUnmappedUV;
					mesh->GetPolygonVertexUV(lPolygonIndex, lVerticeIndex, lUV2Name, lCurrentUV, lUnmappedUV);
					lVertices[currentIndex + addedSize] = static_cast<float>(lCurrentUV[0]);
					lVertices[currentIndex + addedSize + 1] = static_cast<float>(lCurrentUV[1])*-1.0f;
					addedSize += UV_STRIDE;
				}

				if (someData->myHasBiNormal)
				{
					int lBinormIndexIndex = lVerticeIndex;
					if (lBiNormalElement->GetReferenceMode() == FbxLayerElement::eIndexToDirect)
					{
						lBinormIndexIndex = lBiNormalElement->GetIndexArray().GetAt(lVerticeIndex);
					}

					lCurrentBiNormal = lBiNormalElement->GetDirectArray().GetAt(lBinormIndexIndex);

					CU::Vector3<float> normal(static_cast<float>(lCurrentBiNormal[0]), static_cast<float>(lCurrentBiNormal[1]), static_cast<float>(lCurrentBiNormal[2]));
					normal = normal*fixMatrix;
					CU::Normalize(normal);
					lVertices[currentIndex + addedSize] = normal.x;
					lVertices[currentIndex + addedSize + 1] = normal.y;
					lVertices[currentIndex + addedSize + 2] = normal.z;
					lVertices[currentIndex + addedSize + 3] = 0;
					addedSize += BINORMAL_STRIDE;
				}

				if (someData->myHasTangents)
				{
					int lBinormIndexIndex = lVerticeIndex;
					if (lTangentElement->GetReferenceMode() == FbxLayerElement::eIndexToDirect)
					{
						lBinormIndexIndex = lTangentElement->GetIndexArray().GetAt(lVerticeIndex);
					}

					lCurrentTangent = lTangentElement->GetDirectArray().GetAt(lBinormIndexIndex);

					mesh->GetPolygonVertexNormal(lPolygonIndex, lVerticeIndex, lCurrentNormal);

					CU::Vector3<float> normal(static_cast<float>(lCurrentBiNormal[0]), static_cast<float>(lCurrentBiNormal[1]), static_cast<float>(lCurrentBiNormal[2]));
					normal = normal*fixMatrix;
					CU::Normalize(normal);
					lVertices[currentIndex + addedSize] = normal.x;
					lVertices[currentIndex + addedSize + 1] = normal.y;
					lVertices[currentIndex + addedSize + 2] = normal.z;
					lVertices[currentIndex + addedSize + 3] = 0;
					addedSize += TANGENT_STRIDE;
				}

				if (someData->myHasSkinweights)
				{
					FbxVector4 currentWeights = weights[lControlPointIndex];
					FbxVectorTemplate4<int> currentBones = bones[lControlPointIndex];
					for (int l = 0; l < 4; ++l)
					{
						if (currentBones[l] == -1)
						{
							currentWeights[l] = 0.0f;
						}
					}
					currentWeights.Normalize();

					lVertices[currentIndex + addedSize] = currentWeights[0];
					lVertices[currentIndex + addedSize + 1] = currentWeights[1];
					lVertices[currentIndex + addedSize + 2] = currentWeights[2];
					lVertices[currentIndex + addedSize + 3] = currentWeights[3];
					addedSize += SKINWEIGHT_STRIDE;



					lVertices[currentIndex + addedSize] = *(float*)&currentBones[0];
					lVertices[currentIndex + addedSize + 1] = *(float*)&currentBones[1];
					lVertices[currentIndex + addedSize + 2] = *(float*)&currentBones[2];
					lVertices[currentIndex + addedSize + 3] = *(float*)&currentBones[3];
					addedSize += BONEID_STRIDE;
				}
				if (someData->myHasVertexColor)
				{
					int lColorIndex = lVerticeIndex + (TRIANGLE_VERTEX_COUNT * lPolygonIndex);
					//if (lColorElement->GetReferenceMode() == FbxLayerElement::eIndexToDirect)
					//{
					//	lColorIndex = lColorElement->GetIndexArray().GetAt(lVerticeIndex);
					//}
					lCurrentColor = lColorElement->GetDirectArray().GetAt(lColorIndex);

					lControlPointIndex;
					lPolygonIndex;


					lVertices[currentIndex + addedSize] = lCurrentColor[0];
					lVertices[currentIndex + addedSize + 1] = lCurrentColor[1];
					lVertices[currentIndex + addedSize + 2] = lCurrentColor[2];
					lVertices[currentIndex + addedSize + 3] = lCurrentColor[3];
					addedSize += COLOR_STRIDE;
				}
			}
			++lVertexCount;
		}
		someData->mSubMeshes[lMaterialIndex]->TriangleCount += 1;
	}

	someData->myVertexCount = lVertexCount;
	someData->myVertexStride = stride;

	someData->myVertexBuffer = lVertices;
	someData->myIndicies = lIndices;

	if (weights)
	{
		delete[] weights;
		delete[] bones;
	}

	return true;
}

FbxAMatrix GetGlobalPosition(FbxNode* pNode, FbxPose* pPose, FbxAMatrix* pParentGlobalPosition)
{
	FbxAMatrix lGlobalPosition;
	bool        lPositionFound = false;

	if (pPose)
	{
		int lNodeIndex = pPose->Find(pNode);

		if (lNodeIndex > -1)
		{
			// The bind pose is always a global matrix.
			// If we have a rest pose, we need to check if it is
			// stored in global or local space.
			if (pPose->IsBindPose() || !pPose->IsLocalMatrix(lNodeIndex))
			{
				lGlobalPosition = GetPoseMatrix(pPose, lNodeIndex);
			}
			else
			{
				// We have a local matrix, we need to convert it to
				// a global space matrix.
				FbxAMatrix lParentGlobalPosition;

				if (pParentGlobalPosition)
				{
					lParentGlobalPosition = *pParentGlobalPosition;
				}
				else
				{
					if (pNode->GetParent())
					{
						lParentGlobalPosition = GetGlobalPosition(pNode->GetParent(), pPose, nullptr);
					}
				}

				FbxAMatrix lLocalPosition = GetPoseMatrix(pPose, lNodeIndex);
				lGlobalPosition = lParentGlobalPosition * lLocalPosition;
			}
			lPositionFound = true;
		}
	}
	return lGlobalPosition;
}

CU::Matrix44<float> CreateMatrix(FbxAMatrix& aOriention)
{
	CU::Matrix44f returnMatrix;
	for (int y = 0; y < 4; ++y)
	{
		for (int x = 0; x < 4; ++x)
		{
			returnMatrix.myMatrix[y * 4 + x] = (float)aOriention.Get(y, x);
		}
	}
	return returnMatrix;
}

float GetAnimationTime(FbxNode* aNode, FbxAnimLayer* aCurrentAnimLayer)
{
	aCurrentAnimLayer;
	FbxTimeSpan span;
	aNode->GetAnimationInterval(span);
	return float(span.GetStop().GetSecondDouble());
	//double time = 0.0f;
	//FbxAnimCurve* translationX = aNode->LclTranslation.GetCurve(aCurrentAnimLayer, FBXSDK_CURVENODE_COMPONENT_X);
	//FbxAnimCurve* translationY = aNode->LclTranslation.GetCurve(aCurrentAnimLayer, FBXSDK_CURVENODE_COMPONENT_Y);
	//FbxAnimCurve* translationZ = aNode->LclTranslation.GetCurve(aCurrentAnimLayer, FBXSDK_CURVENODE_COMPONENT_Z);
	//if (translationX && translationX->KeyGetCount() > 0)
	//{
	//	time = max(time, translationX->KeyGet(translationX->KeyGetCount() - 1).GetTime().GetSecondDouble());
	//}
	//if (translationY && translationY->KeyGetCount() > 0)
	//{
	//	time = max(time, translationY->KeyGet(translationY->KeyGetCount() - 1).GetTime().GetSecondDouble());
	//}
	//if (translationZ && translationZ->KeyGetCount() > 0)
	//{
	//	time = max(time, translationZ->KeyGet(translationZ->KeyGetCount() - 1).GetTime().GetSecondDouble());
	//}

	//FbxAnimCurve* rotationX = aNode->LclRotation.GetCurve(aCurrentAnimLayer, FBXSDK_CURVENODE_COMPONENT_X);
	//FbxAnimCurve* rotationY = aNode->LclRotation.GetCurve(aCurrentAnimLayer, FBXSDK_CURVENODE_COMPONENT_Y);
	//FbxAnimCurve* rotationZ = aNode->LclRotation.GetCurve(aCurrentAnimLayer, FBXSDK_CURVENODE_COMPONENT_Z);
	//if (rotationX && rotationX->KeyGetCount() > 0)
	//{
	//	time = max(time, rotationX->KeyGet(rotationX->KeyGetCount() - 1).GetTime().GetSecondDouble());
	//}
	//if (rotationY && rotationY->KeyGetCount() > 0)
	//{
	//	time = max(time, rotationY->KeyGet(rotationY->KeyGetCount() - 1).GetTime().GetSecondDouble());
	//}
	//if (rotationZ && rotationZ->KeyGetCount() > 0)
	//{
	//	time = max(time, rotationZ->KeyGet(rotationZ->KeyGetCount() - 1).GetTime().GetSecondDouble());
	//}

	//FbxAnimCurve* scaleX = aNode->LclScaling.GetCurve(aCurrentAnimLayer, FBXSDK_CURVENODE_COMPONENT_X);
	//FbxAnimCurve* scaleY = aNode->LclScaling.GetCurve(aCurrentAnimLayer, FBXSDK_CURVENODE_COMPONENT_Y);
	//FbxAnimCurve* scaleZ = aNode->LclScaling.GetCurve(aCurrentAnimLayer, FBXSDK_CURVENODE_COMPONENT_Z);
	//if (scaleX && scaleX->KeyGetCount() > 0)
	//{
	//	time = max(time, scaleX->KeyGet(scaleX->KeyGetCount() - 1).GetTime().GetSecondDouble());
	//}
	//if (scaleY && scaleY->KeyGetCount() > 0)
	//{
	//	time = max(time, scaleY->KeyGet(scaleY->KeyGetCount() - 1).GetTime().GetSecondDouble());
	//}
	//if (scaleZ && scaleZ->KeyGetCount() > 0)
	//{
	//	time = max(time, scaleZ->KeyGet(scaleZ->KeyGetCount() - 1).GetTime().GetSecondDouble());
	//}
	//return (float)time;
}

CU::Matrix44<float> GetAnimationMatrix(FbxNode* aNode, FbxAnimLayer* aCurrentAnimLayer, float aTime, CU::Matrix44<float>& offsetMatrix)
{
	aTime;
	aCurrentAnimLayer;
	aNode;

	CU::Matrix44<float> returnMatrix = offsetMatrix;
	return returnMatrix;
	//CU::Vector3<float> startTranslation = offsetMatrix.Get();
	//FbxTime time;
	//time.SetSecondDouble(aTime);
	//CU::Vector3<float> translation(0, 0, 0);
	//
	//FbxAnimCurve* translationX = aNode->LclTranslation.GetCurve(aCurrentAnimLayer, FBXSDK_CURVENODE_COMPONENT_X);
	//FbxAnimCurve* translationY = aNode->LclTranslation.GetCurve(aCurrentAnimLayer, FBXSDK_CURVENODE_COMPONENT_Y);
	//FbxAnimCurve* translationZ = aNode->LclTranslation.GetCurve(aCurrentAnimLayer, FBXSDK_CURVENODE_COMPONENT_Z);
	//if (translationX)
	//{
	//	translation.x = translationX->Evaluate(time);
	//}
	//else
	//{
	//	translation.x = startTranslation.x;
	//}
	//if (translationY)
	//{
	//	translation.y = translationY->Evaluate(time);
	//}
	//else
	//{
	//	translation.y = startTranslation.y;
	//}
	//if (translationZ)
	//{
	//	translation.z = translationZ->Evaluate(time);
	//}
	//else
	//{
	//	translation.z = startTranslation.z;
	//}
	//
	//
	//
	//FbxVector4 rotation(0, 0, 0, 0);
	//FbxAnimCurve* rotationX = aNode->LclRotation.GetCurve(aCurrentAnimLayer, FBXSDK_CURVENODE_COMPONENT_X);
	//FbxAnimCurve* rotationY = aNode->LclRotation.GetCurve(aCurrentAnimLayer, FBXSDK_CURVENODE_COMPONENT_Y);
	//FbxAnimCurve* rotationZ = aNode->LclRotation.GetCurve(aCurrentAnimLayer, FBXSDK_CURVENODE_COMPONENT_Z);
	//if (rotationX)
	//{
	//	rotation[0] = -rotationX->Evaluate(time)*(PI / 180.0f);
	//}
	//if (rotationY)
	//{
	//	rotation[1] = rotationY->Evaluate(time)*(PI / 180.0f);
	//}
	//if (rotationZ)
	//{
	//	rotation[2] = -rotationZ->Evaluate(time)*(PI / 180.0f);
	//}
	//CU::Matrix44<float> rotationMatrix;
	//returnMatrix.SetPosition(translation);
	//
	//CU::Matrix44<float> fixMatrix;
	//fixMatrix.myMatrix[0] = -1;
	//
	//returnMatrix = fixMatrix * returnMatrix * fixMatrix;
	//returnMatrix *= CU::Matrix44<float>::CreateRotateAroundZ(rotation[2]);
	//returnMatrix *= CU::Matrix44<float>::CreateRotateAroundY(rotation[1]);
	//returnMatrix *= CU::Matrix44<float>::CreateRotateAroundX(rotation[0]);
	//
	//FbxVector4 scale(1, 1, 1, 1);
	//FbxAnimCurve* scaleX = aNode->LclScaling.GetCurve(aCurrentAnimLayer, FBXSDK_CURVENODE_COMPONENT_X);
	//FbxAnimCurve* scaleY = aNode->LclScaling.GetCurve(aCurrentAnimLayer, FBXSDK_CURVENODE_COMPONENT_Y);
	//FbxAnimCurve* scaleZ = aNode->LclScaling.GetCurve(aCurrentAnimLayer, FBXSDK_CURVENODE_COMPONENT_Z);
	//if (scaleX)
	//{
	//	scale[0] = scaleX->Evaluate(time);
	//}
	//if (scaleY)
	//{
	//	scale[1] = scaleY->Evaluate(time);
	//}
	//if (scaleZ)
	//{
	//	scale[2] = scaleZ->Evaluate(time);
	//}
	//
	//FbxAMatrix currentMatrix;
	////currentMatrix.SetTRS(translation,rotationMatrix,scale);
	//
	//
	//return returnMatrix;
}

void LoadAnimation(const char* aFilePath, AnimationData& aAnimation, FbxNode* aNode, FbxAMatrix& aParentOrientation, FbxPose* aPose, FbxAnimLayer* aCurrentAnimLayer, int parentBone
	, CU::GrowingArray<std::string>& someOutErrors)
{
	FbxAMatrix lGlobalPosition = GetGlobalPosition(aNode, 0.0f, aPose, &aParentOrientation);
	FbxNodeAttribute* lNodeAttribute = aNode->GetNodeAttribute();
	int boneId = -1;
	if (lNodeAttribute)
	{
		if (lNodeAttribute->GetAttributeType() == FbxNodeAttribute::eSkeleton)
		{
			Bone newBone;
			newBone.myAnimationTime = GetAnimationTime(aNode, aCurrentAnimLayer);
			float oneFrameTime = 1.0f / 24.0f;

			CU::Matrix44<float> fixMatrix;
			fixMatrix.myMatrix[0] = -1;
			FbxAMatrix lLocalTransform = aNode->EvaluateLocalTransform();
			newBone.myBaseOrientation = fixMatrix * CreateMatrix(lLocalTransform) * fixMatrix;

			char buffer[32];
			_itoa_s<32>(parentBone, buffer, 10);
			newBone.myName = aNode->GetName();
			newBone.myName += buffer;

			int lNodeIndex = aPose->Find(aNode);
			auto bindPoseMatrix = aPose->GetMatrix(lNodeIndex);

			FbxAMatrix bindMatrix;
			memcpy((double*)bindMatrix, (double*)bindPoseMatrix, sizeof(bindMatrix.mData));

			FbxAMatrix localPosOffset;

			memcpy((double*)localPosOffset, (double*)bindPoseMatrix, sizeof(localPosOffset.mData));
			localPosOffset = localPosOffset * aParentOrientation.Inverse();

			newBone.myBindMatrix = fixMatrix * CreateMatrix(lGlobalPosition.Inverse()) * fixMatrix;

			CU::Matrix44<float> localStartOffset = CreateMatrix(bindMatrix.Inverse());
			for (float currentFrameTime = 0.0f; currentFrameTime < newBone.myAnimationTime; currentFrameTime += oneFrameTime)
			{
				KeyFrame keyFrame;
				keyFrame.myTime = currentFrameTime;

				FbxTime time;
				time.SetSecondDouble(currentFrameTime);
				keyFrame.myMatrix = fixMatrix * CreateMatrix(aNode->EvaluateLocalTransform(time)) * fixMatrix;
				newBone.myFrames.push_back(keyFrame);
			}
			FbxAMatrix animationMatrix;

			FbxSkeleton* skeleton = aNode->GetSkeleton();
			if (skeleton->IsSkeletonRoot())
			{
				aAnimation.myBindMatrix = CU::Matrix44<float>();
				aAnimation.myRootBone = aAnimation.myBones.size();
			}
			boneId = aAnimation.myBones.size();
			aNode->SetUserDataPtr((void*)boneId);

			if (parentBone != -1)
			{
				aAnimation.myBones[parentBone].myChilds.push_back(boneId);
			}
			newBone.myId = boneId;
			aAnimation.myBones.push_back(newBone);
		}
	}

	const int lChildCount = aNode->GetChildCount();
	for (int lChildIndex = 0; lChildIndex < lChildCount; ++lChildIndex)
	{
		LoadAnimation(aFilePath, aAnimation, aNode->GetChild(lChildIndex), lGlobalPosition, aPose, aCurrentAnimLayer, boneId, someOutErrors);
	}
}

void SetLodGroup(FbxModelData* aModel, FbxNode* aNode)
{
	// Create a lod group for this model
	Prism::LodGroup* group = aModel->CreateLodGroup();
	FbxLODGroup *lLodGroupAttr = (FbxLODGroup*)aNode->GetNodeAttribute();
	double minDistance = 0; // Minumum distance the lod group will be rendered at
	double maxDistance = 1000; // Maximum distance the lod group will be rendered at
	if (lLodGroupAttr->MinMaxDistance.Get())
	{
		// If the value is set from Maya, read it
		minDistance = lLodGroupAttr->MinDistance.Get();
		maxDistance = lLodGroupAttr->MaxDistance.Get();
	}

	// Display levels
	int displayLevels = lLodGroupAttr->GetNumDisplayLevels();
	for (int i = 0; i < displayLevels; i++)
	{
		Prism::Lod& lod = group->CreateLod();
		FbxLODGroup::EDisplayLevel lLevel;
		if (lLodGroupAttr->GetDisplayLevel(i, lLevel))
		{
			lod.myLevel = i;
			lod.myUseLod = lLevel == FbxLODGroup::eUseLOD;
		}

	}

	// Add the levels and the values for them
	group->myThreshHolds.Add(minDistance);
	for (int i = 0; i < lLodGroupAttr->GetNumThresholds(); i++)
	{
		FbxDistance lThreshVal;
		if (lLodGroupAttr->GetThreshold(i, lThreshVal))
		{
			double threasHoldValue = lThreshVal.value();
			group->myThreshHolds.Add(threasHoldValue);
		}
	}
	group->myThreshHolds.Add(maxDistance);

}

void LoadNodeRecursive(const char* aFilePath, FbxModelData* aModel, AnimationData& aAnimation, FbxNode* aNode, FbxAMatrix& aParentOrientation
	, FbxPose* aPose, FbxAnimLayer* aCurrentAnimLayer, int parentBone, CU::GrowingArray<std::string>& someOutErrors)
{
	parentBone;

	FbxAMatrix lGlobalPosition = GetGlobalPosition(aNode, 0.0f, aPose, &aParentOrientation);
	FbxNodeAttribute* lNodeAttribute = aNode->GetNodeAttribute();

	if (lNodeAttribute && lNodeAttribute->GetAttributeType() == FbxNodeAttribute::eSkeleton)
	{
		return;
	}

	CU::Matrix44<float> fixMatrix = CU::Matrix44<float>::CreateReflectionMatrixAboutAxis(CU::Vector3<float>(1, 0, 0));

	FbxAMatrix lGeometryOffset = GetGeometry(aNode);
	FbxAMatrix lGlobalOffPosition = lGlobalPosition * lGeometryOffset;

	FbxAMatrix lRotationOffset = GetRotaionPivot(aNode);

	aModel->myRotationPivot = fixMatrix * CreateMatrix(lRotationOffset) * fixMatrix;

	aModel->myOrientation = fixMatrix * CreateMatrix(lGlobalOffPosition) * fixMatrix;

	int boneId = -1;
	if (lNodeAttribute)
	{

		if (lNodeAttribute->GetAttributeType() == FbxNodeAttribute::eMesh)
		{
			aModel->myData = new ModelData();
			aModel->myData->myLayout.Init(8);

			// Geometry offset.
			// it is not inherited by the children.
			FillData(aFilePath, aModel->myData, aNode, &aAnimation, someOutErrors);
		}
		else if (lNodeAttribute->GetAttributeType() == FbxNodeAttribute::eLight)
		{
			LoaderLight* newLight = new LoaderLight();
			FbxLight* light = aNode->GetLight();

			newLight->myIntensity = light->Intensity;
			auto color = light->Color.Get();
			newLight->myColor = { float(color.mData[0]), float(color.mData[1]), float(color.mData[2]) };

			auto type = light->LightType.Get();
			if (type == FbxLight::eDirectional)
			{
				newLight->myType = EDirectionalLight;
			}
			else if (type == FbxLight::ePoint)
			{
				newLight->myType = EPointLight;
			}
			else if (type == FbxLight::eSpot)
			{
				newLight->myInnerAngle = light->InnerAngle;
				newLight->myOuterAngle = light->OuterAngle;
			}

			aModel->myLight = newLight;
		}
		else if (lNodeAttribute->GetAttributeType() == FbxNodeAttribute::eCamera)
		{
			aModel->myCamera = new LoaderCamera();
			auto orgCamera = aNode->GetCamera();
			aModel->myCamera->myFov = orgCamera->FieldOfViewY;
		}
		else if (lNodeAttribute->GetAttributeType() == FbxNodeAttribute::eLODGroup)
		{
			SetLodGroup(aModel, aNode);
		}
		FbxTimeSpan animationInterval;

		if (aNode->GetAnimationInterval(animationInterval))
		{
			aModel->myAnimationCurves = new AnimationCurves();
			aModel->myAnimationCurves->myRotationCurve[0] = aNode->LclRotation.GetCurve(aCurrentAnimLayer, FBXSDK_CURVENODE_COMPONENT_X);
			aModel->myAnimationCurves->myRotationCurve[1] = aNode->LclRotation.GetCurve(aCurrentAnimLayer, FBXSDK_CURVENODE_COMPONENT_Y);
			aModel->myAnimationCurves->myRotationCurve[2] = aNode->LclRotation.GetCurve(aCurrentAnimLayer, FBXSDK_CURVENODE_COMPONENT_Z);
			aModel->myAnimationCurves->myRotationCurve[3] = aNode->LclRotation.GetCurve(aCurrentAnimLayer, FBXSDK_CURVENODE_ROTATION);
			aModel->myAnimationCurves->myScalingCurve[0] = aNode->LclScaling.GetCurve(aCurrentAnimLayer, FBXSDK_CURVENODE_COMPONENT_X);
			aModel->myAnimationCurves->myScalingCurve[1] = aNode->LclScaling.GetCurve(aCurrentAnimLayer, FBXSDK_CURVENODE_COMPONENT_Y);
			aModel->myAnimationCurves->myScalingCurve[2] = aNode->LclScaling.GetCurve(aCurrentAnimLayer, FBXSDK_CURVENODE_COMPONENT_Z);
			aModel->myAnimationCurves->myTtranslationCurve[0] = aNode->LclTranslation.GetCurve(aCurrentAnimLayer, FBXSDK_CURVENODE_COMPONENT_X);
			aModel->myAnimationCurves->myTtranslationCurve[1] = aNode->LclTranslation.GetCurve(aCurrentAnimLayer, FBXSDK_CURVENODE_COMPONENT_Y);
			aModel->myAnimationCurves->myTtranslationCurve[2] = aNode->LclTranslation.GetCurve(aCurrentAnimLayer, FBXSDK_CURVENODE_COMPONENT_Z);

			int nrOfKeys = 0;
			nrOfKeys;

			float startTime = (float)animationInterval.GetStart().GetSecondDouble();
			float endTime = (float)animationInterval.GetStop().GetSecondDouble();

			aModel->myAnimatedOrientation.Init((int)((endTime - startTime) / (1.0f / 24.0f)));

			for (float currentTime = startTime; currentTime < endTime; currentTime += 1.0f / 24.0f)
			{
				FbxTime time;
				time.SetSecondDouble(currentTime);

				KeyFrame animationFrame;
				animationFrame.myTime = currentTime;

				animationFrame.myMatrix = fixMatrix * CreateMatrix(aNode->EvaluateLocalTransform(time)) * fixMatrix;
				aModel->myAnimatedOrientation.Add(animationFrame);
			}
		}
	}

	const int lChildCount = aNode->GetChildCount();
	if (lChildCount > 0)
	{
		aModel->myChildren.Init(lChildCount);
		for (int lChildIndex = 0; lChildIndex < lChildCount; ++lChildIndex)
		{
			aModel->myChildren.Add(new FbxModelData());

			LoadNodeRecursive(aFilePath, aModel->myChildren.GetLast(), aAnimation, aNode->GetChild(lChildIndex)
				, lGlobalPosition, aPose, aCurrentAnimLayer, boneId, someOutErrors);
		}
	}
}

FbxModelData* FBXLoader::loadModel(const char* aFile, CU::GrowingArray<std::string>& someOutErrors)
{
	//DL_PRINT("FBXLoader Creating ModelData...");
	myLoadingModel = new FbxModelData;

	//DL_PRINT("Success!");
	//DL_PRINT("FBXLoader Creating TextureData...");
	myLoadingModel->myTextureData = new TextureData();
	//DL_PRINT("Success!");
	//DL_PRINT("FBXLoader Loading Scene...");
	auto scene = LoadScene(aFile);
	//DL_PRINT("Successfully loaded scene!");

	//DL_PRINT("FBXLoader Loading Textures...");
	//TextureData
	const int lTextureCount = scene->GetTextureCount();
	for (int lTextureIndex = 0; lTextureIndex < lTextureCount; ++lTextureIndex)
	{
		FbxTexture * lTexture = scene->GetTexture(lTextureIndex);
		FbxFileTexture * lFileTexture = reinterpret_cast<FbxFileTexture*>(lTexture);
		if (lFileTexture && !lFileTexture->GetUserDataPtr())
		{
			const FbxString lFileName = lFileTexture->GetFileName();

			unsigned int lTextureObject = 0;
			lTextureObject;
			bool lStatus = false;
			lStatus;

			std::string str = lFileTexture->GetFileName();
			str = CU::GetSubString(str, "Data", true, 1);

			TextureInfo info;
			info.myFileName = str;
			myLoadingModel->myTextureData->myTextures.push_back(info);
			lFileTexture->SetFileName(str.c_str());
		}
	}
	//DL_PRINT("Success!");
	//DL_PRINT("FBXLoader Loading Animations...");

	FbxArray<FbxString*> animationNames;
	FbxArray<FbxPose*> poses;
	scene->FillAnimStackNameArray(animationNames);
	scene->FillPoseArray(poses);
	FbxAnimStack * lCurrentAnimationStack = nullptr;
	FbxAnimLayer* lCurrentAnimLayer = nullptr;
	if (animationNames.GetCount() > 0)
	{
		lCurrentAnimationStack = scene->FindMember<FbxAnimStack>(animationNames[0]->Buffer());
		if (lCurrentAnimationStack != NULL)
		{
			lCurrentAnimLayer = lCurrentAnimationStack->GetMember<FbxAnimLayer>();
		}
	}
	//lCurrentAnimLayer->IsR
	myLoadingModel->myAnimation = new AnimationData();
	FbxPose* pose = nullptr;
	if (poses.GetCount() > 0)
	{
		pose = poses[0];
	}


	LoadAnimation(aFile, *myLoadingModel->myAnimation, scene->GetRootNode(), FbxAMatrix(), pose, lCurrentAnimLayer, -1, someOutErrors);
	LoadNodeRecursive(aFile, myLoadingModel, *myLoadingModel->myAnimation, scene->GetRootNode(), FbxAMatrix(), pose, lCurrentAnimLayer, -1
		, someOutErrors);

	//DL_PRINT("Success!");


	/*std::string xmlPath(aFile, strlen(aFile) - 4);
	xmlPath += ".xml";*/

	std::string xmlPath = CU::GetGeneratedDataFolderFilePath(aFile, "xml");
	if (CU::FileExists(xmlPath))
	{
		XMLReader reader;
		reader.OpenDocument(xmlPath);
		reader.ForceReadAttribute(reader.ForceFindFirstChild(reader.ForceFindFirstChild("root"), "radius"), "value", myLoadingModel->myRadius);
		reader.CloseDocument();
	}

	scene->Clear();
	return myLoadingModel;
}
