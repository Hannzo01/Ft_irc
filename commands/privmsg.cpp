#include "../Server.hpp"

bool    Server::are_equal(const std::string& a, const std::string& b){
    if (a.size() != b.size())
        return false;
    for (size_t i = 0; i < a.size(); i++)
    {
        if (tolower(a[i]) != tolower(b[i]))
            return false;
    }
    return true;
}

void Server::handlePrivmsg(Client* client, std::string param)
{
    std::string     target;
    std::string     message;
    std::string     argument;
    std::string     channel_name;
    std::istringstream iss(param);

    iss >> target;
    bool nick_found = false;

    if (target.empty()) //hnaya hit y9ed ykon param "   " << chekc hdi 
    {
        sendReply(client, "411", client->getNick(), "", ":No recipient given");
        return ;
    }
    iss >> argument;
    if (argument.empty())
    {
        sendReply(client, "412", client->getNick(), "", ":No text to send");
        return ;        
    }

    size_t pos = param.find(" :", 0);
    if (pos != std::string::npos )
        argument = param.substr(pos + 2);

    if (target[0] == '#')
    {
        
        channel_name = target.substr(1);
        for (size_t i = 0; i < channel_name.size(); ++i)
            channel_name[i] = tolower(channel_name[i]);
        std::map<std::string, Channel*>::iterator it = _channels.find(channel_name);
        if (it != _channels.end())
        {
            if (!it->second->hasMember(client))
                sendReply(client, "404", client->getNick(), target, ":Cannot send to channel");
            else
            {
                message = ":" + client->getNick() + "!" + client->getUser() + "@" + client->getHost() + " PRIVMSG " + target + " :" + argument + "\r\n";
                it->second->broadcast_msg(client, message);
            }
        }
        else
        {
            sendReply(client, "403", client->getNick(), target, ":No such channel");
            return;
        }
    }
    else
    {
        for(size_t i = 0; i < _clients.size(); i++)
        {
            if (are_equal(target, _clients[i]->getNick()) == true)
            {
                nick_found = true;
                // :<expediteur>!<username>@<host> PRIVMSG <cible> :<le message complet>\r\n
                message = ":" + client->getNick() + "!" + client->getUser() + "@" + client->getHost() + " PRIVMSG " + target + " :" + argument + "\r\n";
                if (send(_clients[i]->getFd(), message.c_str(), message.size(), 0) < 0)
                    throw std::runtime_error("send failed");

                break ;
            }
        }
        if (nick_found == false)
        {
            sendReply(client, "401", client->getNick(), target, ":No such nick");
            return ;
        }
    }
}
