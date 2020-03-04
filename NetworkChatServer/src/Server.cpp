#include "Server.h"
#include "ServerClient.h"
#include "UniqueIdentifier.h"
#include <iostream>
#include "Poco/Net/NetException.h"


Server::Server(std::string ipaddress, int port)
{
	address_ = ipaddress;
	port_ = port;
	running_ = false;
	rawMode_ = false;

	setup();

	run_ = boost::thread(&Server::run, this);
}

Server::~Server()
{
	if (run_.joinable())
		run_.join();
	if (manage_.joinable())
		manage_.join();
	if (receive_.joinable())
		receive_.join();
	if (send_.joinable())
		send_.join();
}

bool Server::setup()
{
	try {
		ip_ = Poco::Net::SocketAddress(address_, port_);
		socket_ = Poco::Net::DatagramSocket(ip_);
	}
	catch (Poco::IOException e) {
		errorMsg_ = e.message();
		std::cout << "Exception at setup: " << errorMsg_ << std::endl;
		return false;
	}
	return true;
}

void Server::run()
{
	running_ = true;
	std::cout << "Server started on " << address_ << "::" << port_ << std::endl;
	manageClients();
	receive();
	
	while (running_) {
		std::string text;
		std::getline(std::cin, text);
		if (text.size() != 0) {
			if (text[0] != '/') {
				sendToAll("/m/Server: " + text);
				continue;
			}

			text = text.substr(1);
			if (text == "raw") {
				if (rawMode_)
					std::cout << "Raw mode off." << std::endl;
				else
					std::cout << "Raw mode on." << std::endl;
				rawMode_ = !rawMode_;
			}
			else if (text == "clients") {
				std::cout << "Clients: " << std::endl;
				std::cout << "===============" << std::endl;
				for (int i = 0; i < clientlist_.size(); ++i) {
					std::cout << clientlist_[i].name_ + "(" + std::to_string(clientlist_[i].ID_)
						+ "): " + clientlist_[i].address_.toString() << std::endl;
				}
				std::cout << "===============" << std::endl;
			}
			else if (text.substr(0,4) == "kick" && text.size()>5) { // syntax:  /kick someone  or   /kick 8920
				std::string name = text.substr(5);  // omit the space
				
				bool isnum = false;     //  https://stackoverflow.com/a/4654718
				std::string::const_iterator it = name.begin();
				while (it != name.end() && std::isdigit(*it)) ++it;
				if (it == name.end()) isnum = true;
				
				if (isnum) {
					bool existed = false;
					int id = std::stoi(name);
					for (int i = 0; i < clientlist_.size(); ++i) {
						if (clientlist_[i].ID_ == id) {
							existed = true;
							break;
						}
					}
					if (existed) disconnect(id, true);
					else std::cout << "Client with id: " << id << " doesn't exist! Check ID number." << std::endl;
				}
				else {
					for (int i = 0; i < clientlist_.size(); ++i) {
						if (clientlist_[i].name_ == name) {
							disconnect(clientlist_[i].ID_, true);
							break;
						}
					}
				}
			}
			else if (text == "quit") {
				quit();
			}
			else if (text == "help") {
				printHelp();
			}
			else {
				std::cout << "Unknow command." << std::endl;
				printHelp();
			}
		}
	}
}

void Server::printHelp()
{
	std::cout << "Here is a list of all available commands:" << std::endl;
	std::cout << "============================================" << std::endl;
	std::cout << "/raw - enables raw mode." << std::endl;
	std::cout << "/clients - show all connected clients." << std::endl;
	std::cout << "/kick [users ID or username] - kicks a user." << std::endl;
	std::cout << "/help - show this help message" << std::endl;
	std::cout << "/quit - quit the server." << std::endl;
	std::cout << "============================================" << std::endl;
}

void Server::manageClients()
{
	manage_ = boost::thread(&Server::manageClientsThread, this);
}

void Server::manageClientsThread()
{
	while (running_) {
		sendToAll("/i/Server");  // ping info
		sendStatus();
		Sleep(2000);  // wait for a moment
		for (int i = 0; i < clientlist_.size(); ++i) {
			if (std::find(clientResponse_.begin(), clientResponse_.end(), clientlist_[i].ID_) == clientResponse_.end()) {
				if (clientlist_[i].attempt_ >= 5) {   // MAX_ATTEMPTS == 5
					disconnect(clientlist_[i].ID_, false);
				}
				else {
					clientlist_[i].attempt_++;
				}
			}
			else {
				std::vector<int>::iterator it;
				for (it = clientResponse_.begin(); it != clientResponse_.end(); ++it) {
					if ((*it) == clientlist_[i].ID_) {
						clientResponse_.erase(it);
						break;
					}
				}
				clientlist_[i].attempt_ = 0;
			}
		}
	}
}

void Server::receive()
{
	receive_ = boost::thread(&Server::receiveThread, this);
}

void Server::receiveThread()
{
	while (running_) {
		char data[1024];
		Poco::Net::SocketAddress sender;
		int n;
		try {
			//socket_.receiveBytes(data, 1024);
			n = socket_.receiveFrom(data, 1024, sender);
			//std::cout << "received bytes: " << n << std::endl;
			data[n] = '\0';
		}
		catch (Poco::Net::InvalidSocketException e) {
		}
		catch (Poco::IOException e) {
			errorMsg_ = e.message();
			std::cout << "Exception at receive: " << errorMsg_ << std::endl;
		}
		//std::cout << sender.host() << ":" << sender.port() << std::endl;
		std::string message(data);
		if (rawMode_)
			std::cout << message << std::endl;
		process(message, sender);
	}
}

void Server::process(std::string packet, Poco::Net::SocketAddress sender)
{
	std::string prefix;
	if (packet.size() >= 3) 
	{
		prefix = packet.substr(0,3);
		if (prefix == "/c/")   // connection packet
		{
			std::string clientname = packet.substr(3);
			int id = UniqueIdentifier::getInstance()->id();
			ServerClient client(clientname, sender, id);
			clientlist_.push_back(client);
			std::cout << "clienname: " << clientname << std::endl;
			std::cout << "id: " << id << std::endl;
			std::string ID = "/c/" + std::to_string(id);
			sendMessage(ID, sender); // reply to client
		}
		else if (prefix == "/m/") 
		{
			//std::cout << "received message: " << packet << std::endl;
			sendToAll(packet);
		}
		else if (prefix == "/d/")
		{
			int endpos = packet.find("/e/");  // locate the suffix
			std::string id(packet, 3, endpos - 3);
			disconnect(std::stoi(id), true);
		}
		else if (prefix == "/i/") {
			int endpos = packet.find("/e/");
			std::string id(packet, 3, endpos - 3);
			clientResponse_.push_back(std::stoi(id));
		}
		else {
			std::cout << packet << std::endl;
		}
	}
	else {
		std::cout << packet << std::endl;
	}
}

void Server::sendMessage(const std::string& message, Poco::Net::SocketAddress receiver)
{
	std::string out = message + "/e/";    // add suffix to the message
	send(out, receiver);
}

void Server::send(const std::string& message, Poco::Net::SocketAddress receiver)
{
	send_ = boost::thread(&Server::sendThread, this, message, receiver);
}

void Server::sendThread(const std::string& message, Poco::Net::SocketAddress receiver)
{
	try {
		socket_.sendTo(message.data(), message.size(), receiver);
	}
	catch (Poco::IOException e) {
		errorMsg_ = e.message();
		std::cout << "Exception at send: " << errorMsg_ << std::endl;
	}
}

void Server::sendToAll(std::string message)
{
	if (rawMode_)
		std::cout << message << std::endl;

	for (int i = 0; i < clientlist_.size(); ++i)
	{
		sendMessage(message, clientlist_[i].address_);
	}
}

void Server::disconnect(int id, bool status)
{
	std::string name = "";
	Poco::Net::SocketAddress ip;
	bool existed = false;
	std::vector<ServerClient>::iterator it;
	for (it = clientlist_.begin(); it != clientlist_.end(); ++it) {
		if ((*it).ID_ == id) {
			name = (*it).name_;
			ip = (*it).address_;
			clientlist_.erase(it);
			existed = true;
			break;
		}
	}
	if (!existed) return;
	std::string message = "";
	if (status) {
		message = "Client " + name + " (" + std::to_string(id) + ") @ " + ip.toString() + " disconnected.";
	}
	else {
		message = "Client " + name + " (" + std::to_string(id) + ") @ " + ip.toString() + " timed out.";
	}
	std::cout << message << std::endl;
}

void Server::sendStatus()
{
	if (clientlist_.size() <= 0) return;
	std::string users = "/u/";
	for (int i = 0; i < clientlist_.size() - 1; ++i) {
		users += clientlist_[i].name_ + ",";
	}
	users += clientlist_[clientlist_.size() - 1].name_;
	sendToAll(users);
}

void Server::quit()
{
	for (int i = 0; i < clientlist_.size(); ++i) {
		disconnect(clientlist_[i].ID_, true);
	}
	running_ = false;
	//socket_.close();   // will cause exeption
}