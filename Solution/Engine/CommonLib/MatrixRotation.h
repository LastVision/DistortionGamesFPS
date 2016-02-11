#ifndef MATRIX_ROTATIONS_HEADER
#define MATRIX_ROTATIONS_HEADER

#include "Matrix44.h"
#include "Quaternion.h"
#include "Angle3.h"

#define PI 3.141593f
#define dPI 3.14159265

namespace CommonLib
{

	float ToRadians(float aDegree);

	double ToRadiansD(double aDegree);

	float ToDegrees(float aRadian);

	double ToDegreesD(double aRadian);

	template<class TYPE>
	inline Matrix44<TYPE> CreateIdentityMatrix()
	{
		Matrix44<TYPE> temp;
		temp.SetToIdentity();
		return temp;
	}

	template<class TYPE>
	inline Matrix33<TYPE> CreateIdentityMatrix33()
	{
		Matrix33<TYPE> temp;
		temp.SetToIdentity();
		return temp;
	}

	template<class TYPE>
	inline Matrix44<TYPE> CreateSimpleMatrix(CL::Vector3f aPosition)
	{
		Matrix44<TYPE> temp;
		temp.SetToIdentity();
		temp.SetTranslation(aPosition);
		return temp;
	}
	
	
	template<class TYPE>
	inline Matrix44<TYPE> CreateRHtoLH()
	{
		Matrix44<TYPE> temp;
		temp.SetToIdentity();
		temp._11 = 0;
		temp._12 = 1;
		temp._21 = 1;
		temp._22 = 0;
		
		return temp;
	}

	template<class TYPE>
	Matrix33<TYPE> CreateMatrixFromDirection(const CL::Vector3<TYPE>& aDirection)
	{
		CL::Matrix33<TYPE> matrix;
		CL::Angle3<TYPE> angles;
		angles.FromDirection(aDirection);
		matrix.SetRotationAngles(angles.b,angles.p,angles.h);
		return matrix;
	}

	template<class TYPE>
	Matrix33<TYPE> CreateRotationXMatrix(const float aRadian)
	{
		Matrix33<TYPE> temp;
		temp.SetToIdentity();
		TYPE Sin,Cos;

		Sin=(TYPE)sin(aRadian);
		Cos=(TYPE)cos(aRadian);
		
		temp(1,1) = Cos;
		temp(2,1) = Sin;
		temp(2,2) = Cos;
		temp(1,2) = -Sin;

		return temp;
	}
	
	template <class TYPE>
	Matrix33<TYPE> RotationAroundVector(Vector3<TYPE> aVector,TYPE aAngle)
	{
		Matrix33 temp;
		float Sin,Cos;

		Sin=(float)sin(aAngle);
		Cos=(float)cos(aAngle);
		
		temp.myMatrix[0][0]=-aVector.x*aVector.x*(1-Cos)+Cos;
		temp.myMatrix[1][0]=aVector.x*aVector.y*(1-Cos)-aVector.z*Sin;
		temp.myMatrix[2][0]=aVector.x*aVector.z*(1-Cos)+aVector.y*Sin;

		temp.myMatrix[0][1]=-aVector.y*aVector.x*(1-Cos)+aVector.z*Sin;
		temp.myMatrix[1][1]=aVector.y*aVector.y*(1-Cos)+Cos;
		temp.myMatrix[2][1]=aVector.y*aVector.z*(1-Cos)-aVector.x*Sin;

		temp.myMatrix[0][2]=-aVector.x*aVector.z*(1-Cos)-aVector.y*Sin;
		temp.myMatrix[1][2]=aVector.y*aVector.z*(1-Cos)+aVector.x*Sin;
		temp.myMatrix[2][2]=aVector.z*aVector.z*(1-Cos)+Cos;

		return(temp);
	}

	template<class TYPE>
	Matrix44<TYPE> CreateRotationXMatrix44(const float aRadian,const TYPE aW = 1)
	{
		Matrix44<TYPE> temp;
		temp.SetToIdentity();
		temp.Set33(CreateRotationXMatrix<TYPE>(aRadian));
		temp._44 = aW;

		return temp;
	}

	template<class TYPE>
	Matrix33<TYPE> CreateReflectionMatrixAboutAxis(CL::Vector3<TYPE> reflectionVector)
	{
		CL::Matrix33<TYPE> reflectionMatrix;
		reflectionMatrix.myData[0] = 1-2*(reflectionVector.x*reflectionVector.x);
		reflectionMatrix.myData[1] = -2*(reflectionVector.x*reflectionVector.y);
		reflectionMatrix.myData[2] = -2*(reflectionVector.x*reflectionVector.z);

		reflectionMatrix.myData[3] = -2*(reflectionVector.y*reflectionVector.x);
		reflectionMatrix.myData[4] = 1-2*(reflectionVector.y*reflectionVector.y);
		reflectionMatrix.myData[5] = -2*(reflectionVector.y*reflectionVector.z);

		reflectionMatrix.myData[6] = -2*(reflectionVector.z*reflectionVector.x);
		reflectionMatrix.myData[7] = -2*(reflectionVector.z*reflectionVector.y);
		reflectionMatrix.myData[8] = 1-2*(reflectionVector.z*reflectionVector.z);
		return reflectionMatrix;
	};

	template<class TYPE>
	Matrix33<TYPE> CreateRotationYMatrix(const float aRadian)
	{
		Matrix33<TYPE> temp;
		temp.SetToIdentity();
		TYPE Sin,Cos;

		Sin=(TYPE)sin(aRadian);
		Cos=(TYPE)cos(aRadian);

		temp(0,0) = Cos;
		temp(2,0) = -Sin;
		temp(2,2) = Cos;
		temp(0,2) = Sin;

		return(temp);
	}

	template<class TYPE>
	Matrix44<TYPE> CreateRotationYMatrix44(const float aRadian,const TYPE aW = 1)
	{
		Matrix44<TYPE> temp;
		temp.SetToIdentity();
		temp.Set33(CreateRotationYMatrix<TYPE>(aRadian));
		temp._44 = aW;

		return temp;
	}

	template<class TYPE>
	Matrix33<TYPE> CreateRotationZMatrix(const float aRadian)
	{
		Matrix33<TYPE> temp;
		temp.SetToIdentity();
		TYPE Sin,Cos;

		Sin=(TYPE)sin(aRadian);
		Cos=(TYPE)cos(aRadian);

		temp(0,0) = Cos;
		temp(1,0) = Sin;
		temp(1,1) = Cos;
		temp(0,1) = -Sin;

		return(temp);
	}

	template<class TYPE>
	Matrix44<TYPE> CreateRotationZMatrix44(const float aRadian,const TYPE aW = 1)
	{
		Matrix44<TYPE> temp;
		temp.SetToIdentity();
		temp.Set33(CreateRotationZMatrix<TYPE>(aRadian));
		temp._44 = aW;

		return temp;
	}

	template<class TYPE>
	Matrix33<TYPE> CreateRotationMatrix(const Vector3<TYPE> aAxis, const float aRadian)
	{
		Matrix33<TYPE> temp;
		temp.SetToIdentity();
		TYPE Sin,Cos;

		Sin=(TYPE)sin(aRadian);
		Cos=(TYPE)cos(aRadian);
		
		temp(0,0) = aAxis.x * aAxis.x * (1-Cos) + Cos;
		temp(1,0) = aAxis.x * aAxis.y * (1-Cos) - aAxis.z * Sin;
		temp(2,0) = aAxis.x * aAxis.z * (1-Cos) + aAxis.y * Sin;

		temp(0,1) = -aAxis.y * aAxis.x * (1-Cos) + aAxis.z * Sin;
		temp(1,1) = aAxis.y * aAxis.y * (1-Cos) + Cos;
		temp(2,1) = aAxis.y * aAxis.z * (1-Cos) - aAxis.x * Sin;

		temp(0,2) = -aAxis.x * aAxis.z * (1-Cos) - aAxis.y * Sin;
		temp(1,2) = aAxis.y * aAxis.z * (1-Cos) + aAxis.x * Sin;
		temp(2,2) = aAxis.z * aAxis.z * (1-Cos) + Cos;
		return temp;
	}

	template<class TYPE>
	Matrix33<TYPE> CreateRotationMatrix(const Quaternion<TYPE> aQuat)
	{
		TYPE degrees = acos(aQuat.w)*2.0f;
		CL::Vector3f vector;
		vector.x = aQuat.x*sin(degrees/(TYPE)2.0);
		vector.y = aQuat.y*sin(degrees/(TYPE)2.0);
		vector.z = aQuat.z*sin(degrees/(TYPE)2.0);
		vector.Normalize();
		return CreateRotationMatrix<TYPE>(CL::Vector3f(aQuat.x,aQuat.y,aQuat.z),aQuat.w);
	}

	template<class TYPE>
	Matrix44<TYPE> CreateRotationMatrix44(const Vector3<TYPE> aAxis, const float aRadian,const TYPE aW = 1)
	{
		Matrix44<TYPE> temp;
		temp.SetToIdentity();
		temp.Set33(CreateRotationMatrix(aAxis,aRadian));
		temp._44 = aW;

		return temp;
	}

	template<class TYPE>
	void CreateRotationMatrix44(const Vector3<TYPE> aAxis, const float aRadian, CL::Matrix44<TYPE>& aOutMatrix)
	{
		aOutMatrix.SetToIdentity();
		aOutMatrix.Set33(CreateRotationMatrix(aAxis,aRadian));
		aOutMatrix._44 = 1;
	}

	#define CreateRotationMatrix44f CreateRotationMatrix44<float>;
	#define CreateRotationXMatrix44f CreateRotationXMatrix44<float>
	#define CreateRotationYMatrix44f CreateRotationYMatrix44<float>
	#define CreateRotationZMatrix44f CreateRotationZMatrix44<float>
}

#endif