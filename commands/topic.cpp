#include "../Server.hpp"

///* TOPIC #channel  txt*///

void Server::handleTopic(Client* client, std::string param)
{
    std::istringstream iss(param);
    std::string channelName, topic;
    iss >> channelName; 
    std::getline(iss, topic);
    if (!topic.empty() && topic[0] == ' ')
        topic = topic.substr(1);
    for (size_t i = 0; i < channelName.size(); i++)
        channelName[i] = std::tolower(channelName[i]);
    if (channelName.empty() || channelName[0] != '#') {
        client->sendRaw(":localhost 403 " + client->getNick() + " " + channelName + " :No such channel\r\n");
        return;
    }
    Channel* channel = getChannel(channelName.substr(1));
    if (!channel) {
        client->sendRaw(":localhost 403 " + client->getNick() + " " + channelName + " :No such channel\r\n");
        return;
    }
    if (!channel->hasMember(client)) {
        client->sendRaw(":localhost 442 " + client->getNick() + " " + channelName + " :You're not on that channel\r\n");
        return;
    }

    if (!topic.empty()) {
        if (channel->isTopicOpOnly() && !channel->isOperator(client)) {
            client->sendRaw(":localhost 482 " + client->getNick() + " " + channelName + " :You're not channel operator\r\n");
            return;
        }
        channel->setTopic(topic);
        channel->broadcast(":" + client->getPrefix() + " TOPIC " + channelName + " :" + topic + "\r\n");
    } else {
        std::string topicMsg;
        if ( channel->getTopic().empty())
            topicMsg = ":localhost 331 " + client->getNick() + " " + channelName + " :No topic is set\r\n";
        else
            topicMsg = ":localhost 332 " + client->getNick() + " " + channelName + " :" + channel->getTopic() + "\r\n";

        client->sendRaw(topicMsg);
    }
}