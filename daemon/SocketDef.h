#pragma once

#ifdef WIN32
#include <WinSock2.h>
#else
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
typedef int SOCKET;
#endif
