#include "stdafx.h"
#include "Portal.h"


namespace Prism
{
	Portal::Portal(const Room& aRoom0, const Room& aRoom1)
	{
		myRooms[0] = &aRoom0;
		myRooms[1] = &aRoom1;
	}


	Portal::~Portal()
	{
	}
}