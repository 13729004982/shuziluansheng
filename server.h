#ifndef SERVER_H
#define SERVER_H
#ifdef _WIN32
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#pragma comment(lib,"ws2_32")
#elif __linux__
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#endif
#include "messageParser.h"
#include <memory>

#define DEFAULT_BUFLEN 512
#define DEFAULT_PORT 27015
#define MAX_CLIENTS 80

using std::shared_ptr;

enum class serverInitStatus
{
	SUCCESS,
	NETWORK_INIT_FAILED,
	SOCKET_INIT_FAILED,
	BIND_FAILED,
	LISTEN_FAILED
};

class server
{
private:
	#ifdef _WIN32
	SOCKET m_socket;
	#elif __linux__
	int m_socket;
	#endif
	shared_ptr<messageParser> m_parser;
public:
	server();
	~server();
	serverInitStatus init();
	void start();
	void stop();
};

#endif
