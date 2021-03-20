#include "Server.h"
#include <iostream>
#include <WS2tcpip.h>


int main()
{
	Server server;
	server.serve();

	//WSAInitializer wsa;
	//SOCKET sock = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	//if (sock == INVALID_SOCKET)
	//{
	//	std::cout << "wsaError: " << WSAGetLastError() << std::endl;
	//}
	//std::cout << "sock: " << sock << std::endl;
	//sockaddr_in clientService;
	//clientService.sin_family = AF_INET;
	////clientService.sin_addr.s_addr = inet_addr("127.0.0.1");
	//inet_pton(AF_INET, "127.0.0.1", &clientService.sin_addr);
	//clientService.sin_port = htons(1234);

	//int res = ::connect(sock, (sockaddr*)&clientService, sizeof(clientService));
	//std::cout << "connect: " << res << std::endl;
	//char buffer[1024] = { 0 };
	//res = ::recv(sock, buffer, sizeof(buffer), 0);
	//std::cout << "recv: " << res << std::endl << "buffer: " << buffer << std::endl;
	//const char* message = "A cool\0 message";
	//res = ::send(sock, message, 15, 0);
	//std::cout << "send: " << res << std::endl;
	//Sleep(1000);
	//getchar();
	return 0;
}