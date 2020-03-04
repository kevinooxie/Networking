#pragma once

#include <string>

#include "Poco/Net/SocketAddress.h"
#include "Poco/Net/DatagramSocket.h"
#include "boost/thread.hpp"

class Client
{
public:
	
	Client(std::string name, std::string address, int port);
	~Client();

	std::string name() { return name_; }

	std::string address() { return address_; }

	std::string message() { return errorMsg_; }

	int port() { return port_; }

	bool openStatus() { return openConnectStatus_; }

	void setId(int id) { id_ = id; }
	int id() { return id_; }

	void send(const std::string& data); 

	// receive packets
	std::string receive();

	void close() { socket_.close(); }

private:
	std::string name_, address_;
	int port_, toPort_;
	bool openConnectStatus_;
	std::string errorMsg_;
	int id_;

	Poco::Net::SocketAddress ip_, ip2_;
	Poco::Net::DatagramSocket socket_;

	boost::thread sendthread_;

	// open connection
	bool openConnection(const std::string& address, int port);

	// send packets
	void sendThread(const std::string& data);
};