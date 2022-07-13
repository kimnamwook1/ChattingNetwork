#define _WINSOCK_DEPRECATED_NO_WARNINGS 
#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <WinSock2.h>
#include <conio.h>

#pragma comment(lib, "ws2_32.lib")

int main()
{
	bool bRunning = true;

	WSAData wsaData;
	WSAStartup(MAKEWORD(2, 2), &wsaData);

	SOCKET ServerSocket = socket(AF_INET, SOCK_STREAM, 0);
	SOCKADDR_IN ServerAddrIn;
	memset(&ServerAddrIn, 0, sizeof(SOCKADDR_IN));
	ServerAddrIn.sin_family = AF_INET;
	ServerAddrIn.sin_port = htons(1234);
	ServerAddrIn.sin_addr.s_addr = INADDR_ANY;

	fd_set Original; //os���� Ư�� �̺�Ʈ�� �߻��ϸ� ������ �˷��� �ϴ� ����ü
	fd_set CopyReads; // ������ �ٲ��� �ʰ� �纻�� �ٲپ üũ�Ѵ�.
	timeval Timeout;
	Timeout.tv_sec = 0; // s 
	Timeout.tv_usec = 500; //ms

	FD_ZERO(&Original);
	FD_SET(ServerSocket, &Original); //���� ���Ͽ� �������� ����� ������ �˷���.

	bind(ServerSocket, (SOCKADDR*)&ServerAddrIn, sizeof(SOCKADDR_IN));

	listen(ServerSocket, 0);

	while (bRunning)
	{
		CopyReads = Original;
		//polling
		int fd_num = select(0, &CopyReads, 0, 0, &Timeout);

		if (fd_num == 0)
		{
			//another process
			continue;
		}

		if (fd_num == SOCKET_ERROR)
		{
			bRunning = false;
			break;
		}

		for (size_t i = 0; i < Original.fd_count; i++)
		{	//����� ���� ����Ʈ �߿� �̺�Ʈ�� �߻�����.
			if (FD_ISSET(Original.fd_array[i], &CopyReads)) //���� �Ǿ���?
			{
				if (Original.fd_array[i] == ServerSocket)
				{
					//���� ���Ͽ� �̺�Ʈ�� �߻���. ���� ��û
					SOCKADDR_IN ClientSockAddrIn;
					memset((char*)&ClientSockAddrIn, 0, sizeof(SOCKADDR_IN));
					int ClientSockAddrInSize = sizeof(SOCKADDR_IN);
					SOCKET ClientSocket = accept(ServerSocket, (SOCKADDR*)&ClientSockAddrIn, &ClientSockAddrInSize);
					FD_SET(ClientSocket, &Original);  //������ ������ �߰� 
					std::cout << "Connected Client : " << ClientSocket << std::endl;
				}
				else
				{
					SOCKET ClientSocket = Original.fd_array[i];
					//Client ����, Recv, Send
					char Buffer[1024] = { 0, };

					int RecvLength = recv(ClientSocket, Buffer, sizeof(Buffer), 0);

					if (RecvLength == 0) //Ŭ���̾�Ʈ�� ������ ������ ��
					{   
						std::cout << "Disconnected Client" << std::endl;
						FD_CLR(ClientSocket, &Original); //���� ����Ʈ���� ���ּ���.
						closesocket(ClientSocket);
					}
					else if (RecvLength < 0)
					{
						std::cout << "Disconnected Client By Error" << GetLastError() << std::endl;
						FD_CLR(ClientSocket, &Original); //���� ����Ʈ���� ���ּ���.
						closesocket(ClientSocket);
					}
					else
					{
						for (size_t i = 0; i < Original.fd_count; ++i)
						{
							if (Original.fd_array[i] != ServerSocket)
							{
								int SendLength = send(Original.fd_array[i], Buffer, 8, 0);
							}
						}
					}
					printf("%s\n", Buffer);
				}
			}
		}
	}
	closesocket(ServerSocket);

	WSACleanup();
	return 0;
}