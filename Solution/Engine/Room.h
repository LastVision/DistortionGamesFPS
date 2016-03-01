#pragma once
#include <AABB.h>

namespace Prism
{
	class Room
	{
	public:
		Room(const CU::Vector3<float>& aPosition, const CU::Vector3<float>& aScale);
		~Room();

		bool Inside(const CU::Vector3<float>& aPosition) const;
		bool Collide(const Room& aRoom) const;

	private:
		CU::Intersection::AABB myAABB;
	};
}