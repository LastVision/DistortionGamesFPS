#pragma once

class Entity;

struct EntityCommandData
{
	EntityCommandData();
	EntityCommandData(eEntityCommand aCommand, Entity* anEntity
		, const CU::Vector2<float>& aPosition = CU::Vector2<float>(-1.f, -1.f));
	
	bool operator==(const EntityCommandData& aCommandData) const;
	bool operator!=(const EntityCommandData& aCommandData) const;

	const CU::Vector2<float>& GetPosition() const;

	eEntityCommand myCommand;
	Entity* myEntity;
	CU::Vector2<float> myPosition;
};

inline bool EntityCommandData::operator==(const EntityCommandData& aCommandData) const
{
	return myCommand == aCommandData.myCommand && myEntity == aCommandData.myEntity
		&& myPosition == aCommandData.myPosition;
}

inline bool EntityCommandData::operator!=(const EntityCommandData& aCommandData) const
{
	return !operator==(aCommandData);
}