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
        client->sendRaw(":localhost 461 " + client->getNick() + " INVITE :Not enough parameters\r\n");
        return;
    }

    if (channelName[0] != '#') {
        client->sendRaw(":localhost 403 " + client->getNick() + " " + channelName + " :No such channel\r\n");
        return;
    }
    std::string origChanHash = channelName; // save "#chan" for reply
    channelName = channelName.substr(1);

    // Channel exists? 
    Channel* channel = getChannel(channelName);
    if (!channel) {
        client->sendRaw(":localhost 403 " + client->getNick() + " " + origChanHash + " :No such channel\r\n");
        return;
    }

    // User is on channel? 
    if (!channel->hasMember(client)) {
        client->sendRaw(":localhost 442 " + client->getNick() + " " + origChanHash + " :You're not on that channel\r\n");
        return;
    }

    // Is client operator? (required for 42/IRC, always for ft_irc)
    if (!channel->isOperator(client)) {
        client->sendRaw(":localhost 482 " + client->getNick() + " " + origChanHash + " :You're not channel operator\r\n");
        return;
    }

    // Find invitee 
    Client* target = NULL;
    for (size_t i = 0; i < _clients.size(); ++i)
        if (_clients[i]->getNick() == nick)
            target = _clients[i];
    if (!target) {
        client->sendRaw(":localhost 401 " + client->getNick() + " " + nick + " :No such nick\r\n");
        return;
    }

    // Already in channel? 
    if (channel->hasMember(target)) {
        client->sendRaw(":localhost 443 " + client->getNick() + " " + nick + " " + origChanHash + " :is already on channel\r\n");
        return;
    }

    // Already invited? (NOT in RFC, but good to block double invites)
    if (channel->isInvited(target)) {
        // Optionally: just reply 341 as usual (don't fail)
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