#include "stdafx.h"

#include <Camera.h>
#include "Movement.h"
#include "Player.h"
#include <XMLReader.h>


Player::Player()
{
	XMLReader reader;
	reader.OpenDocument("Data/Setting/SET_player.xml");

	tinyxml2::XMLElement* element = reader.ForceFindFirstChild("root");

	CU::Vector3<float> eyePosition;
	reader.ForceReadAttribute(reader.ForceFindFirstChild(element, "eyeHeight"), "value", eyePosition.y);

	tinyxml2::XMLElement* movementElement = reader.ForceFindFirstChild(element, "movement");

	myOrientation.SetPos(eyePosition);
	myCamera = new Prism::Camera(myOrientation);
	myMovement = new Movement(myOrientation, reader, movementElement);

	reader.CloseDocument();
}


Player::~Player()
{
}

void Player::Update(float aDelta)
{
	myMovement->Update(aDelta);

	CU::Vector3<float> playerPos(myOrientation.GetPos());
	DEBUG_PRINT(playerPos);

	myCamera->Update(aDelta);
}

