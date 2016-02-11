#include "stdafx.h"

#include "Entity.h"
#include "EntityCommandData.h"

EntityCommandData::EntityCommandData()
	: myCommand(eEntityCommand::STOP)
	, myEntity(nullptr)
	, myPosition(-1.f, -1.f)
{
}

EntityCommandData::EntityCommandData(eEntityCommand aCommand, Entity* anEntity
		, const CU::Vector2<float>& aPosition)
	: myCommand(aCommand)
	, myEntity(anEntity)
	, myPosition(aPosition)
{
}

const CU::Vector2<float>& EntityCommandData::GetPosition() const
{
	if (myEntity != nullptr)
	{
		return myEntity->GetPosition();
	}
	DL_ASSERT_EXP(myPosition.x != -1 && myPosition.y != -1, "Action Data not valid.");
	return myPosition;
}