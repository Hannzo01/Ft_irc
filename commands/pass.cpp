#include "../Server.hpp"

bool Server::checkPassword(std::string& param)
{
    // Remove any leading whitespace
    size_t first = param.find_first_not_of(" \t\r\n");
    if (first != std::string::npos)
        param = param.substr(first);

    // Remove leading ':' (IRC trailing param convention, e.g. PASS :hunter2)
    if (!param.empty() && param[0] == ':')
        param = param.substr(1);

    // Should not be empty
    if (param.empty())
        return false;

    // IRC line total length max (including CRLF) is 512
    if (param.length() > 510) // being strict, real limit is >510 with command
        return false;

    // Optionally, forbid spaces, but RFC allows them if param was given as trailing
    // If you want to forbid, uncomment this:
    // if (param.find(' ') != std::string::npos) return false;

    return true;
}

void Server::handlePass(Client* client, std::string param)
{
    if (client->getPasswordFilled() ) // || client->getisAuthorized() hadi commentitha huit mymknch ykon autoriser o ydkhl o awl cmd hia     if (!client->getisAuthorized())
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
        // ERR_passwordWDMISMATCH (464)
        // :<server> 464 <nick or *> :Password incorrect
        sendReply(client, "464", client->getNick(), "", "Password incorrect");
        return;
    }
    else
        client->setPassFilled(true);
}