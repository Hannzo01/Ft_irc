#include "../Server.hpp"

///*   KICK <channel> <nick> [:reason]  *///

void Server::handleKick(Client* client, std::string param)
{
    std::istringstream iss(param);
    std::string channelName, nick, reason;
    iss >> channelName >> nick;

    std::string word;
    if (iss >> word && !word.empty() && word[0] == ':')
    {
        reason = word.substr(1);
        std::string more;
        while (iss >> more)
            reason += " " + more;
    }
    for (size_t i = 0; i < channelName.size(); i++)
        channelName[i] = std::tolower(channelName[i]);
    if (reason.empty())
        reason = client->getNick();

    if (channelName.empty() || nick.empty()) {
        client->sendRaw(":localhost 461 " + client->getNick() + " KICK :Not enough parameters\r\n");
        return;
    }
    if (channelName[0] != '#') {
        client->sendRaw(":localhost 403 " + client->getNick() + " " + channelName + " :No such channel\r\n");
        return;
    }
    std::string origChanHash = channelName;
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
    if (!target || !channel->hasMember(target)) {
        client->sendRaw(":localhost 441 " + client->getNick() + " " + nick + " " + origChanHash + " :They aren't on that channel\r\n");
        return;
    }

    channel->broadcast(":" + client->getPrefix()
        + " KICK " + origChanHash + " " + target->getNick()
        + " :" + reason + "\r\n");
    
    channel->removeMember(target);
    target->leaveChannel(channel);

    channel->removeOperator(target); 
    channel->removeInvite(target);  


    if (channel->getMembers().empty())
        removeChannel(channelName);

}