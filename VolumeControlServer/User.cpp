#include "User.h"

User::User(unsigned ID) :
	ID(ID),
	generalSocket(INVALID_SOCKET),
	pendingSocket(INVALID_SOCKET)
	//,updatesSocket(0)
{
}

User::User(const User & other) :
	ID(other.ID),
	generalSocket(other.generalSocket),
	pendingSocket(other.pendingSocket)
{
}

User::~User()
{
	// closeSockets();
	// Dont do this because of copying..........
}

void User::closeSockets()
{
	try
	{
		::closesocket(generalSocket);
		::closesocket(pendingSocket);
		//::closesocket(updatesSocket);
	}
	catch (...)
	{}
}

bool User::operator==(const User & other) const
{
	return ID == other.ID;
}

bool User::operator<(const User & other) const
{
	return ID < other.ID;
}

User & User::operator=(const User & other)
{
	*this = User(other);
	return *this;
}
