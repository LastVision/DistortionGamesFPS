#pragma once

namespace Prism
{
	class Terrain;
}

class Formation
{
public:
	Formation(const Prism::Terrain& aTerrain);
	~Formation();

	void Activate(const Entity* aTargetUnit = nullptr, const CU::Vector2<float>& aTarget = { -1.f, -1.f });

	bool GetActive() const;

	bool Update(float aDeltaTime);
	void AddUnit(Entity* aUnit);

private:
	void operator=(Formation&) = delete;
	void CalcPosition();
	void Reset();
	//CU::Vector2<float> myPosition;
	CU::Matrix33<float> myOrientation2D;
	CU::GrowingArray<Entity*> myUnits;
	CU::Vector2<float> myTarget;
	const Entity* myTargetUnit;
	const Prism::Terrain& myTerrain;
	CU::GrowingArray<CU::Vector2<float>> myPath;
	float mySpeed;
	bool myActive;
};

inline void Formation::AddUnit(Entity* aUnit)
{
	myUnits.Add(aUnit);
}

inline bool Formation::GetActive() const
{
	return myActive;
}