#pragma once
#include "Message.h"

namespace Prism
{
	class Scene;
}

struct SpawnUnitMessage : public Message
{
	SpawnUnitMessage(int aUnitType, int aOwner, const CU::Vector2<float>& aSpawnPoint
		, const CU::Vector2<float>& aRallyPoint);

	void operator=(SpawnUnitMessage&) = delete;

	const int myUnitType;
	const int myOwnerType;
	const CU::Vector2<float>& mySpawnPoint;
	const CU::Vector2<float>& myRallyPoint;
};

inline SpawnUnitMessage::SpawnUnitMessage(int aUnitType, int aOwner, const CU::Vector2<float>& aSpawnPoint
		, const CU::Vector2<float>& aRallyPoint)
	: Message(eMessageType::SPAWN_UNIT)
	, myUnitType(aUnitType)
	, myOwnerType(aOwner)
	, mySpawnPoint(aSpawnPoint)
	, myRallyPoint(aRallyPoint)
{
}