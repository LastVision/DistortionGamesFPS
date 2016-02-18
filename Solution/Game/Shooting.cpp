#include "stdafx.h"

#include <InputWrapper.h>
#include <Instance.h>
#include <ModelLoader.h>
#include <Scene.h>
#include "Shooting.h"

Shooting::Shooting(Prism::Scene* aScene)
	: myBulletSpeed(0.f)
{
	myBullet = new Prism::Instance(*Prism::ModelLoader::GetInstance()->LoadModel("Data/Resource/Model/Test_temp/Test_arrow_01.fbx",
		"Data/Resource/Shader/S_effect_pbldebug.fx"),myBulletOrientation);
	aScene->AddInstance(myBullet);
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

	myBulletOrientation.SetPos(myBulletOrientation.GetPos() + myBulletOrientation.GetForward() * myBulletSpeed);
}

void Shooting::ShootAtDirection(const CU::Matrix44<float>& aOrientation)
{
	myBulletOrientation = aOrientation;
	myBulletSpeed = 100.f;
	
}