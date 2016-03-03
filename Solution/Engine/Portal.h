#pragma once
#include <AABB.h>

namespace Prism
{
	class Room;

	class Portal
	{
	public:
		Portal(Room* aRoom0, Room* aRoom1);
		~Portal();

		Room* GetOther(const Room* aRoom);

		const CU::Vector3<float>& GetTopRight() const;
		const CU::Vector3<float>& GetBottomLeft() const;
		const CU::Vector3<float>& GetCenterPosition() const;
		float GetRadius() const;
		bool GetAlreadyPassed() const;
		void SetAlreadyPassed(bool aValue);

	private:
		bool FindCollisionPlane(const CU::Intersection::AABB& anAABB0, const CU::Intersection::AABB& anAABB1);
		bool PlaneInsideAABB(const CU::Intersection::AABB& anAABB, const CU::Vector3<float>& aMin, const CU::Vector3<float>& aMax);
		Room* myLargeRoom;
		Room* mySmallRoom;
		CU::Vector3<float> myTopRight;
		CU::Vector3<float> myBottomLeft;
		CU::Vector3<float> myCenterPosition;
		float myRadius;
		bool myAlreadyPassed;
		CU::Intersection::AABB myAABB;
	};

	inline const CU::Vector3<float>& Portal::GetTopRight() const
	{
		return myTopRight;
	}

	inline const CU::Vector3<float>& Portal::GetBottomLeft() const
	{
		return myBottomLeft;
	}

	inline const CU::Vector3<float>& Portal::GetCenterPosition() const
	{
		return myCenterPosition;
	}

	inline float Portal::GetRadius() const
	{
		return myRadius;
	}

	inline bool Portal::GetAlreadyPassed() const
	{
		return myAlreadyPassed;
	}

	inline void Portal::SetAlreadyPassed(bool aValue)
	{
		myAlreadyPassed = aValue;
	}
}