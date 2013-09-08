#include "SocketUtils.h"

std::string GetSocketIpAddressString(const sockaddr_in& socketAddress)
{
#if defined(_WIN32)
	DWORD addressStringLength = 256;
	std::string result;
	while(1)
	{
		result.resize(addressStringLength);
		int error = WSAAddressToStringA(reinterpret_cast<sockaddr*>(const_cast<sockaddr_in*>(&socketAddress)), sizeof(sockaddr_in), 
			NULL, const_cast<char*>(result.data()), &addressStringLength);
		if(error == 0)
		{
			break;
		}
		else
		{
			error = WSAGetLastError();
			if(error == WSAEFAULT && (result.size() != addressStringLength))
			{
				//Size already has been updated
				continue;
			}
			else
			{
				result.clear();
				break;
			}
		}
	}
	return result;
#elif defined(__unix__)
	std::string result;
	socklen_t addressStringLength = INET_ADDRSTRLEN;
	while(1)
	{
		result.resize(addressStringLength);
		const char* error = inet_ntop(socketAddress->sin_family, socketAddress->sin_addr, const_cast<char*>(result.data()), &addressStringLength);
		if(error != NULL)
		{
			break;
		}
		else
		{
			if(errno == ENOSPC)
			{
				addressStringLength *= 2;
				continue;
			}
			else
			{
				result.clear();
				break;
			}
		}
	}
	return result;
#else
	return "(Not implemented)";
#endif
}
