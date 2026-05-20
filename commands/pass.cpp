#include "../Server.hpp"

///* user <username> 0 * <realname> *///

bool Server::checkPassword(std::string& param)
{
    size_t first = param.find_first_not_of(" \t\r\n");
    if (first != std::string::npos)
        param = param.substr(first);

    if (!param.empty() && param[0] == ':')
        param = param.substr(1);

    if (param.empty())
        return false;

    if (param.length() > 510)
        return false;
    return true;
}

void Server::handlePass(Client* client, std::string param)
{
    if (client->getPasswordFilled())
    {
        sendReply(client, "462", client->getNick(), "", "You may not reregister");
        return;
    }
    else if (!checkPassword(param))
    {
        sendReply(client, "461", client->getNick(), "PASS ", "Not enough parameters");
        return;
    }
    else if (param != _password)
    {
        sendReply(client, "464", client->getNick(), "", "Password incorrect");
        return;
    }
    else
        client->setPassFilled(true);
}