#include "stdafx.h"
#include "Formation.h"
#include <Terrain.h>
#include <PathFinderFunnel.h>

Formation::Formation(const Prism::Terrain& aTerrain)
	: myUnits(GC::directorUnitCap)
	, myTerrain(aTerrain)
	, myPath(16)
	, mySpeed(100)
	, myTargetUnit(nullptr)
	, myActive(false)
{
	Reset();
}


Formation::~Formation()
{
}


void Formation::Activate(const Entity* aTargetUnit, const CU::Vector2<float>& aTarget)
{
	myActive = true;
	myTarget = aTarget;
	myTargetUnit = aTargetUnit;

	CalcPosition();

	CU::Vector2<float> target(aTarget);
	if (aTargetUnit != nullptr)
	{
		target = aTargetUnit->GetPosition();
	}
	if (myTerrain.GetPathFinder()->FindPath(myOrientation2D.GetPos(), target, myPath) == true)
	{
		CU::Vector2<float> direction2D(target - myOrientation2D.GetPos());
		CU::Normalize(direction2D);

		CU::Vector3<float> planeNormal(0, 0, 1.f);
		CU::Vector3<float> direction(direction2D.x, direction2D.y, 0);
		CU::Vector3<float> forward(0, 1.f, 0);
		float dot = CU::Dot(forward, direction);
		float det = CU::Dot(planeNormal, CU::Cross(forward, direction));

		float angle = atan2(det, dot);

		myOrientation2D.SetRotation2D(angle);
	}
}

//Update function, wait for units
bool Formation::Update(float aDeltaTime)
{
	CalcPosition();

	Prism::RenderBox(myTerrain.GetHeight(myOrientation2D.GetPos()), eColorDebug::BLUE);

	//recalc every 0.5 seconds if myTargetUnit != nullptr

	myOrientation2D.SetPos(myOrientation2D.GetPos() + CU::Vector2<float>(0, 1.f) * mySpeed * aDeltaTime);

	CU::Vector2<float> toTarget(myTarget - myOrientation2D.GetPos());

	if (CU::Dot(myPath.GetLast() - myOrientation2D.GetPos(), CU::Vector2<float>(0, 1.f) * myOrientation2D) < 0)
	{
		// now has reached target, go to next or is finished.

		myActive = false;
		Reset();
		return false;
	}


	
	return true;
}

void Formation::Reset()
{
	myUnits.RemoveAll();
	myPath.RemoveAll();
}

void Formation::CalcPosition()
{
	CU::Vector2<float> position;

	for (int i = 0; i < myUnits.Size(); ++i)
	{
		position += myUnits[i]->GetPosition();
	}

	position /= static_cast<float>(myUnits.Size());

	myOrientation2D.SetPos(position);
}