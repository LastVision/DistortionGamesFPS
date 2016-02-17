#pragma once
#include "Component.h"
#include <Matrix.h>

namespace Prism
{
	class Camera;
}

class PlayerComponent
{
public:
	PlayerComponent();
	~PlayerComponent();

	void Update(float aDelta);

	Prism::Camera* myCamera;
private:
	CU::Matrix44<float> myOrientation;
	CU::Matrix44<float> myPitch;
	CU::Matrix44<float> myYaw;
	CU::Vector2<float> myMousePosition;
};