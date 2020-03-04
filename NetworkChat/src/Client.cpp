#include "Client.h"
#include <random>

#include "Poco/Net/NetException.h"

Client::Client(std::string name, std::string address, int port)
{
	name_ = name;
	address_ = address;
	toPort_ = port;  // server port no.
	//port_ = 8080; // client port no.

	// get a random port no. for client (https://stackoverflow.com/a/7560564)
	std::random_device rd;   // obtain a random number from hardware
	std::mt19937 eng(rd());  // seed the generator
	std::uniform_int_distribution<> distr(0, 65535);  // define the range
	port_ = distr(eng);

	id_ = -1;

	openConnectStatus_ = openConnection(address_, toPort_);
}

Client::~Client()
{
	if (sendthread_.joinable())
		sendthread_.join();
}

bool Client::openConnection(const std::string& address, int port)
{
	try {
		ip_ = Poco::Net::SocketAddress(address, port_);
		socket_ = Poco::Net::DatagramSocket(ip_);
		ip2_ = Poco::Net::SocketAddress(address, toPort_);
	}
	catch (Poco::Net::InvalidAddressException e) {
		errorMsg_ = e.message();
		return false;
	}
	catch (Poco::Net::InvalidSocketException e) {
		errorMsg_ = e.message();
		return false;
	}
	catch (Poco::IOException e) {
		errorMsg_ = e.message();
		return false;
	}
	std::string connection = "/c/" + name_;
	send(connection);
	return true;
}

std::string Client::receive()
{
	char data[1024];
	int n;
	try {
		n = socket_.receiveBytes(data, 1023);
		data[n] = '\0';
	}
	catch (Poco::IOException e) {
		errorMsg_ = e.message();
	}
	std::string message(data);

	return message;
}

void Client::send(const std::string& data)
{
	sendthread_ = boost::thread(&Client::sendThread, this, data);
}

void Client::sendThread(const std::string& data)
{
	try {
		socket_.sendTo(data.data(), data.size(), ip2_);
	}
	catch (Poco::IOException e) {
		errorMsg_ = e.message();
	}
}