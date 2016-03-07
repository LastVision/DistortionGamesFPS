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
		const CU::Vector3<float>& GetPoint(int anId) const;
		float GetRadius() const;
		bool GetAlreadyPassed() const;
		void SetAlreadyPassed(bool aValue);

	private:
		bool FindCollisionPlane(const CU::Intersection::AABB& anAABB0, const CU::Intersection::AABB& anAABB1);
		bool PlaneInsideAABB(const CU::Intersection::AABB& anAABB, const CU::Vector3<float>& aMin, const CU::Vector3<float>& aMax);
		void CalcPoints();
		Room* myLargeRoom;
		Room* mySmallRoom;
		CU::Vector3<float> myTopRightBack;
		CU::Vector3<float> myBottomLeftFront;
		CU::Vector3<float> myCenterPosition;
		CU::Vector3<float> myPoints[4];
		float myRadius;
		bool myAlreadyPassed;
		CU::Intersection::AABB myAABB;
	};

	inline const CU::Vector3<float>& Portal::GetTopRight() const
	{
		return myTopRightBack;
	}

	inline const CU::Vector3<float>& Portal::GetBottomLeft() const
	{
		return myBottomLeftFront;
	}

	inline const CU::Vector3<float>& Portal::GetCenterPosition() const
	{
		return myCenterPosition;
	}

	inline const CU::Vector3<float>& Portal::GetPoint(int anId) const
	{
		DL_ASSERT_EXP(anId >= 0 && anId <= 4, "Index out of bounds.");
		return myPoints[anId];
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