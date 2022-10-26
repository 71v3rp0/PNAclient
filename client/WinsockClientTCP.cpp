#include <iostream>
#include <iomanip>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include "Gumming.h"

#pragma comment(lib, "Ws2_32.lib")

int main()
{
	//прежде чем начать работу с сетью нужно загрузить необходимую версию библиотеки,иначе вызов сетевой функции вернёт ошибку
	using namespace std;

	// Initializing Winsock
	WSADATA wsaData;
	//Проверка на что?
	if (WSAStartup(MAKEWORD(2, 2), &wsaData))//wsastrartup() -функция загрузки библиотеки, makeword()-указываем версию winsock
	{
		cerr << "WSAStartup failed";
		return 1;
	}



	struct addrinfo* result = NULL;
	struct addrinfo hints =
	{
			NULL,			// flags
			AF_UNSPEC,		// family    //AF_UNSPEC - неважно ipv4 или ipv6 //AF_INET или AF_INET6
			SOCK_STREAM,	// socktype
			IPPROTO_TCP		// protocol
	};

	const unsigned bufferSize = 513;
	char dataBuffer[bufferSize];

	cout << "Enter host addresses [localhost - 127.0.0.1]: ";

	cin.get(dataBuffer, bufferSize).get();
	//getaddrinfo((имяхоста/IP-адрес), (Порт), (структура с параметрами), (result указатель на структуру с результатами)
	if (getaddrinfo(dataBuffer, "7000", &hints, &result)) // getaddrinfo("127.0.0.1", "7000", &hints, &result)
	{
		cerr << "getaddrinfo failed";
		WSACleanup();
		return 1;
	}


	// Creating a Socket for the Client

	SOCKET connectSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);

	if (connectSocket == INVALID_SOCKET)
	{
		cerr << "socket failed";
		freeaddrinfo(result);
		WSACleanup();
		return 1;
	}

	// Connecting to a Socket

	if (connect(connectSocket, result->ai_addr, result->ai_addrlen) == SOCKET_ERROR)
	{
		cerr << "connect failed";
		freeaddrinfo(result);
		closesocket(connectSocket);
		WSACleanup();
		return 1;
	}
	else
	{
		system("cls");

		cout << "Client: connected to [" << dataBuffer << "] - successful\n"
			<< "Use gamma: -g your_message\n"
			<< "Press \"Enter\" disconnect from host\n\n";

	}

	freeaddrinfo(result);

	// Sending and Receiving Data on the Client

	int gammaBuffer[bufferSize - 1];
	int res;

	do
	{
		// Sending a message

		cout << "Client: ";

		cin.get(dataBuffer, bufferSize);
		res = strlen(dataBuffer);

		while (res && (char)cin.get() != '\n');

		if (dataBuffer[0] == '-' && dataBuffer[1] == 'g' && dataBuffer[2] == ' ')
		{
			// Generating and sending gamma
			createGamma(gammaBuffer, res);

			// Encrypt message
			encryption(dataBuffer, gammaBuffer, res);
		}

		if (send(connectSocket, dataBuffer, res + (bool)res, 0) == SOCKET_ERROR)
		{
			cerr << "send failed";
			closesocket(connectSocket);
			WSACleanup();
			return 1;
		}

		if (dataBuffer[0] == '-' && dataBuffer[1] == 'g' && dataBuffer[2] == ' ')
		{
			// Send key

			if (send(connectSocket, (char*)gammaBuffer, (bufferSize - 1) * sizeof(int), 0) == SOCKET_ERROR)
			{
				cerr << "send failed";
				closesocket(connectSocket);
				WSACleanup();
				return 1;
			}
		}

		if (res == 0)
		{
			cout << "\nClient: connection closed...\n";

			break;
		}

		// Receiving a message

		res = recv(connectSocket, dataBuffer, bufferSize, 0);

		if (res > 0)
		{
			cout << "Server: " << dataBuffer << endl << endl;
		}
		else
		{
			cerr << "recv failed";
			closesocket(connectSocket);
			WSACleanup();
			return 1;
		}

	} while (res > 0);

	// Disconnecting the Client

	if (shutdown(connectSocket, SD_SEND))
	{
		cerr << "shutdown failed";
		closesocket(connectSocket);
		WSACleanup();
		return 1;
	}

	closesocket(connectSocket);
	WSACleanup();
	system("pause");

	return 0;
}
