#include "Server.hpp"

void Server::handleNick(Client* client, std::string param)
{
     if (!client->getpassFilled())
    {
        // ERR_NOTREGISTERED (451)
        sendReply(client, "451", "*", "", "You have not registered");
        return;
    }
    if (param.empty()) {
        // ERR_NONICKNAMEGIVEN (431)
        // :<server> 431 <nick or *> :No nickname given
        sendReply(client, "431",  client->getNick(), "", "No nickname given");
        return;
    } else if (nickIsInUse(param)) {
        // ERR_NICKNAMEINUSE (433)
        // :<server> 433 <nick or *> <badnick> :Nickname is already in use
        sendReply(client, "433", client->getNick(), param, " Nickname is already in use");
        return;
    } else if (!isValidNick(param)) {
        // ERR_ERRONEUSNICKNAME (432)
        // :<server> 432 <nick or *> <badnick> :Erroneous nickname
        sendReply(client, "432",  client->getNick() , param, " Erroneous nickname");
        return;
    } else {
        client->setNick(param);
        client->setNickFilled(true);
    }
}

void Server::handlePass(Client* client, std::string param)
{
    if (client->getpassFilled() ) // || client->getisAuthorized() hadi commentitha huit mymknch ykon autoriser o ydkhl o awl cmd hia     if (!client->getisAuthorized())
    {
        sendReply(client, "462", client->getNick(), "", "You may not reregister");
        return;
    }
    else if (!checkPassword(param))
    {
        sendReply(client, "461", client->getNick(), "PASS ", "Not enough parameters");
        return;
    }
    else if (param != _pass)
    {
        // ERR_PASSWDMISMATCH (464)
        // :<server> 464 <nick or *> :Password incorrect
        sendReply(client, "464", client->getNick(), "", "Password incorrect");
        return;
    }
    else
        client->setPassFilled(true);
}

void Server::handleUser(Client* client, std::string param)
{
    if (!client->getpassFilled())
    {
        // ERR_NOTREGISTERED (451)
        sendReply(client, "451", "*", "", "You have not registered");
        return;
    }
    if (client->gethasUser()) {
        // ERR_ALREADYREGISTERED (462)
        sendReply(client, "462",  client->getNick(), "", "You may not reregister");
        return;
    } else if (param.empty()) {
        // ERR_NEEDMOREPARAMS (461)
        sendReply(client, "461",  client->getNick(), "USER", " Not enough parameters");
        return;

    } else {
        std::istringstream pss(param);
        std::string username, mode, unused, realname;
        pss >> username >> mode >> unused;
        std::getline(pss, realname);
        if (!realname.empty() && realname[0] == ':')
            realname = realname.substr(1);
        client->setUser(username);
        client->setRealName(realname);
        client->setHasUser(true);
    }
}

void Server::handlePing(Client* client, std::string param)
{
    std::string buff = "PONG :" + param + "\r\n";
    send(client->getFd(),  buff.c_str(), buff.length(), 0);
    std::cout << "Done eft pong hehe" << std::endl;
}


void Server::handleQuit(Client* client)
{
    //nderha b chi var hsn

    // for (size_t i = 0; i < _v.size() ; i++)
    // {
    //     if (_v[i].fd == client->getFd())
    //     {
    //         _fds_buff.erase(_v[i].fd);
    //         close(_v[i].fd);
    //         _v.erase(_v.begin() + i);
    //         break;
    //     }
    // }
    for (size_t j = 0; j < clients.size(); j++)
    {
        if (clients[j] == client)
        {
            delete clients[j];
            clients.erase(clients.begin() + j);
            break;
        }
    }
}

void Server::handlePrivmsg(Client* client, std::string param)
{
    std::string target;
    std::string message;
    std::string argument;
    std::istringstream iss(param);
    std::string channel_name;

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
        channel_name = target.substr(1, target.size());
        for (size_t i = 0; i < channel_name.size(); ++i)
            channel_name[i] = tolower(channel_name[i]); // ransavihom b to lower f join bch yjini sahl hit channel Salon hia SALON
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
        for(size_t i = 0; i < clients.size(); i++)
        {
            if (are_equal(target, clients[i]->getNick()) == true)
            {
                nick_found = true;
                // :<expediteur>!<username>@<host> PRIVMSG <cible> :<le message complet>\r\n
                message = ":" + client->getNick() + "!" + client->getUser() + "@" + client->getHost() + " PRIVMSG " + target + " :" + argument + "\r\n";
                send(clients[i]->getFd(), message.c_str(), message.size(), 0);
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


// void Server::handlePong(Client* client, std::string param)
// {

// }
