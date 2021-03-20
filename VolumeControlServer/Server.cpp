#include "Server.h"



Server::Server() :
	m_running(true),
	m_nextID(1)
{
	m_socket = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (m_socket == INVALID_SOCKET)
		throw std::exception("Failed to create socket!");
}


Server::~Server()
{
	m_running = false; // just in case :P
	try
	{
		::closesocket(m_socket);
	}
	catch (...)
	{
	}
}

void Server::serve()
{
	bindAndListen();

	std::thread t(&Server::handleIncomingClients, this);
	t.detach();

	while (true)
	{
		accept();
	}
}

HRESULT Server::onVolumeNotification(PAUDIO_VOLUME_NOTIFICATION_DATA notification)
{
	if (notification == NULL)
	{
		return E_INVALIDARG;
	}
	if (notification->guidEventContext != m_context)
	{

	}
	return S_OK;
}

unsigned Server::getNextID()
{
	return m_nextID++;
}

void Server::bindAndListen()
{
	u_short port = 4444;
	struct sockaddr_in sa = { 0 };
	sa.sin_port = htons(port);
	sa.sin_family = AF_INET;
	sa.sin_addr.s_addr = 0;
	// again stepping out to the global namespace
	if (::bind(m_socket, (struct sockaddr*)&sa, sizeof(sa)) == SOCKET_ERROR)
		throw std::exception(__FUNCTION__ " - bind");
	if (::listen(m_socket, SOMAXCONN) == SOCKET_ERROR)
		throw std::exception(__FUNCTION__ " - listen");
}

void Server::accept()
{
	SOCKET client_socket = ::accept(m_socket, NULL, NULL);
	if (client_socket == INVALID_SOCKET)
	{
		printf("There was an error while accepting a client. Continuing..\n");
		return;
	}

	json params;
	MessageCode code;
	Helper::getMessage(client_socket, code, params);
	if (code == MessageCode::Greet)
	{
		printf("Greetings %d!\n", client_socket);
		handleGreet(client_socket, params);
	}
	else
	{
		//Some error. shouldn't happen
		try
		{
			::closesocket(client_socket);
		} catch (...) {}
	}

	//std::unique_lock<std::mutex> lock(m_pendingClientsMutex);
	//m_pendingclients.push(client_socket);
	//lock.unlock();
	//m_conVarPendingClients.notify_all();
}

void Server::handleIncomingClients()
{
	while (m_running)
	{
		std::unique_lock<std::mutex> lock(m_usersMutex);
		if (m_users.empty())
			m_usersConVar.wait(lock);
		if (m_users.empty())
			continue; // for the undefined behavior

		SOCKET client = m_users[0].generalSocket;
		lock.unlock();

		clientHandler(client);
	}
}

void Server::clientHandler(SOCKET clientSocket)
{
	MessageCode code;
	json params;
	do
	{
		Helper::getMessage(clientSocket, code, params);
		switch (code)
		{
		case MessageCode::GetVolume:
			handleGetVolume();
			break;
		case MessageCode::SetVolume:
			handleSetVolume(params);
			break;
		case MessageCode::GetMute:
			handleGetMute();
			break;
		case MessageCode::SetMute:
			handleSetMute(params);
			break;
		case MessageCode::ToggleMute:
			handleToggleMute();
			break;
		}
	} while (code != MessageCode::Disconnet && code != MessageCode::Unknown);
	handleDisconnect(clientSocket);
}

void Server::handleGreet(SOCKET sock, json params)
{
	enum class GreetMode
	{ // See struct User
		General = 0,
		//Pending,
		//Updates
	};

	GreetMode mode = Helper::getWithDefault(params, "mode", GreetMode::General);
	/*unsigned id = Helper::getWithDefault(params, "id", 0U);

	User* existingUser = findUser(id);
	if (!existingUser)
		mode = GreetMode::General;*/

	switch (mode)
	{
	case GreetMode::General:
	{
		User user(getNextID());
		user.generalSocket = sock;

		std::unique_lock<std::mutex> lock(m_usersMutex);
		m_users.push_back(user);
		lock.unlock();
		m_usersConVar.notify_all();
		break;
	}
	/*case GreetMode::Pending:
	{ // Coming soon
		existingUser->pendingSocket = sock;
		break;
	}*/
	/*case GreetMode::Updates:
		break;*/
	}
	Helper::sendMessage(sock, MessageCode::GreetRes, json{ std::make_pair("res",0) });
}

void Server::handleDisconnect(SOCKET sock)
{
	
	for (unsigned i = 0; i < m_users.size(); i++)
	{
		if (m_users[i].generalSocket == sock)
		{
			m_users[i].closeSockets(); // Clean up
			m_users.erase(m_users.begin() + i);
			break;
		}
		else if (m_users[i].pendingSocket == sock)
		{
			m_users[i].pendingSocket = INVALID_SOCKET;
			::closesocket(sock);
		}
	}
}

void Server::handleGetVolume()
{
	int volume = m_volumeController.getMasterVolume();
	Helper::sendMessage(m_users[0].generalSocket, MessageCode::GetVolumeRes, json{ std::make_pair("volume",volume) });
}

void Server::handleSetVolume(json params)
{
	int volume = Helper::getWithDefault(params, "volume", 0);
	m_volumeController.setMasterVolume(volume);
}

void Server::handleGetMute()
{
	bool mute = m_volumeController.getMute();
	Helper::sendMessage(m_users[0].generalSocket, MessageCode::GetMuteRes, json{ std::make_pair("mute", mute) });
}

void Server::handleSetMute(json params)
{
	bool mute = Helper::getWithDefault(params, "mute", false);
	m_volumeController.setMute(mute);
}

void Server::handleToggleMute()
{
	m_volumeController.toggleMute();
}

User* Server::findUser(unsigned userID)
{
	if (!userID)
		return NULL;
	for (User &user : m_users)
	{
		if (user.ID == userID)
			return &user;
	}
	return NULL;
}
//
//User * Server::findUser(SOCKET sock)
//{
//	for (User& user : m_users)
//	{
//		if (user.generalSocket == sock || user.pendingSocket == sock)
//			return &user;
//	}
//	return NULL;
//}
