#include "stdafx.h"
#include "Animation.h"
#include "AnimationProxy.h"

namespace Prism
{
	AnimationProxy::AnimationProxy()
		: myAnimation(nullptr)
	{
	}


	AnimationProxy::~AnimationProxy()
	{
		delete myAnimation;
	}
}