#pragma once
#include <string>
#include "asio.hpp"
class Client
{
public:
	Client();
	virtual ~Client();
	static void Echo(const std::string, const asio::ip::address_v4, uint16_t);
	void SendText(const std::string, const asio::ip::address_v4, uint16_t);
	bool RecvFile(const std::string, const std::string, const asio::ip::address_v4, const int unsignedshort);
	bool SendFile(const std::string, const std::string, const asio::ip::address_v4, const int port);
};
