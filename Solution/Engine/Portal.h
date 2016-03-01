#pragma once

namespace Prism
{
	class Room;

	class Portal
	{
	public:
		Portal(const Room& aRoom0, const Room& aRoom1);
		~Portal();

	private:
		const Room* myRooms[2];
	};
}