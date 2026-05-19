#include "../Server.hpp"

void Server::handlePing(Client* client, std::string param)
{
    std::string buff = "PONG :" + param + "\r\n";
    send(client->getFd(),  buff.c_str(), buff.length(), 0);
    std::cout << "Done eft pong hehe" << std::endl;
}