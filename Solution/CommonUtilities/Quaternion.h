#pragma once
#include "Vector.h"

namespace CU
{
	class Quaternion
	{
	public:
		Quaternion();
		Quaternion(const Vector3<float>& aNormal, float anAngle);

		Quaternion Inverted() const;

		Quaternion operator*(const Quaternion& aQuaternion) const;
		Vector3<float> operator*(const Vector3<float>& aVector) const;
		Quaternion operator^(float aT) const;

		Quaternion Slerp(const Quaternion& other, float aT) const;


	private:
		void ToAxisAngle(Vector3<float>& aVectorAxisOut, float& anAngleOut) const;
		Vector3<float> v;
		float w;
	};

}