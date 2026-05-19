#include "../Server.hpp"

bool Server::is_command(const std::string& command)
{
    return (command == "PASS" || command == "NICK" || command == "USER"
        || command == "PING" || command == "PONG" || command == "JOIN"
        || command == "PRIVMSG" || command == "TOPIC" || command == "KICK" || command == "INVITE" 
        || command == "MODE"  || command == "QUIT");
}

bool Server::authorizedRequired(const std::string& command)
{
        return ( command == "JOIN" || command == "PRIVMSG" || command == "TOPIC" || command == "KICK" || command == "INVITE" 
        || command == "MODE");
}

void Server::handleCap(Client* client, std::string param)
{
    if (!param.empty() && param.substr(0, 2) == "LS")
        client->sendRaw(":server CAP * LS :\r\n");
}

void Server::processCommand(int fd, std::string& line)
{
    Client* client = getClientByFd(fd);
    if (!client)
        return;


    std::string         command;
    std::istringstream  iss(line);
    iss >> command;
    
    for (size_t i = 0; i < command.size(); ++i)
        command[i] = toupper(command[i]);
    
    std::string param;
    size_t spacePos = line.find(' ');
    if (spacePos == std::string::npos)
        param =  "";
    else
        param =  line.substr(spacePos + 1);
    
    if (command == "CAP"){
        handleCap(client, param);
        return;
    }

    if (!client->getisAuthorized())
    {
        if (!is_command(command)){
            sendReply(client, "421", client->getNick(), command, "Unkown command");
            return ;}
        if (authorizedRequired(command)){
            sendReply(client, "451", client->getNick(), command, "You have not registered");
            return ;}
        if (command == "PING"){
            handlePing(client, param);
            return ;}
        if (command == "QUIT"){
            handleQuit(client);
            return ;}
        if (param == ""){
            sendReply(client, "461",  client->getNick(), command, "Not enough parameters");
            return ;}
        if (command == "PASS")        handlePass(client, param);
        else if (command == "NICK")   handleNick(client, param);
        else if (command == "USER")   handleUser(client, param);
        if (client->gethasUser() && client->getnickFilled() && client->getPasswordFilled()){
            client->setAuthorized(true);
            std::string wlc_msg = "Welcome to the Internet Relay Network " + client->getNick() + "!" + client->getUser() + "@" + client->getHost();
            sendReply(client, "001", client->getNick(), "", wlc_msg);}
    }

    else
    {
        if (command == "PASS")          handlePass(client, param);
        else if (command == "USER")     handleUser(client, param);
        else if (command == "NICK")     handleNick(client, param);
        else if (command == "PING")     handlePing(client, param);
        else if (command == "QUIT")     handleQuit(client);
        else if (command == "PRIVMSG")  handlePrivmsg(client, param);
        // else if (command == "PONG")  handlePong(client, param);
        else if (command == "JOIN")     handleJoin(client, param);
        else if (command == "TOPIC")    handleTopic(client, param);
        else if (command == "INVITE")   handleInvite(client, param);
        else if (command == "MODE")     handleMode(client, param);
        else if (command == "KICK")     handleKick(client, param);
        else
            sendReply(client, "421", client->getNick(), command, "Unknown command");

    }
}
