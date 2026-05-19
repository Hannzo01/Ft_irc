#include "Bot.hpp"

void Bot::parse_input(std::string ip, std::string port, std::string password)
{
    if (ip.empty() || port.empty() || password.empty())
        throw std::logic_error("Invalid input");

    int p = atoi(port.c_str());
    for (size_t i = 0; i < port.size(); i++)
    {
        if (!isdigit(port[i]))
            throw std::logic_error("Valid ports are between 1024 and 65535");
    }
    if (p < 1024 || p > 65535)
        throw std::logic_error("Invalid port num"); 
}



int main(int argc, char *argv[])
{
    if (argc != 4){
        std::cerr << "Usage : ./ircbot <ip> <port> <passowrd>" << std::endl;
        return 1;
    }

    try{
        //parse linput
        Bot::parse_input(argv[1], argv[2], argv[3]);
        Bot hannzo(argv[1], atoi(argv[2]), argv[3]);
        hannzo.setupSocket();
    }
    catch(std::exception& e)
    {
        std::cerr << e.what() << std::endl;
    }
}