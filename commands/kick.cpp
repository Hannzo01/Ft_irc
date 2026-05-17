#include "../Server.hpp"

static std::string trim(const std::string& str) {
    size_t start = str.find_first_not_of(" \t\r\n");
    size_t end = str.find_last_not_of(" \t\r\n");
    return (start == std::string::npos) ? "" : str.substr(start, end - start + 1);
}

///*   KICK <channel> <user> [:comment]   *///
void Server::handleKick(Client* client, std::string param)
{
    // Parse parameters 
    std::istringstream iss(param);
    std::string channelName, targetNick, comment;
    iss >> channelName >> targetNick;
    std::getline(iss, comment);

    // Remove leading space/colon if present
    comment = trim(comment);
    if (!comment.empty() && comment[0] == ':')
        comment = comment.substr(1);

    //  missing param
    if (channelName.empty() || targetNick.empty()) {
        std::string reply = ":" + std::string("localhost") + " 461 " +
            client->getNick() + " KICK :Not enough parameters\r\n";
        client->sendRaw(reply);
        return;
    }
   if (channelName[0] != '#') {
        // ERR_NOSUCHCHANNEL (403)
        std::string reply = ":" + std::string("localhost") + " 403 " +
            client->getNick() + " " + channelName + " :No such channel\r\n";
        client->sendRaw(reply);
        return;
    }
    for (size_t i = 0; i < channelName.size(); i++)
        channelName[i] = tolower(channelName[i]);

    //  Channel exists
    Channel* channel = getChannel(channelName);
    if (!channel) {
        // ERR_NOSUCHCHANNEL (403)
        std::string reply = ":" + std::string("localhost") + " 403 " +
            client->getNick() + " " + channelName + " :No such channel\r\n";
        client->sendRaw(reply);
        return;
    }

    // User must be on channel
    if (!channel->hasMember(client)) {
        // ERR_NOTONCHANNEL (442)
        std::string reply = ":" + std::string("localhost") + " 442 " +
            client->getNick() + " " + channelName + " :You're not on that channel\r\n";
        client->sendRaw(reply);
        return;
    }

    //Must be operator
    if (!channel->isOperator(client)) {
        // ERR_CHANOPRIVSNEEDED (482)
        std::string reply = ":" + std::string("localhost") + " 482 " +
            client->getNick() + " " + channelName + " :You're not channel operator\r\n";
        client->sendRaw(reply);
        return;
    }

    // Is target present
    Client* target = NULL;
    // find target by nick:
    const std::vector<Client*>& members = channel->getMembers();
    for (size_t i = 0; i < members.size(); ++i)
        if (members[i]->getNick() == targetNick)
            target = members[i];
    if (!target) {
        // ERR_USERNOTINCHANNEL (441)
        std::string reply = ":" + std::string("localhost") + " 441 " +
            client->getNick() + " " + targetNick + " " + channelName + " :They aren't on that channel\r\n";
        client->sendRaw(reply);
        return;
    }

    // === 7. Do the KICK ===
    std::string kickMsg = ":" + client->getNick() + "!" + client->getUser() + "@localhost "
        + "KICK " + channelName + " " + targetNick
        + " :" + (comment.empty() ? client->getNick() : comment) + "\r\n";
    // Notify all channel members
    for (size_t i = 0; i < members.size(); ++i)
        members[i]->sendRaw(kickMsg);

    // Remove target from channel
    channel->removeMember(target);
    target->leaveChannel(channel);

    // // Optionally, remove channel if empty
    // if (channel->getMembers().empty())
    //     removeChannel(channelName);
}