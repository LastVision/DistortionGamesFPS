#include "stdafx.h"

#include <InputWrapper.h>
#include <Instance.h>
#include <ModelLoader.h>
#include "Shooting.h"

Shooting::Shooting()
	: myBulletSpeed(0.f)
{
	myBullet = new Prism::Instance(*Prism::ModelLoader::GetInstance()->LoadModel("Data/Resource/Model/Test_temp/Test_arrow_01.fbx",
		"Data/Resource/Shader/S_effect_pbldebug.fx"),myBulletOrientation);
}

Shooting::~Shooting()
{
}

void Shooting::Update(float aDelta, const CU::Matrix44<float>& aOrientation)
{
	if (CU::InputWrapper::GetInstance()->MouseDown(0) == true)
	{
		ShootAtDirection(aOrientation);
	}

	myBullet->Update(aDelta);

	myBulletOrientation.SetPos(myBulletOrientation.GetForward() * myBulletSpeed);
}

void Shooting::Render(const Prism::Camera& aCamera)
{
	myBullet->Render(aCamera);
}

void Shooting::ShootAtDirection(const CU::Matrix44<float>& aOrientation)
{
	myBulletOrientation = aOrientation;
	myBulletSpeed = 100.f;
	
}