#include "../Server.hpp"


///* INVITE <nickname> <#channel>*///

void Server::handleInvite(Client* client, std::string param)
{
    // Parse params 
    std::istringstream iss(param);
    std::string nick, channelName;
    iss >> nick >> channelName;

    // Check params 
    if (nick.empty() || channelName.empty()) {
        std::string reply = ":localhost 461 " + client->getNick() + " INVITE :Not enough parameters\r\n";
        client->sendRaw(reply);
        return;
    }

    if (channelName[0] != '#') {
        std::string reply = ":localhost 403 " + client->getNick() + " " + channelName + " :No such channel\r\n";
        client->sendRaw(reply);
        return;
    }
    channelName = channelName.substr(1);
    // Channel exists? 
    Channel* channel = getChannel(channelName);
    if (!channel) {
        std::string reply = ":localhost 403 " + client->getNick() + " " + channelName + " :No such channel\r\n";
        client->sendRaw(reply);
        return;
    }

    // User is on channel? 
    if (!channel->hasMember(client)) {
        std::string reply = ":localhost 442 " + client->getNick() + " " + channelName + " :You're not on that channel\r\n";
        client->sendRaw(reply);
        return;
    }

    // Is client operator? (for +i or always for 42) 
    if (!channel->isOperator(client)) {
        std::string reply = ":localhost 482 " + client->getNick() + " " + channelName + " :You're not channel operator\r\n";
        client->sendRaw(reply);
        return;
    }

    // Find invitee 
    Client* target = NULL;
    for (size_t i = 0; i < clients.size(); ++i)
        if (clients[i]->getNick() == nick)
            target = clients[i];
    if (!target) {
        std::string reply = ":localhost 401 " + client->getNick() + " " + nick + " :No such nick\r\n";
        client->sendRaw(reply);
        return;
    }

    // Already in channel? 
    if (channel->hasMember(target)) {
        std::string reply = ":localhost 443 " + client->getNick() + " " + nick + " " + channelName + " :is already on channel\r\n";
        client->sendRaw(reply);
        return;
    }

    // Add to invite list (implement logic in Channel) 
    // channel->addInvite(target);

    // Send confirmation numeric to inviter 
    std::string reply = ":localhost 341 " + client->getNick() + " " + nick + " " + channelName + "\r\n";
    client->sendRaw(reply);

    // Optionally, notify the invitee 
    std::string notif = ":" + client->getNick() + "!" + client->getUser() + "@localhost INVITE " + nick + " :" + channelName + "\r\n";
    target->sendRaw(notif);

    // Now, when the invited client sends JOIN, you should allow it (even if +i) if they're on the invite list.
}