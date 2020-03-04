#pragma once

#include <string>

#include "Poco/Net/SocketAddress.h"

class ServerClient
{
public:
	ServerClient(std::string& name, Poco::Net::SocketAddress& address, int id);
	~ServerClient() {}

	std::string name_;
	Poco::Net::SocketAddress address_;  // contain "host::port" info
	int ID_;
	int attempt_;
};