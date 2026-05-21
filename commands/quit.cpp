#include "../Server.hpp"


void Server::handleQuit(Client* client, size_t &i)
{
    int fd = client->getFd();
    // Remove from all channels first
    const std::vector<Channel*> chans = client->getChannels();
    for (size_t j = 0; j < chans.size(); j++) {
        chans[j]->broadcast(":" + client->getPrefix() + " QUIT :Quit\r\n");
        chans[j]->removeMember(client);
        chans[j]->removeOperator(client);
        if (chans[j]->getMembers().empty())
            removeChannel(chans[j]->getName());
    }
    disconnectClient(i, fd);
}
