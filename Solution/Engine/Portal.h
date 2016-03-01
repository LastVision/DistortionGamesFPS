#pragma once

namespace Prism
{
	class Room;

	class Portal
	{
	public:
		Portal(const Room* aRoom0, const Room* aRoom1);
		~Portal();

	private:
		bool FindCollisionPlane(const CU::Intersection::AABB& anAABB0, const CU::Intersection::AABB& anAABB1);
		bool PlaneInsideAABB(const CU::Intersection::AABB& anAABB, const CU::Vector3<float>& aMin, const CU::Vector3<float>& aMax) const;
		const Room* myRooms[2];
		CU::Vector3<float> myTopLeft;
		CU::Vector3<float> myBotRight;
	};
}