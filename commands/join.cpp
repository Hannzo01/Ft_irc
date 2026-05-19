#include "../Server.hpp"

static std::vector<std::string> split(const std::string& str, char delim) {
    std::vector<std::string> res; std::string buf;
    std::istringstream ss(str);
    while (std::getline(ss, buf, delim)) {
        if (!buf.empty())
        {
            for(size_t i = 0; i< buf.size(); i++)
                buf[i] = tolower(buf[i]);
            res.push_back(buf);
        }
    }
    return res;
}

void Server::handleJoin(Client* client, std::string param)
{
    if (param.empty()) {
        // Not enough parameters (RFC 2812: 461)
        sendReply(client, "461", client->getNick(), "JOIN", "Not enough parameters");
        return;
    }
    // if (param == "0")
    // {

    // }
    std::vector<std::string> channels = split(param, ',');
    for (size_t i = 0; i < channels.size(); ++i)
    {
        std::string chname = channels[i];
        if (chname.empty() || chname[0] != '#')
            continue;
        Channel* channel = getChannel(chname);
        bool newly_created = false;
        if (!channel) {
            channel = new Channel(chname);
            addChannel(chname, channel);
            newly_created = true;
        }
        if (channel->hasMember(client))
            continue;
        channel->addMember(client);
        client->joinChannel(channel);

        if (newly_created)
            channel->addOperator(client);

        // Send JOIN notification to all members
        client->setCurrentChannel(channel);
        std::string joinMsg = ":" + client->getNick() + "!" + client->getUser() + "@localhost JOIN :" + chname + "\r\n";
        const std::vector<Client*>& members = channel->getMembers();
        for (size_t j = 0; j < members.size(); ++j)
            members[j]->sendRaw(joinMsg);

        // Send topic if any
        if (!channel->getTopic().empty()) {
            std::string t = ":" + std::string("localhost") + " 332 " + client->getNick() + " " + chname + " :" + channel->getTopic() + "\r\n";
            client->sendRaw(t);
        } else {
            std::string t = ":" + std::string("localhost") + " 331 " + client->getNick() + " " + chname + " :No topic is set\r\n";
            client->sendRaw(t);
        }
        // Send names list (NAMES reply: 353, 366)
        std::string names_line = "="; // public channel
        for (size_t k = 0; k < members.size(); ++k)
            names_line += " " + members[k]->getNick();
        std::string names_reply = ":" + std::string("localhost") + " 353 " + client->getNick() + " = " + chname + " :" + names_line + "\r\n";
            client->sendRaw(names_reply);
        std::string names_end = ":" + std::string("localhost") + " 366 " + client->getNick() + " " + chname + " :End of /NAMES list\r\n";
            client->sendRaw(names_end);
    }
}