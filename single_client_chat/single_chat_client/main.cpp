#include <iostream>
#include <WS2tcpip.h>

#pragma comment (lib, "ws2_32.lib")

void main()
{
	// Initialize WinSock
	WSADATA wsData;
	WORD ver = MAKEWORD(2, 2);

	int wsOK = WSAStartup(ver, &wsData);

	if (wsOK != 0)
	{
		std::cerr << "Can't initialize WinSock!!! Quitting" << std::endl;
		return;
	}
	// Create a socket
	SOCKET listening = socket(AF_INET, SOCK_STREAM, 0);

	if (listening == INVALID_SOCKET)
	{
		std::cerr << "Can't create socket! Quitting" << std::endl;
		return;
	}

	// Bing socket to IP address and port
	sockaddr_in hint;
	hint.sin_family = AF_INET;
	hint.sin_port = htons(9001);
	hint.sin_addr.S_un.S_addr = INADDR_ANY;	// Could also use inet_pton

	bind(listening, (sockaddr*)&hint, sizeof(hint));

	// Tell winsock the sock is for listening
	listen(listening, SOMAXCONN);

	// Wait for connection
	sockaddr_in client;
	int clientSize = sizeof(client);

	SOCKET clientSocket = accept(listening, (sockaddr*)&client, &clientSize);

	char host[NI_MAXHOST];		// client's remote address
	char service[NI_MAXSERV];	// Service (port) the client is connected to

	ZeroMemory(host, NI_MAXHOST);
	ZeroMemory(service, NI_MAXSERV);

	if (getnameinfo((sockaddr*)&client, sizeof(client), host, NI_MAXHOST, service, NI_MAXSERV, 0) == 0)
	{
		std::cout << host << "connected on port " << service << std::endl;
	}
	else
	{
		inet_ntop(AF_INET, &client.sin_addr, host, NI_MAXHOST);
		std::cout << host << "connected on port " << ntohs(client.sin_port) << std::endl;
	}

	// Close listening sock
	closesocket(listening);

	// While loop accept and echo message back to client
	char buf[4096];

	while (true)
	{
		ZeroMemory(buf, 4096);

		// Wait for client to send message
		int bytesReceived = recv(clientSocket, buf, 4096, 0);
		if (bytesReceived == SOCKET_ERROR)
		{
			std::cerr << "Error in recv(). Quitting" << std::endl;
			break;
		}
		if (bytesReceived == 0) {
			std::cout << "Client disconnected" << std::endl;
			break;
		}

		// Send message back to client
		send(clientSocket, buf, bytesReceived + 1, 0);
	}
	// Close the socket
	closesocket(clientSocket);
	// Cleanup WinSock
	WSACleanup();
}

// https://www.youtube.com/watch?v=WDn-htpBlnU
// PCs are little endian
// Networking is big endian