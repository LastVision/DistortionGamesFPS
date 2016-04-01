#pragma once
#include "Subscriber.h"
#include <GrowingArray.h>
#include <bitset>
#define PREALLOCATED_EMITTERGROUP 32

class Entity;
class ParticleEmitterComponent;

namespace Prism
{
	class ParticleEmitterInstance;
	class Camera;
};

struct EmitterData
{
	EmitterData(const std::string& aType);
	~EmitterData();
	CU::StaticArray<CU::GrowingArray<Prism::ParticleEmitterInstance*>, PREALLOCATED_EMITTERGROUP> myEmitters;
	std::bitset<PREALLOCATED_EMITTERGROUP> myFinishedGroups;
	std::string myType;
	short myCurrentIndex;
	short myActiveCount;
	bool myTypeIsActive;

};

class EmitterManager : public Subscriber
{
public:
	EmitterManager();
	~EmitterManager();
	void Initiate(Prism::Camera* aCamera);
	void UpdateEmitters(float aDeltaTime, CU::Matrix44f aWorldMatrix);
	void RenderEmitters();
	void ReceiveMessage(const EmitterMessage& aMessage) override;
private:
	void ReadListOfLists(const std::string& aPath);
	void ReadList(const std::string& aPath, const std::string& anID, short anIndex);
	std::unordered_map<std::string, EmitterData*> myEmitters;
	CU::GrowingArray<EmitterData*> myEmitterList;
	Prism::Camera* myCamera;
	short myEmitterIndex;

	void operator=(const EmitterManager&) = delete;





};

