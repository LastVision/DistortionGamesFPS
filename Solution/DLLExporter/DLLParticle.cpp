#include <CommonHelper.h>
#include <DebugDrawer.h>
#include <DL_Debug.h>
#include "DLLParticle.h"
#include <Engine.h>
#include <EngineEnums.h>
#include <FileWatcher.h>
#include <ParticleDataContainer.h>
#include <ParticleEmitterData.h>
#include <ParticleEmitterInstance.h>

DLLParticle::DLLParticle()
	: myIsLoaded(false)
{
	myOrientation.SetPos({0.f, 0.f, 80.f});
}

DLLParticle::~DLLParticle()
{
}

void DLLParticle::ReLoadParticle()
{
	LoadParticle(myParticleFile);
}

void DLLParticle::WatchFile(std::string& aParticleFile)
{
	Prism::FileWatcher::GetInstance()->Clear();
	Prism::FileWatcher::GetInstance()->WatchFile(aParticleFile, std::bind(&DLLParticle::ReLoadParticle, this));
}

void DLLParticle::LoadParticle(std::string& aParticleFile)
{
	myIsLoaded = false;
	WatchFile(aParticleFile);
	
	if (myCurrentParticle != nullptr)
	{
		myCurrentParticle->ReleaseData();
		delete myCurrentParticle;
		myCurrentParticle = nullptr;
	}

	if (myParticleData != nullptr)
	{
		delete myParticleData;
		myParticleData = nullptr;
	}

	CU::Matrix44f currentOrientation = myOrientation;
	std::stringstream ss;
	ss << "My current orientation at Load : " << "\nX : " << currentOrientation.GetPos().x << "\nY : " << currentOrientation.GetPos().y << "\nZ : " << currentOrientation.GetPos().z;
	DL_DEBUG(ss.str().c_str());
	myParticleData = new Prism::ParticleEmitterData();
	//myParticleData->LoadDataFile(aParticleFile.c_str());
	
	myCurrentParticle = new Prism::ParticleEmitterInstance(Prism::ParticleDataContainer::GetInstance()
		->GetParticleData(aParticleFile), false); //Man bör inte ha över 200 partiklar, ska vara satt till false
	myIsLoaded = true;
	myParticleFile = aParticleFile;
}

void DLLParticle::Update(float aDeltaTime)
{
	DEBUG_PRINT(myIsLoaded);
	if (myIsLoaded == false) return;
	DL_DEBUG("Update");
	myCurrentParticle->Update(aDeltaTime, CU::Matrix44f());
}

void DLLParticle::Render(Prism::Camera* aCamera)
{
	if (myIsLoaded == false) return;
	DL_DEBUG("Render");
	Prism::ParticleDataContainer::GetInstance()->SetGPUData(*aCamera);
	myCurrentParticle->Render();
}
