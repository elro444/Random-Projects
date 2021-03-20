#pragma once

#include <WinSock2.h>
#include <Windows.h>
#include <mutex>
#include <thread>
#include <queue>
#include <set>

#include "Helper.h"
#include "WSAInitializer.h"
#include "VolumeController.h"
#include "VolumeNotifier.h"
#include "User.h"

class Server : public BaseOnVolumeChangeListener
{
public:
	Server();
	~Server();

	void serve();
private:
	unsigned m_nextID;
	unsigned getNextID();

	void bindAndListen();
	void accept();
	void handleIncomingClients();
	void clientHandler(SOCKET clientSocket);


	// Handlers:
	void handleGreet(SOCKET socket, json params); // Gotta finish
	void handleDisconnect(SOCKET sock);

	void handleGetVolume();
	void handleSetVolume(json params);

	void handleGetMute();
	void handleSetMute(json params);
	void handleToggleMute();

	/*void handleGetRemaining(json params);
	void handleKillAllOthers(json params);*/
	//void handle(json params);

	// Utils:
	User* findUser(unsigned userID);
	//User* findUser(SOCKET sock);
	virtual HRESULT STDMETHODCALLTYPE onVolumeNotification(PAUDIO_VOLUME_NOTIFICATION_DATA notification);
	
	//std::queue<SOCKET> m_pendingclients;
	WSAInitializer m_wsaInitializer; // as long as this instance exists WSA is initialized.
	SOCKET m_socket;
	bool m_running;

	std::vector<User> m_users;
	std::mutex m_usersMutex;
	std::condition_variable m_usersConVar;
	unsigned m_currentUserID;

	VolumeController m_volumeController;
};

