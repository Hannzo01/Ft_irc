#include "../Server.hpp"

///*JOIN #chan1,#chan2 key1,key2  irc standare*///

static std::string trim(const std::string& s) {
    size_t start = s.find_first_not_of(" \t\r\n");
    size_t end = s.find_last_not_of(" \t\r\n");
    if (start == std::string::npos) return "";
    return s.substr(start, end - start + 1);
}

void split_channels_and_keys(const std::string& param, std::vector<std::string>& out_channels, std::vector<std::string>& out_keys)
{
    std::vector<std::string> temp;
    std::string buf;
    std::istringstream ss(param);
    while (std::getline(ss, buf, ',')) {
        std::string t = trim(buf);
        if (!t.empty())
            temp.push_back(t);
    }

    for (size_t i = 0; i < temp.size(); ++i) {
        std::istringstream ws(temp[i]);
        std::vector<std::string> elems;
        std::string word;
        while (ws >> word) {
            elems.push_back(word);
        }
        if (elems.size() == 1) 
            out_channels.push_back(elems[0]);
        else if (elems.size() == 2) {
            out_channels.push_back(elems[0]);
            out_keys.push_back(elems[1]);
            for (size_t j = i + 1; j < temp.size(); ++j) {
                    out_keys.push_back(temp[j]);
            }
            break; 
        }
    }
}


void Server::handleJoin(Client* client, std::string param)
{
    if (param.empty()) {
        client->sendRaw(":localhost 461 " + client->getNick() + " JOIN :Not enough parameters\r\n");
        return;
    }

    std::string channels_arg, keys_arg;
    size_t space_pos = param.find(' ');
    if (space_pos != std::string::npos) {
        channels_arg = param.substr(0, space_pos);
        keys_arg = param.substr(space_pos + 1);
    } else {
        channels_arg = param;
        keys_arg = "";
    }
    std::vector<std::string> channels ;
    std::vector<std::string> keys;
    split_channels_and_keys(param,  channels, keys);

    for (size_t i = 0; i < channels.size(); ++i)
    {
        std::string chname = channels[i];
        if (chname.empty() || chname[0] != '#')
            continue;
        std::string chnameBare = chname.substr(1);

        Channel* channel = getChannel(chnameBare);
        bool newly_created = false;
        if (!channel) {
            channel = new Channel(chnameBare);
            addChannel(chnameBare, channel);
            newly_created = true;
        }

        if (channel->hasMember(client))
            continue;

        if (channel->isInviteOnly() && !channel->isInvited(client)) {
            client->sendRaw(":localhost 473 " + client->getNick() + " #" + chnameBare + " :Cannot join channel (+i)\r\n");
            continue;
        }

        if (channel->isLimitEnabled() && channel->getMembers().size() >= (size_t)channel->getLimit()) {
            client->sendRaw(":localhost 471 " + client->getNick() + " #" + chnameBare + " :Cannot join channel (+l)\r\n");
            continue;
        }

        if (channel->isKeyEnabled()) {
            std::string supplied_key;
            if (i < keys.size())
                supplied_key = keys[i]; // keys list might be shorter than channels
            if (supplied_key.empty() || supplied_key != channel->getKey()) {
                client->sendRaw(":localhost 475 " + client->getNick() + " #" + chnameBare + " :Cannot join channel (+k)\r\n");
                continue;
            }
        }

        channel->addMember(client);
        client->joinChannel(channel);
        if (newly_created)
            channel->addOperator(client);

        if (channel->isInvited(client))
            channel->removeInvite(client);

        std::string ircChanName = "#" + chnameBare; 

        std::string joinMsg = ":" + client->getPrefix() + " JOIN :" + ircChanName + "\r\n";
        const std::vector<Client*>& members = channel->getMembers();
        for (size_t j = 0; j < members.size(); ++j)
            members[j]->sendRaw(joinMsg);

        if (!channel->getTopic().empty()) {
            client->sendRaw(":localhost 332 " + client->getNick() + " " + ircChanName + " :" + channel->getTopic() + "\r\n");
        } else {
            client->sendRaw(":localhost 331 " + client->getNick() + " " + ircChanName + " :No topic is set\r\n");
        }

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