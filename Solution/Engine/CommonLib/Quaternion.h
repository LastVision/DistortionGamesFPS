#ifndef QUATERNION_HEADER
#define QUATERNION_HEADER

namespace CommonLib
{
	template<class Type>
	class Quaternion
	{
	public:
		Quaternion();
		~Quaternion();

		void FromMatrix(const CL::Matrix33<Type>& aRotaionMatrix);
		void FromDirection(const CL::Vector3<Type> aDirection);

		Type Dot(const Quaternion aQuaternion);

		Type x;
		Type y;
		Type z;
		Type w;
	};

	template<class Type>
	Quaternion<Type> Slerp(const Quaternion<Type> aQuaternion,const Quaternion<Type> aQuaternion2, float aT)
	{
		Type cosOmega = aQuaternion.Dot(aQuaternion2);

		if(cosOmega < 0.0f)
		{
			aQuaternion2.w = -aQuaternion2.w;
			aQuaternion2.x = -aQuaternion2.x;
			aQuaternion2.y = -aQuaternion2.y;
			aQuaternion2.z = -aQuaternion2.z;
			cosOmega = -cosOmega;
		}

		float k0;
		float k1;
		if(cosOmega > 0.9999f)
		{
			k0 = 1.0f-aT;
			k1 = aT;
		}
		else
		{
			float sinOmega = sqrt(1.0f - cosOmega*cosOmega);

			float Omega = atan2(sinOmega, cosOmega);

			float oneOverSinOmega = 1.0f / sinOmega;

			k0 = sin((1.0f - aT) * omega) * oneOverSinOmega;
			k1 = sin(aT*omega) * oneOverSinOmega;
		}
		Quaternion<Type> returnQuat;
		returnQuat.w = aQuaternion.w*k0 + aQuaternion2.w*k1;
		returnQuat.x = aQuaternion.x*k0 + aQuaternion2.x*k1;
		returnQuat.y = aQuaternion.y*k0 + aQuaternion2.y*k1;
		returnQuat.z = aQuaternion.z*k0 + aQuaternion2.z*k1;
	};

	template<class Type>
	Quaternion<Type>::Quaternion()
	{
	}

	template<class Type>
	Quaternion<Type>::~Quaternion()
	{
	}

	template<class Type>
	Type Quaternion<Type>::Dot(const Quaternion aQuaternion)
	{
		return w*aQuaternion.w + x*aQuaternion.x + y*aQuaternion.y + z*aQuaternion.z;
	}

	template<class Type>
	void Quaternion<Type>::FromMatrix(const CL::Matrix33<Type>& aRotaionMatrix)
	{
		Type fourWSquardMinus1 = aRotaionMatrix._11 + aRotaionMatrix._22 + aRotaionMatrix._33;
		Type fourXSquardMinus1 = aRotaionMatrix._11 - aRotaionMatrix._22 - aRotaionMatrix._33;
		Type fourYSquardMinus1 = aRotaionMatrix._22 - aRotaionMatrix._11 - aRotaionMatrix._33;
		Type fourZSquardMinus1 = aRotaionMatrix._33 - aRotaionMatrix._11 - aRotaionMatrix._22;

		int biggestIndex = 0;
		float biggestSquaredMinus1 = fourWSquardMinus1;
		if(biggestSquaredMinus1 < fourXSquardMinus1)
		{
			biggestSquaredMinus1 = fourXSquardMinus1;
			biggestIndex = 1;
		}
		if(biggestSquaredMinus1 < fourYSquardMinus1)
		{
			biggestSquaredMinus1 = fourYSquardMinus1;
			biggestIndex = 2;
		}
		if(biggestSquaredMinus1 < fourZSquardMinus1)
		{
			biggestSquaredMinus1 = fourZSquardMinus1;
			biggestIndex = 3;
		}

		Type biggestVal = sqrt(biggestSquaredMinus1 + 1.0f) * 0.5f;
		Type mult = 0.25f / biggestVal;

		switch(biggestIndex)
		{
		case 0:
			w = biggestVal;
			x = (aRotaionMatrix._23 - aRotaionMatrix._32) * mult;
			y = (aRotaionMatrix._31 - aRotaionMatrix._13) * mult;
			z = (aRotaionMatrix._12 - aRotaionMatrix._21) * mult;
			break;
		case 1:
			x = biggestVal;
			w = (aRotaionMatrix._23 - aRotaionMatrix._32) * mult;
			y = (aRotaionMatrix._12 + aRotaionMatrix._21) * mult;
			z = (aRotaionMatrix._31 + aRotaionMatrix._13) * mult;
			break;
		case 2:
			y = biggestVal;
			w = (aRotaionMatrix._31 - aRotaionMatrix._13) * mult;
			x = (aRotaionMatrix._12 + aRotaionMatrix._21) * mult;
			z = (aRotaionMatrix._23 + aRotaionMatrix._32) * mult;
			break;
		case 3:
			z = biggestVal;
			w = (aRotaionMatrix._12 - aRotaionMatrix._21) * mult;
			x = (aRotaionMatrix._31 + aRotaionMatrix._13) * mult;
			y = (aRotaionMatrix._23 + aRotaionMatrix._32) * mult;
			break;
		};
	}

	template<class Type>
	void Quaternion<Type>::FromDirection(const CL::Vector3<Type> aDirection)
	{
		CL::Vector3<Type> NegEyePosition;
		CL::Vector3<Type> R0, R1, R2;
		CL::Vector3<Type> M;

		CL::Vector3<Type> up(0,1,0);
		if(up.Dot(aDirection) > 0.999f)
		{
			up = CL::Vector3<Type>(1,0,0);
		}

		R2 = aDirection;
		R2.Normalize();

		R0 = up.Cross(R2);
		R0.Normalize();

		R1 = R2.Cross(R0);

		CL::Matrix33<Type> matrix;

		matrix.myVectorRows[0] = R0;
		matrix.myVectorRows[1] = R1;
		matrix.myVectorRows[2] = R2;

		FromMatrix(matrix);
	}
}

#endif