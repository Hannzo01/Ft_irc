#include "../Server.hpp"

void Server::handlePing(Client* client, std::string param)
{
    std::string buff = "PONG :" + param + "\r\n";
    if (send(client->getFd(),  buff.c_str(), buff.length(), 0) < 0)
        throw std::runtime_error("send failed");

}