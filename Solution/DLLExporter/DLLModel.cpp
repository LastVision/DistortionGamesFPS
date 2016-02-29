#include <DL_Debug.h>
#include "DLLModel.h"
#include <Engine.h>
#include <EngineEnums.h>
#include <Instance.h>
#include <Model.h>
#include <ModelLoader.h>
#include <ModelProxy.h>

DLLModel::DLLModel()
	: myInstance(nullptr)
{
}

DLLModel::~DLLModel()
{
	delete myInstance;
	myInstance = nullptr;
}

void DLLModel::Update(float aDeltaTime)
{
	AutoRotate(aDeltaTime);
}

void DLLModel::LoadModel(const char* aModelFile, const char* aShaderFile)
{
	if (aShaderFile == "")
	{
		aShaderFile = "Data/Resource/Shader/S_effect_basic.fx";
	}
	DL_DEBUG("Load Model Start");
	CU::Matrix44f currentOrientation = myOrientation;
	delete myInstance;

	Prism::ModelProxy* model = Prism::ModelLoader::GetInstance()->LoadModel(aModelFile, aShaderFile);

	myInstance = new Prism::Instance(*model, myOrientation);
	DL_DEBUG("Load Model End");
}


void DLLModel::AutoRotate(float aDeltaTime)
{
	CU::Vector3f orginalPos(myOrientation.GetPos());
	myOrientation.SetPos(CU::Vector3f());
	myOrientation = CU::Matrix44<float>::CreateRotateAroundX(myAutoRotationSpeed.x / (3.14f * 180)) * myOrientation;
	myOrientation = CU::Matrix44<float>::CreateRotateAroundY(myAutoRotationSpeed.y / (3.14f * 180)) * myOrientation;
	myOrientation = CU::Matrix44<float>::CreateRotateAroundZ(myAutoRotationSpeed.z / (3.14f * 180)) * myOrientation;
	myOrientation.SetPos(orginalPos);
}

void DLLModel::ManualRotate()
{
	myAutoRotationSpeed = { 0, 0, 0 };
	CU::Matrix44f staticRotation;
	CU::Vector3f orginalPos(myOrientation.GetPos());
	myOrientation.SetPos(CU::Vector3f());
	staticRotation = CU::Matrix44f::CreateRotateAroundX(myManualRotationAngle.x / (3.14f * 180));
	staticRotation = CU::Matrix44f::CreateRotateAroundY(myManualRotationAngle.y / (3.14f * 180));
	staticRotation = CU::Matrix44f::CreateRotateAroundZ(myManualRotationAngle.z / (3.14f * 180));
	myOrientation = myOrientation * staticRotation;
	myOrientation.SetPos(orginalPos);
}
