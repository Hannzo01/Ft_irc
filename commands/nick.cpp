#include "../Server.hpp"

bool Server::isValidNick(const std::string& nick)
{
    if (nick.empty() || nick.size() > 9)
        return false;

    const std::string allowedFirst = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz[]\\`_^{}|";
    const std::string allowedRest  = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-[]\\`_^{}|";

    if (allowedFirst.find(nick[0]) == std::string::npos)
        return false;

    for (size_t i = 1; i < nick.size(); ++i)
    {
        if (allowedRest.find(nick[i]) == std::string::npos)
            return false;
    }
    return true;
}

bool Server::nickIsInUse(std::string nickname)const
{
    for (size_t i = 0; i  < _clients.size(); i++)
    {
        if (_clients[i]->getNick() == nickname)
            return true;
    }
    return false;
}

void Server::handleNick(Client* client, std::string param)
{
     if (!client->getPasswordFilled())
    {
        sendReply(client, "451", "*", "", "You have not registered");
        return;
    }
    if (param.empty()) {
        sendReply(client, "431",  client->getNick(), "", "No nickname given");
        return;
    } else if (nickIsInUse(param)) {
        sendReply(client, "433", client->getNick(), param, " Nickname is already in use");
        return;
    } else if (!isValidNick(param)) {
        sendReply(client, "432",  client->getNick() , param, " Erroneous nickname");
        return;
    } else {
        client->setNick(param);
        client->setNickFilled(true);
    }
}