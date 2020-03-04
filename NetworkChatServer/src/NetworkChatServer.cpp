#include <iostream>
#include <string>
#include <sstream>

#include "Server.h"
#include "boost/thread.hpp"


int main(int argc, const char** argv)
{
    if (argc != 3)
        std::cout << "Usage: NetworkChatServer [ipaddress] [port] \n";

//    std::cout << "Ipaddress: " << argv[1] << std::endl;
//    std::cout << "Port: " << argv[2] << std::endl;

    std::string address = argv[1];
    std::stringstream strvalue;
    strvalue << argv[2];
    int port;
    strvalue >> port;

    Server sr(address, port);
}
