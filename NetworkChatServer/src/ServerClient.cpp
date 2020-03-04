#include "ServerClient.h"

ServerClient::ServerClient(std::string& name, Poco::Net::SocketAddress& address, int id)
{
	name_ = name;
	address_ = address;
	ID_ = id;
	attempt_ = 0;
}