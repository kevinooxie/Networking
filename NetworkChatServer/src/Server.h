#pragma once

#include <string>

#include "Poco/Net/SocketAddress.h"
#include "Poco/Net/DatagramSocket.h"
#include "boost/thread.hpp"

#include "ServerClient.h"

class Server {

public:
	Server(std::string ipaddress, int port);
	~Server();

	void run();

private:
	std::string address_;
	int port_;
	bool running_;
	std::string errorMsg_;

	bool rawMode_;

	Poco::Net::SocketAddress ip_;
	Poco::Net::DatagramSocket socket_;
	boost::thread run_, manage_, receive_, send_;

	std::vector<ServerClient> clientlist_;
	std::vector<int> clientResponse_;

	bool setup();

	void manageClients();
	void manageClientsThread();

	void receive();
	void receiveThread();

	void process(std::string packet, Poco::Net::SocketAddress sender);

	void sendMessage(const std::string& message, Poco::Net::SocketAddress receiver);
	void send(const std::string& message, Poco::Net::SocketAddress receiver);
	void sendThread(const std::string& message, Poco::Net::SocketAddress receiver);
	void sendToAll(std::string message);

	void disconnect(int id, bool status);

	void sendStatus();

	void quit();

	void printHelp();
};