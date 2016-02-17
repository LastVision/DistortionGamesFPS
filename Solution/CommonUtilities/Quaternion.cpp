#include "stdafx.h"
#include "Quaternion.h"

namespace CU
{

	Quaternion::Quaternion()
		: v(0, 0, 0)
		, w(1.f)
	{
	}

	Quaternion::Quaternion(const Vector3<float>& aNormal, float anAngle)
		: v(aNormal * sinf(anAngle * 0.5f))
		, w(cosf(anAngle * 0.5f))
	{
	}

	Quaternion Quaternion::Inverted() const
	{
		Quaternion q;

		q.w = w;
		q.v = -v;

		return q;
	}

	Quaternion Quaternion::operator*(const Quaternion& aQuaternion) const
	{
		Quaternion r;
		r.w = w * aQuaternion.w - Dot(v, aQuaternion.v);
		r.v = v * aQuaternion.w + aQuaternion.v * w + Cross(v, aQuaternion.v);

		return r;
	}

	Vector3<float> Quaternion::operator*(const Vector3<float>& aVector) const
	{
		//Quaternion p;
		//p.w = 0;
		//p.v = aVector;

		//const Quaternion& q = *this;
		//return (q * p * q.Inverted()).v;

		Vector3<float> vcV = Cross(v, aVector);
		return aVector + vcV * (2.f * w) + Cross(v, vcV) * 2.f;
	}

	Quaternion Quaternion::operator^(float aT) const
	{
		float a;
		Vector3<float> n;

		ToAxisAngle(n, a);

		float aTimesT = a * aT;

		return Quaternion(n, aTimesT);
	}

	Quaternion Quaternion::Slerp(const Quaternion& other, float aT) const
	{
		//const Quaternion& q = *this;

		//Quaternion r = other;

		return ((other * Inverted()) ^ aT) * (*this);
	}

	void Quaternion::ToAxisAngle(Vector3<float>& aVectorAxisOut, float& anAngleOut) const
	{
		if (Length2(v) < 0.0001f)
		{
			aVectorAxisOut = Vector3<float>(1.f, 0, 0);
		}
		else
		{
			aVectorAxisOut = GetNormalized(v);
		}

		anAngleOut = acos(w) * 2.f;
	}
}