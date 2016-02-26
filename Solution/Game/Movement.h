#pragma once
#include <Quaternion.h>

class XMLReader;

namespace tinyxml2
{
	class XMLElement;
}

class Movement
{
public:
	Movement(CU::Matrix44<float>& anOrientation, XMLReader& aReader, tinyxml2::XMLElement* anElement);
	~Movement();

	void Update(float aDelta);
private:
	void operator=(Movement&) = delete;
	void Move(float aDelta);
	void Rotate();

	CU::Matrix44<float>& myOrientation;
	CU::Vector2<float> myCursorPosition;
	float myRotationSpeed;
	float mySpeed;
	float mySprintMultiplier;

	CU::Quaternion myPitch;
	CU::Quaternion myYaw;
};

