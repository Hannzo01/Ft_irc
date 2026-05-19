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
        client->sendRaw(":localhost 461 " + client->getNick() + " JOIN :Not enough parameters\r\n");
        return;
    }

    std::vector<std::string> channels = split(param, ',');
    for (size_t i = 0; i < channels.size(); ++i)
    {
        std::string chname = channels[i];
        if (chname.empty() || chname[0] != '#')
            continue;

        std::string chnameBare = chname.substr(1); // strip '#'

        Channel* channel = getChannel(chnameBare);
        bool newly_created = false;
        if (!channel) {
            channel = new Channel(chnameBare);
            addChannel(chnameBare, channel);
            newly_created = true;
        }
        // Check if already a member
        if (channel->hasMember(client))
            continue;

        // +i handling: Only allow if invited or +i not set
        if (channel->isInviteOnly() && !channel->isInvited(client)) {
            client->sendRaw(":localhost 473 " + client->getNick() + " #" + chnameBare + " :Cannot join channel (+i)\r\n");
            continue;
        }
        if (channel->isLimitEnabled() && channel->getMembers().size() == (size_t)channel->getLimit()){
            client->sendRaw(":localhost 471 " + client->getNick() + " #" + chnameBare + " :Cannot join channel (+l)\r\n");
            continue;
        }

        channel->addMember(client);
        client->joinChannel(channel);
        if (newly_created)
            channel->addOperator(client);

        // Remove invite after join (if applicable)
        if (channel->isInvited(client))
            channel->removeInvite(client);

        // Build the channel name for all IRC replies (with #)
        std::string ircChanName = "#" + chnameBare;

        // Send JOIN notification to all members, including this client
        std::string joinMsg = ":" + client->getPrefix() + " JOIN :" + ircChanName + "\r\n";
        const std::vector<Client*>& members = channel->getMembers();
        for (size_t j = 0; j < members.size(); ++j)
            members[j]->sendRaw(joinMsg);

        // Send TOPIC info (332/331)
        if (!channel->getTopic().empty()) {
            client->sendRaw(":localhost 332 " + client->getNick() + " " + ircChanName + " :" + channel->getTopic() + "\r\n");
        } else {
            client->sendRaw(":localhost 331 " + client->getNick() + " " + ircChanName + " :No topic is set\r\n");
        }

        // Send NAMES list (353/366)
        // Build names list, op users with @
        std::string names_line;
        for (size_t k = 0; k < members.size(); ++k) {
            if (!names_line.empty())
                names_line += " ";
            if (channel->isOperator(members[k]))
                names_line += "@" + members[k]->getNick();
            else
                names_line += members[k]->getNick();
        }
        client->sendRaw(":localhost 353 " + client->getNick() + " = " + ircChanName + " :" + names_line + "\r\n");
        client->sendRaw(":localhost 366 " + client->getNick() + " " + ircChanName + " :End of /NAMES list\r\n");
    }
}