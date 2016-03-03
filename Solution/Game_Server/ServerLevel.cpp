#include "stdafx.h"
#include "ServerLevel.h"

#include <iostream>

ServerLevel::ServerLevel()
{
}

ServerLevel::~ServerLevel()
{
}

void ServerLevel::Update(const float aDeltaTime)
{
	myTestTimer -= aDeltaTime;
	if (myTestTimer <= 0.f)
	{
		myTestTimer = 1.f;
		std::cout << "OMG a level in the server!!!!111" << std::endl;
	}
}