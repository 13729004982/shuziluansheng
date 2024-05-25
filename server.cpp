#include "server.h"

server::server():m_parser(messageParser::getInstance())
{
	init();
	//do log

}

server::~server()
{

}

serverInitStatus server::init()
{
	#ifdef _WIN32
	WSADATA wsaData;
	int iResult;
	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0)
	{
		return serverInitStatus::NETWORK_INIT_FAILED;
	}
	m_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (m_socket == INVALID_SOCKET)
	{
		WSACleanup();
		return serverInitStatus::SOCKET_INIT_FAILED;
	}
	#elif __linux__
	m_socket = socket(AF_INET, SOCK_STREAM, 0);
	if (m_socket == -1)
	{
		return serverInitStatus::SOCKET_INIT_FAILED;
	}
	#endif
	sockaddr_in service;
	service.sin_family = AF_INET;
	service.sin_addr.s_addr = inet_addr("");
	service.sin_port = htons(DEFAULT_PORT);
	if (bind(m_socket, (SOCKADDR*)&service, sizeof(service)) == SOCKET_ERROR)
	{
#ifdef _WIN32
		closesocket(m_socket);
		WSACleanup();
#elif __linux__
		close(m_socket);
#endif
		return serverInitStatus::BIND_FAILED;
	}
	if (listen(m_socket, SOMAXCONN) == SOCKET_ERROR)
	{
#ifdef _WIN32
		closesocket(m_socket);
		WSACleanup();
#elif __linux__
		close(m_socket);
#endif
		return serverInitStatus::LISTEN_FAILED;
	}
	return serverInitStatus::SUCCESS;
}

void server::start()
{
	int pid, n, i;
	struct sockaddr_in clientaddr;
	socklen_t clientlen;
	int connfd;
	char buf[MAX_CLIENTS];
	char str[INET_ADDRSTRLEN];
	while (true)
	{
		clientlen = sizeof(clientaddr);
		connfd = accept(m_socket, (struct sockaddr*)&clientaddr, &clientlen);
#ifdef __linux__
		if ((pid = fork()) == 0)
		{
			close(m_socket);
			inet_ntop(AF_INET, &clientaddr.sin_addr, str, sizeof(str));
			printf("server: got connection from %s\n", str);
			n = recv(connfd, buf, MAX_CLIENTS, 0);
			buf[n] = '\0';
			printf("server: received %s\n", buf);
			m_parser->addOri(buf);
			exit(0);
		}
		close(connfd);
#elif _WIN32
		if (connfd == INVALID_SOCKET)
		{
			closesocket(m_socket);
			WSACleanup();
			return;
		}
		inet_ntop(AF_INET, &clientaddr.sin_addr, str, sizeof(str));
		printf("server: got connection from %s\n", str);
		n = recv(connfd, buf, MAX_CLIENTS, 0);
		buf[n] = '\0';
		printf("server: received %s\n", buf);
		m_parser->addOri(buf);
		closesocket(connfd);
#endif
	}
}
