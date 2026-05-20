#include "../Server.hpp"

void Server::handleUser(Client* client, std::string param)
{
    if (!client->getPasswordFilled())
    {
        sendReply(client, "451", "*", "", "You have not registered");
        return;
    }
    if (client->gethasUser()) {
        sendReply(client, "462",  client->getNick(), "", "You may not reregister");
        return;
    } else if (param.empty()) {
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
