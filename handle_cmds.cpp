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

void prefix()
{

}

// void Server::handlePrivmsg(Client* client, std::string param){
    // std::string target; 
    // std::istringstream iss(param);
    // iss >> target;

//     int pos = param.find(":", 0);

//     bool nick_found = false;
//     // first : check si une personne ou une chaine
//     if (target[0] == '#')
//     {

//     }    // chaine
//     else
//     {
//         for(int i = 0; i < clients.size(); i++)
//         {
//             if (target == clients[i]->getNick())
//             {
//                 nick_found = true;
//                 // check if the msg is empty if yes error else we send it 

//             }
//         }
//         if (nick_found == false)
//         {

//         }
//     }

//     // si c une personne check si elle existe si non tu envoie au fd errur 401 ERR_NOSUCHNICK

//     //si le msg est vide 412 ERR_NOTEXTTOSEND

//     // second si c pour une chaine on envoie le msg pour tout le monde sauf nous meme le serveur

// }


// void Server::handlePong(Client* client, std::string param)
// {

// }
