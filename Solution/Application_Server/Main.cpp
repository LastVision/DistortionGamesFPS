#include "stdafx.h"
#include <NetworkManager.h>
void main()
{
	NetworkManager::Create(true);
	NetworkManager::GetInstance()->StartNetwork();
	while (true)
	{





	}
	NetworkManager::Destroy();
}