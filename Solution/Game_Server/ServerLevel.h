#pragma once
#include <SharedLevel.h>

class ServerLevel : public SharedLevel
{
public:
	ServerLevel();
	~ServerLevel();

	void onConstraintBreak(physx::PxConstraintInfo *constraints, physx::PxU32 count) override {};
	void onWake(physx::PxActor **actors, physx::PxU32 count) override {};
	void onSleep(physx::PxActor **actors, physx::PxU32 count) override {};
	void onContact(const physx::PxContactPairHeader &pairHeader, const physx::PxContactPair *pairs, physx::PxU32 nbPairs) override {};
	void onTrigger(physx::PxTriggerPair *pairs, physx::PxU32 count) override;

	void Update(const float aDeltaTime) override;
	void ReceiveMessage(const NetworkAddPlayerMessage& aMessage);
private:
	
};

