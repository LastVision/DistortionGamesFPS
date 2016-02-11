#ifndef COMMON_LIB_ANGLE_3_HEADER
#define COMMON_LIB_ANGLE_3_HEADER

namespace CommonLib
{
	template<class Type>
	class Angle3
	{
	public:
		Angle3();
		~Angle3();

		void Set(Type heading, Type bank,Type pitch);
		CL::Vector3<Type> ToDirection();
		void FromDirection(const CL::Vector3<Type> aDirection);
		union
		{
			Type myData[3];
			struct
			{
				union
				{
					Type h;
					Type Heading;
				};
				union
				{
					Type p;
					Type myPitch;
				};
				union
				{
					Type b;
					Type myBank;
				};
			};
		};
	};
	typedef Angle3<float> Angle3f;

	template<class Type>
	Angle3<Type>::Angle3()
	{
	}

	template<class Type>
	Angle3<Type>::~Angle3()
	{
	}

	template<class Type>
	void Angle3<Type>::Set(Type heading, Type bank,Type pitch)
	{
		h = heading;
		b = bank;
		p = pitch;
	}

	template<class Type>
	CL::Vector3<Type> Angle3<Type>::ToDirection()
	{
		CL::Vector3<Type> returnVector;
		returnVector.x = sin(h)*cos(p);
		returnVector.y = -sin(p);
		returnVector.z = cos(h)*cos(p);
		return returnVector;
	}

	template<class Type>
	void Angle3<Type>::FromDirection(const CL::Vector3<Type> aDirection)
	{
		p = asin(-aDirection.y);
		h = atan2(aDirection.x,aDirection.z);		
		b = 0;
	}
};
#endif