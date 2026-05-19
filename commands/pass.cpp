#include "../Server.hpp"

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