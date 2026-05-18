#include "../Server.hpp"

static std::string trim(const std::string& str) {
    size_t start = str.find_first_not_of(" \t\r\n");
    size_t end = str.find_last_not_of(" \t\r\n");
    return (start == std::string::npos) ? "" : str.substr(start, end - start + 1);
}

void Server::handleTopic(Client* client, std::string param)
{
    // Parse parameters
    std::istringstream iss(param);
    std::string channelName;
    iss >> channelName;

    std::string rest;
    std::getline(iss, rest);
    rest = trim(rest);

    if (!rest.empty() && rest[0] == ':')
        rest = rest.substr(1);

    if (channelName.empty()) {
        std::string reply = ":" + std::string("localhost") + " 461 " +
        client->getNick() + " TOPIC :Not enough parameters\r\n";
        client->sendRaw(reply);
        return;
    }

    Channel* channel = getChannel(channelName);
    if (!channel) {
        std::string reply = ":" + std::string("localhost") + " 403 " +
        client->getNick() + " " + channelName + " :No such channel\r\n";
        client->sendRaw(reply);
        return;
    }

    if (!channel->hasMember(client)) {
        std::string reply = ":" + std::string("localhost") + " 442 " +
        client->getNick() + " " + channelName + " :You're not on that channel\r\n";
        client->sendRaw(reply);
        return;
    }

    if (rest.empty()) {
        // **Just display topic**
        if (!channel->getTopic().empty()) {
            // RPL_TOPIC (332)
            // :<server> 332 <nick> <channel> :<topic>
            std::string reply = ":" + std::string("localhost") + " 332 " + client->getNick() + " " + channelName + " :" + channel->getTopic() + "\r\n";
            client->sendRaw(reply);
        } else {
            // RPL_NOTOPIC (331)
            // :<server> 331 <nick> <channel> :No topic is set
            std::string reply = ":" + std::string("localhost") + " 331 " + client->getNick() + " " + channelName + " :No topic is set\r\n";
            client->sendRaw(reply);
        }
        return;
    }

    // **Try to set topic!**
    // TODO: Implement mode +t restriction (allow only op to set topic)
    // If +t and !isOperator(client), send:
    // ERR_CHANOPRIVSNEEDED (482): :<server> 482 <nick> <channel> :You're not channel operator

    // Set the topic
    channel->setTopic(rest);

    // Inform all channel members
    std::string topicMsg = ":" + client->getNick() + "!" + client->getUser() + "@localhost TOPIC " + channelName + " :" + rest + "\r\n";
    const std::vector<Client*>& members = channel->getMembers();
    for (size_t i = 0; i < members.size(); ++i)
        members[i]->sendRaw(topicMsg); // Send to all in channel
}