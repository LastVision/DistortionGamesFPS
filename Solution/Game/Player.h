#pragma once

class Movement;
class Shooting;
class Health;

namespace GUI
{
	class GUIManager3D;
}

namespace Prism
{
	class Camera;
}

class Player
{
public:
	Player(Prism::Scene* aScene);
	~Player();

	void Update(float aDelta);

	void Render();
	

	Prism::Camera* GetCamera() const;

private:
	Movement* myMovement;
	Shooting* myShooting;
	Health* myHealth;
	GUI::GUIManager3D* my3DGUIManager;
	Prism::Camera* myCamera;
	CU::Matrix44<float> myOrientation;
	Prism::SpriteProxy* myCrosshair;
	Prism::Instance* myModel;
	float myHeight;
	float myCrouchHeight;
	float myJumpAcceleration;
	float myJumpOffset;
	float mySendTime;
};

inline Prism::Camera* Player::GetCamera() const
{
	return myCamera;
}