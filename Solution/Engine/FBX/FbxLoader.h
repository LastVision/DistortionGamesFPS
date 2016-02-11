#pragma once

#include <fbxsdk.h>
#include <vector>
#include <string>
#include <GrowingArray.h>
#include <Vector.h>
#include <Matrix44.h>
#include <Matrix33.h>
#include "../LodStructs.h"

#define FBXLoaderArray CU::GrowingArray

enum FBXTextureType
{
	ALBEDO,
	NORMAL,
	ROUGHNESS,
	METALNESS,
	AMBIENT,
	EMISSIVE,
	NR_OF_TEXTURETYPES,
};

struct TextureInfo
{
	std::string myFileName;
	FBXTextureType myType;
};

struct KeyFrame
{
	float myTime;
	CU::Matrix44<float> myMatrix;
};

struct Bone 
{
	std::string myName;
	float myAnimationTime;
	int myId;
	CU::Matrix44<float> myBaseOrientation;
	CU::Matrix44<float> myBindMatrix;
	std::vector<KeyFrame> myFrames;
	std::vector<int> myChilds;
};

struct AnimationData
{
	std::string myName;

	CU::Matrix44<float> myBindMatrix;
	int myRootBone;
	std::vector<Bone> myBones;
};

struct ModelData
{
	~ModelData()
	{
		mSubMeshes.DeleteAll();
		SAFE_DELETE(myVertexBuffer);
		SAFE_DELETE(myIndicies);
	}

	enum
	{
		VERTEX_VBO,
		INDEX_VBO,
		VBO_COUNT,
	};

	enum LayoutType
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

	struct Layout
	{
		int mySize;
		int myOffset;
		LayoutType myType;
		int mySemanticIndex;
	};

	// For every material, record the offsets in every VBO and triangle counts
	struct SubMesh
	{
		SubMesh() : IndexOffset(0), TriangleCount(0) {}

		int IndexOffset;
		int TriangleCount;
	};

	float* myVertexBuffer;
	unsigned int* myIndicies;
	int myVertexCount;
	int myVertexStride;
	int myIndexCount;

	std::vector<TextureInfo> myTextures;
	FBXLoaderArray<Layout> myLayout;
	FBXLoaderArray<SubMesh*> mSubMeshes;

	bool myHasBiNormal;
	bool myHasSkinweights;
	bool myHasTangents;
	bool mHasNormal;
	bool mHasUV;
	bool mAllByControlPoint;
	bool myHasVertexColor;
};

	

struct TextureData
{
	std::vector<TextureInfo> myTextures;
};

struct AnimationCurves
{
	FbxAnimCurve* myRotationCurve[4];
	FbxAnimCurve* myScalingCurve[3];
	FbxAnimCurve* myTtranslationCurve[3];
};

enum ELightType
{
	ESpotLight,
	EDirectionalLight,
	EPointLight,
};

struct LoaderLight
{
	ELightType myType;
	CU::Vector3<float> myColor;
	CU::Vector3<float> myDirection;
	CU::Matrix44<float> myOrientation;
	float myIntensity;
	float myInnerAngle;
	float myOuterAngle;
};

struct LoaderCamera
{
	float myFov;
};

class FbxModelData
{
public:
	FbxModelData()
	{
		myData = nullptr;
		myAnimation = nullptr;
		myTextureData = nullptr;
		myLight = nullptr;
		myCamera = nullptr;
		myLodGroup = nullptr;
	}

	~FbxModelData()
	{
		SAFE_DELETE(myLodGroup);
		SAFE_DELETE(myData);
		SAFE_DELETE(myLight);
		SAFE_DELETE(myCamera);
		SAFE_DELETE(myAnimation);
		SAFE_DELETE(myTextureData);
		SAFE_DELETE(myAnimationCurves);
		if (myChildren.Size() > 0)
		{
			myChildren.DeleteAll();
		}
	}

	Prism::LodGroup* CreateLodGroup(){ myLodGroup = new Prism::LodGroup(); return myLodGroup; }

	Prism::LodGroup* myLodGroup;
	ModelData* myData;
	LoaderLight* myLight;
	LoaderCamera* myCamera;
	AnimationData* myAnimation;
	TextureData* myTextureData;
	AnimationCurves* myAnimationCurves;
	FBXLoaderArray<	KeyFrame > myAnimatedOrientation;
	CU::Matrix44<float> myOrientation;
	CU::Matrix44<float> myRotationPivot;
	FBXLoaderArray<FbxModelData*> myChildren;
	float myFPS;
};

class FBXLoader
{
public:
	FBXLoader();
	~FBXLoader();

	FbxModelData* loadModel(const char* aFile, CU::GrowingArray<std::string>& someOutErrors);
private:
	FbxScene* LoadScene(const char* aFile);
	FbxModelData* myLoadingModel;

	FbxIOSettings* mySettings;
	FbxManager* myFbxManager;
};
