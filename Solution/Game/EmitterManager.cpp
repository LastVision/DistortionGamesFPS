#include "stdafx.h"

#include "EmitterManager.h"
#include <EmitterMessage.h>
#include <ParticleDataContainer.h>
#include <ParticleEmitterInstance.h>
#include <PostMaster.h>
#include <XMLReader.h>
#include <TimerManager.h>
#include "CommonHelper.h"
#include <Camera.h>
#include <Frustum.h>
#define FINISHED 0
#define UNFINISHED 1


EmitterManager::EmitterManager(const Prism::Camera& aCamera)
	: myEmitterList(64)
	, myCamera(aCamera)
	, myAvailableEmitterCount(0)
{
	PostMaster* postMaster = PostMaster::GetInstance();
	postMaster->Subscribe(eMessageType::PARTICLE, this);
	ReadListOfLists("Data/Resource/Particle/LI_emitter_lists.xml");

	for (auto it = myEmitters.begin(); it != myEmitters.end(); ++it)
	{
		myEmitterList.Add(it->second);
	}
}

EmitterManager::~EmitterManager()
{
	PostMaster* postMaster = PostMaster::GetInstance();
	postMaster->UnSubscribe(eMessageType::PARTICLE, this);
	myEmitterList.RemoveAll();
	for (auto it = myEmitters.begin(); it != myEmitters.end(); ++it)
	{
		delete it->second;
		it->second = nullptr;
	}
}

void EmitterManager::UpdateEmitters(float aDeltaTime, CU::Matrix44f aWorldMatrix)
{
	for (int i = 0; i < myEmitterList.Size(); ++i)
	{
		if (myEmitterList[i]->myGroupIsActive == false)
		{
			continue;
		}
		for (int k = 0; k < PREALLOCATED_EMITTERGROUP; k++)
		{
			if (myEmitterList[i]->myFinishedGroups[k] == FINISHED)
			{
				continue;
			}

			for (int j = 0; j < myEmitterList[i]->myEmitters[k].Size(); ++j)
			{
				Prism::ParticleEmitterInstance* instance = myEmitterList[i]->myEmitters[k][j];

				if (instance->IsActive() == false)
				{
					continue;
				}
				if (instance->GetEntity() != nullptr)
				{
					instance->SetPosition({ instance->GetEntity()->GetOrientation().GetPos().x, 3.f
						, instance->GetEntity()->GetOrientation().GetPos().z });
				}
				if (instance->GetCamera() != nullptr)
				{
					CU::Vector3<float> pos = instance->GetCamera()->GetOrientation().GetPos();
					instance->SetPosition({ pos.x, pos.y - 50.f, pos.z + 65.f });

				}
				instance->Update(aDeltaTime, aWorldMatrix);
			}
		}
	}
}

void EmitterManager::RenderEmitters()
{
	Prism::ParticleDataContainer::GetInstance()->SetGPUData(myCamera);
	for (int i = 0; i < myEmitterList.Size(); ++i)
	{
		if (myEmitterList[i]->myGroupIsActive == false)
		{
			continue;
		}
		for (int k = 0; k < PREALLOCATED_EMITTERGROUP; k++)
		{
		int finished = 0;
			if (myEmitterList[i]->myFinishedGroups[k] == FINISHED)
			{
				continue;
			}

			for (int j = 0; j < myEmitterList[i]->myEmitters[k].Size(); ++j)
			{
				Prism::ParticleEmitterInstance* instance = myEmitterList[i]->myEmitters[k][j];
				if (instance->IsActive() == false)
				{
					finished++;
					
				}
				else
				{
					if (instance->GetShouldAlwaysShow() == true)
					{
						instance->Render();
					}
				}
			}

			if (finished >= myEmitterList[i]->myEmitters[k].Size())
			{
				myEmitterList[i]->myFinishedCount--;
				myEmitterList[i]->myFinishedGroups[k] = FINISHED;
				//if (myEmitterList[i]->myFinishedCount >= PREALLOCATED_EMITTERGROUP)
				if (myEmitterList[i]->myFinishedCount <= 0)
				{
					myEmitterList[i]->myGroupIsActive = false;
				}
			}
		}
	}
}

void EmitterManager::ReceiveMessage(const EmitterMessage& aMessage)
{
	CU::Vector3f position = aMessage.myPosition;

	if (aMessage.myEmitter != nullptr)
	{
		if (aMessage.myShouldKillEmitter == true)
		{
			aMessage.myEmitter->KillEmitter(1.f);
		}
	}

	if (aMessage.myShouldAlwaysShow == true || aMessage.myIsArtifact == true)
	{
		std::string particleType = CU::ToLower(aMessage.myParticleTypeString);
		if (particleType == "")
		{
			return;
		}
		DL_ASSERT_EXP(myEmitters.find(particleType) != myEmitters.end(), "Effect did not exist!");

		if (myEmitters[particleType]->myCurrentIndex > (PREALLOCATED_EMITTERGROUP - 1))
		{
			myEmitters[particleType]->myCurrentIndex = 0;

			short index = myEmitters[particleType]->myCurrentIndex;
		//	DL_ASSERT_EXP(myEmitters[particleType]->myEmitters[index][0]->IsActive() == false, "Particle popped");
		}

		short index = myEmitters[particleType]->myCurrentIndex;

		/*if (myEmitters[particleType]->myEmitters[index][0]->IsActive() == true)
		{
			return;
		}*/

		for (int i = 0; i < myEmitters[particleType]->myEmitters[index].Size(); ++i)
		{
			Prism::ParticleEmitterInstance* instance = myEmitters[particleType]->myEmitters[index][i];


			instance->SetEntity(nullptr);

			if (aMessage.myShouldAlwaysShow == true)
			{
				instance->SetShouldAlwaysShow(true);
			}

			instance->SetPosition(position);
			instance->Activate();
			if (aMessage.myEmitterLifeTime > 0.f)
			{
				instance->SetEmitterLifeTime(aMessage.myEmitterLifeTime);
			}
			if (aMessage.myRadius > 0.f)
			{
				instance->SetRadius(aMessage.myRadius);
			}
			if ((aMessage.mySize.x > 0.f) && (aMessage.mySize.y > 0.f) && (aMessage.mySize.z > 0.f))
			{
				instance->SetSize(aMessage.mySize);
			}

			if (aMessage.myShouldKillEmitter == true)
			{
				instance->SetEmitterLifeTime(aMessage.myEmitterLifeTime);
			}



		}
		myEmitters[particleType]->myFinishedGroups[index] = UNFINISHED;
		myEmitters[particleType]->myFinishedCount++;
		myEmitters[particleType]->myGroupIsActive = true;
		myEmitters[particleType]->myCurrentIndex++;
	}
}

void EmitterManager::ReadListOfLists(const std::string& aPath)
{
	XMLReader rootDocument;
	rootDocument.OpenDocument(aPath);
	tinyxml2::XMLElement* rootElement = rootDocument.FindFirstChild("root");
	for (tinyxml2::XMLElement* e = rootDocument.FindFirstChild(rootElement); e != nullptr; e = rootDocument.FindNextElement(e))
	{
		std::string entityPath = "";
		rootDocument.ForceReadAttribute(e, "src", entityPath);
		std::string ID;
		rootDocument.ForceReadAttribute(e, "ID", ID);
		ID = CU::ToLower(ID);
		if (entityPath != "")
		{
			EmitterData* newData = new EmitterData(entityPath);
			myEmitters[ID] = newData;

			for (short i = 0; i < PREALLOCATED_EMITTERGROUP; ++i)
			{
				ReadList(entityPath, ID, i);

			}
		}
	}
	rootDocument.CloseDocument();
}

void EmitterManager::ReadList(const std::string& aPath, const std::string& anID, short anIndex)
{
	XMLReader rootDocument;
	rootDocument.OpenDocument(aPath);
	tinyxml2::XMLElement* rootElement = rootDocument.FindFirstChild("root");
	for (tinyxml2::XMLElement* e = rootDocument.FindFirstChild(rootElement); e != nullptr;
		e = rootDocument.FindNextElement(e))
	{
		std::string entityPath = "";
		rootDocument.ForceReadAttribute(e, "src", entityPath);
		if (entityPath == "Data/Resource/Particle/P_emitter_weather_snow_medium.xml" ||
			entityPath == "Data/Resource/Particle/P_emitter_weather_snow_small.xml")
		{
			Prism::ParticleEmitterInstance* newEmitter;
			newEmitter = new Prism::ParticleEmitterInstance(Prism::ParticleDataContainer::GetInstance()->
				GetParticleData(entityPath), true);
			newEmitter->SetShouldAlwaysShow(true);
			myEmitters[anID]->myEmitters[anIndex].Add(newEmitter);
		}
		else if (entityPath != "")
		{
			Prism::ParticleEmitterInstance* newEmitter;
			newEmitter = new Prism::ParticleEmitterInstance(Prism::ParticleDataContainer::GetInstance()->
				GetParticleData(entityPath), false);
			newEmitter->SetShouldAlwaysShow(false);
			myEmitters[anID]->myEmitters[anIndex].Add(newEmitter);
		}
	}
	rootDocument.CloseDocument();
}

//Data
EmitterData::EmitterData(const std::string& aType)
	: myType(aType)
	, myCurrentIndex(0)
	, myFinishedCount(0)
	, myGroupIsActive(false)
{
	for (int i = 0; i < PREALLOCATED_EMITTERGROUP; ++i)
	{
		myEmitters[i].Init(16);
	}
	myFinishedGroups.reset();
}

EmitterData::~EmitterData()
{
	for (unsigned short i = 0; i < PREALLOCATED_EMITTERGROUP; ++i)
	{
		myEmitters[i].DeleteAll();
	}
}