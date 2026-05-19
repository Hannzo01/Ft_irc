#include "../Server.hpp"

void Server::handleNick(Client* client, std::string param)
{
     if (!client->getPasswordFilled())
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