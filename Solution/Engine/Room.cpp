#include "stdafx.h"
#include <Intersection.h>
#include "Room.h"


namespace Prism
{
	Room::Room(const CU::Vector3<float>& aPosition, const CU::Vector3<float>& aScale
			, const std::string& aName, eRoomType aType)
		: myAABB(aPosition, aScale)
		, myPortals(8)
		, myInstances(128)
		, myPointLights(128)
		, myName(aName)
		, myType(aType)
	{
	}

	Room::~Room()
	{
		myEmitter = nullptr;
		myPortals.RemoveAll();
		myInstances.RemoveAll();
		myPointLights.RemoveAll();
	}

	bool Room::Inside(const CU::Vector3<float>& aPosition, float aRadius) const
	{
		return CU::Intersection::SphereInsideAABB(myAABB, aPosition, aRadius);
	}

	bool Room::Collide(const Room& aRoom) const
	{
		return CU::Intersection::AABBvsAABB(myAABB, aRoom.myAABB);
	}

	void Room::Add(Portal* aPortal)
	{
		myPortals.Add(aPortal);
	}

	void Room::Add(Instance* anInstance)
	{
		myInstances.Add(anInstance);
	}

	void Room::Add(PointLight* aPointLight)
	{
		myPointLights.Add(aPointLight);
	}

	void Room::AddEmitter(Prism::ParticleEmitterInstance* anEmitter)
	{
		myEmitter = anEmitter;
	}

	Prism::ParticleEmitterInstance* Room::GetEmitter()
	{
		return myEmitter;
	}

}