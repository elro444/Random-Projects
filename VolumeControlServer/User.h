#pragma once
#include <WinSock2.h>

struct User
{
	const unsigned ID;
	SOCKET generalSocket; // Used for general purpose.. Basically the protocol will use this always
	SOCKET pendingSocket; // Used for when the generalSocket waits for 'itsYourTurn' message.
	//SOCKET updatesSocket; // Used for the server to send updates to the client, like updating the volume (because someone changed it on the computer)

	User(unsigned ID);
	User(const User& other);
	~User();
	void closeSockets();
	bool operator==(const User& other) const;
	bool operator<(const User& other) const;
	User& operator=(const User& other);
};

