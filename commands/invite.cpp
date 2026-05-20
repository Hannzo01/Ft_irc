#include "../Server.hpp"

///* INVITE <nickname> <#channel>*///

void Server::handleInvite(Client* client, std::string param)
{
    std::istringstream iss(param);
    std::string nick, channelName;
    iss >> nick >> channelName;

    for (size_t i = 0; i < channelName.size(); i++)
        channelName[i] = std::tolower(channelName[i]);
    if (nick.empty() || channelName.empty()) {
        client->sendRaw(":localhost 461 " + client->getNick() + " INVITE :Not enough parameters\r\n");
        return;
    }

    if (channelName[0] != '#') {
        client->sendRaw(":localhost 403 " + client->getNick() + " " + channelName + " :No such channel\r\n");
        return;
    }
    std::string origChanHash = channelName; // save "#chan" for reply
    channelName = channelName.substr(1);

    Channel* channel = getChannel(channelName);
    if (!channel) {
        client->sendRaw(":localhost 403 " + client->getNick() + " " + origChanHash + " :No such channel\r\n");
        return;
    }

    if (!channel->hasMember(client)) {
        client->sendRaw(":localhost 442 " + client->getNick() + " " + origChanHash + " :You're not on that channel\r\n");
        return;
    }

    if (!channel->isOperator(client)) {
        client->sendRaw(":localhost 482 " + client->getNick() + " " + origChanHash + " :You're not channel operator\r\n");
        return;
    }

    Client* target = NULL;
    for (size_t i = 0; i < _clients.size(); ++i)
        if (_clients[i]->getNick() == nick)
            target = _clients[i];
    if (!target) {
        client->sendRaw(":localhost 401 " + client->getNick() + " " + nick + " :No such nick\r\n");
        return;
    }

    if (channel->hasMember(target)) {
        client->sendRaw(":localhost 443 " + client->getNick() + " " + nick + " " + origChanHash + " :is already on channel\r\n");
        return;
    }

    if (channel->isInvited(target)) {
        client->sendRaw(":localhost 341 " + client->getNick() + " " + nick + " " + origChanHash + "\r\n");
        return;
    }

    // Add to invite list
    channel->addInvite(target);

    // Send confirmation numeric to inviter 
    client->sendRaw(":localhost 341 " + client->getNick() + " " + nick + " " + origChanHash + "\r\n");

    // Notify the invitee (RFC-style prefix!)
    std::string notif = ":" + client->getPrefix()
        + " INVITE " + target->getNick()
        + " :" + origChanHash + "\r\n";
    target->sendRaw(notif);

}