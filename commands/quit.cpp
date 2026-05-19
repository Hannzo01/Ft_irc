#include "../Server.hpp"

void Server::handleQuit(Client* client)
{
    //nderha b chi var hsn

    // for (size_t i = 0; i < _pollFds.size() ; i++)
    // {
    //     if (_pollFds[i].fd == client->getFd())
    //     {
    //         _clientBuffers.erase(_pollFds[i].fd);
    //         close(_pollFds[i].fd);
    //         _pollFds.erase(_pollFds.begin() + i);
    //         break;
    //     }
    // }
    for (size_t j = 0; j < clients.size(); j++)
    {
        if (clients[j] == client)
        {
            delete clients[j];
            clients.erase(clients.begin() + j);
            break;
        }
    }
}