#include "stdafx.h"

#include <InputWrapper.h>
#include <Instance.h>
#include <ModelLoader.h>
#include <Scene.h>
#include "Shooting.h"

#include <EntityFactory.h>
#include <PhysEntity.h>

Shooting::Shooting(Prism::Scene* aScene)
	: myBulletSpeed(0.f)
	, myScene(aScene)
{
	/*myBullet = EntityFactory::CreateEntity(eEntityType::PROJECTILE, *aScene, myBulletOrientation.GetPos());
	myBullet->Reset();
	myBullet->AddToScene();*/

	myBullets.Init(64);
}

Shooting::~Shooting()
{
	myBullets.DeleteAll();
}

void Shooting::Update(float aDelta, const CU::Matrix44<float>& aOrientation)
{
	if (CU::InputWrapper::GetInstance()->MouseIsPressed(0) == true && myBullets.Size() < 256)
	{
		//myBullet->GetPhysEntity()->AddForce({ 0.f, 1.f, 0.f }, 100000.f);
		ShootAtDirection(aOrientation);
	}

	/*myBullet->Update(aDelta);
	DEBUG_PRINT(myBullet->GetOrientation().GetPos());*/
	//myBullet->SetOrientation(myBulletOrientation);

	//myBulletOrientation.SetPos(myBulletOrientation.GetPos() + myBulletOrientation.GetForward() * myBulletSpeed);

	for (int i = 0; i < myBullets.Size(); ++i)
	{
		myBullets[i]->Update(aDelta);
	}
}

void Shooting::ShootAtDirection(const CU::Matrix44<float>& aOrientation)
{
	/*myBulletOrientation = aOrientation;
	myBulletSpeed = 100.f;

	myBullet->GetPhysEntity()->SetPosition(aOrientation.GetPos());
	myBullet->GetPhysEntity()->AddForce(aOrientation.GetForward(), 20.f);*/

	SET_RUNTIME(false);
	Entity* bullet = EntityFactory::CreateEntity(eEntityType::PROJECTILE, *myScene, myBulletOrientation.GetPos());
	RESET_RUNTIME;
	bullet->Reset();
	bullet->AddToScene();
	bullet->GetPhysEntity()->SetPosition(aOrientation.GetPos());
	bullet->GetPhysEntity()->AddForce(aOrientation.GetForward(), 20.f);

	myBullets.Add(bullet);
}