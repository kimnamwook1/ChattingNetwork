#define _WINSOCK_DEPRECATED_NO_WARNINGS 
#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <WinSock2.h>
#include <conio.h>
#include <string>

using namespace std;

#pragma comment(lib, "ws2_32.lib")

int main()
{
	WSAData wsaData;
	WSAStartup(MAKEWORD(2, 2), &wsaData);

	SOCKET ServerSocket = socket(AF_INET, SOCK_STREAM, 0);
	SOCKADDR_IN ServerAddrIn;
	memset(&ServerAddrIn, 0, sizeof(SOCKADDR_IN));
	//char* p = (char*)&ServerAddrIn;
	ServerAddrIn.sin_family = AF_INET;
	ServerAddrIn.sin_port = htons(1234);
	ServerAddrIn.sin_addr.s_addr = inet_addr("127.0.0.1");

	connect(ServerSocket, (SOCKADDR*)&ServerAddrIn, sizeof(SOCKADDR_IN));

	cout << "Please Type your Message." << endl;

	bool bRunning = true;
	while (bRunning)
	{
		char SendMessage[] = "";
		cin >> SendMessage;
 		char Buffer[1024] = { 0, };

		int TotalRecvLength = 0;

		int SendLength = send(ServerSocket, SendMessage, strlen(SendMessage), 0);

		do
		{
			int RecvLength = recv(ServerSocket, &Buffer[TotalRecvLength], sizeof(Buffer) - TotalRecvLength, 0);
			if (RecvLength == 0)
			{
				//연결종료
				std::cout << "Disconnected Client" << std::endl;
				bRunning = false;
				break;
			}
			else if (RecvLength < 0)
			{
				//error
				std::cout << "Disconnected Client By Error" << GetLastError() << std::endl;
				bRunning = false;
				break;
			}

			TotalRecvLength += RecvLength;

		} while (TotalRecvLength < sizeof(Buffer));
		printf("%s\n", Buffer);
	}
	
	closesocket(ServerSocket);
	
	WSACleanup();

	return 0;
}