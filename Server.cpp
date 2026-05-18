#include "Server.hpp"
#include <sstream>

bool Server::keepRunning = true;

Server::Server(int port, std::string password) : _port(port), _password(password){}

Server::~Server() {}

void    Server::setupSocket()
{
    struct sockaddr_in  serverAddr;

    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(_port);

    _serverSocket = socket(AF_INET, SOCK_STREAM ,0);
    if (_serverSocket < 0)
        throw std::runtime_error("Socket failed");

    fcntl(_serverSocket, F_SETFL, O_NONBLOCK);

    int reuseAddr = 1;
    setsockopt(_serverSocket, SOL_SOCKET, SO_REUSEADDR, &reuseAddr, sizeof(reuseAddr));
    if (bind(_serverSocket, (sockaddr *)&serverAddr, sizeof(serverAddr)) < 0)
        throw std::runtime_error("Bind failed");       
}

void Server::acceptNewConnection()
{
    int                 clientFd;
    struct sockaddr_in  clientAddr;
    struct pollfd       clientPollFd;
    socklen_t len = sizeof(clientAddr);

    clientFd = accept(_serverSocket, (sockaddr*) &clientAddr, &len);
    if (clientFd < 0)
        std::cerr << "Accept failed" << std::endl;
    else
    {
        fcntl(clientFd, F_SETFL, O_NONBLOCK);

        Client* newClient = new Client(clientFd);
        clients.push_back(newClient);
        std::string clientIp = inet_ntoa(clientAddr.sin_addr);// ip ktwli string 
        newClient->setHost(clientIp);
        std::cout << "[SERVER] New Connection ! FD: " << clientFd << " IP: " << clientIp << std::endl;
                            
        clientPollFd.fd = clientFd;
        clientPollFd.events = POLLIN;
        _pollFds.push_back(clientPollFd);
        _clientBuffers[clientFd] = "";
    }
}

void    Server::clean()
{
    for (size_t i = 0; i < clients.size(); i++)
    {
        delete clients[i];
    }
    clients.clear();
    
    std::map<std::string, Channel*>::iterator it;
    for (it = _channels.begin(); it != _channels.end(); ++it)
    {
        delete it->second;
    }
    _channels.clear();
    
    for (size_t i = 0; i < _pollFds.size(); i++)
    {
        close(_pollFds[i].fd);
    }
    _pollFds.clear();
}

void Server::runEventLoop()
{
    _serverPollFd.fd = _serverSocket;
    _serverPollFd.events = POLLIN;
    _pollFds.push_back(_serverPollFd);
    
    if (listen(_serverSocket, 0) < 0)
        throw std::runtime_error("Listen failed");// taille dyal file dattente < 5 si 0 on laisse le system decide 
    while (keepRunning)
    {
        if (poll(&_pollFds[0], _pollFds.size(), -1) < 0)
            break ;
        for (size_t i = 0; i < _pollFds.size(); i++)
        {
            if (_pollFds[i].revents & POLLIN)
            {
                int clientFd = _pollFds[i].fd;
                if (clientFd == _serverSocket)
                    acceptNewConnection();
                else
                    readDataFromClient(i, clientFd);
            }
        }
    }
    clean();
}

void Server::disconnectClient(size_t &i, int clientFd)
{
    for (size_t j = 0; j < clients.size(); j++)
    {
        if (clients[j]->getFd() == clientFd)
        {
            delete clients[j];
            clients.erase(clients.begin() + j);
            break;
        }
    }
    _clientBuffers.erase(clientFd);
    close(clientFd);
    _pollFds.erase(_pollFds.begin() + i); // n7ydha mn vector
    i--; //v kyn9slo size n9si bch mtn9zich chi client
}

void Server::readDataFromClient(size_t &i, int clientFd)
{
    std::string line;
    char        buf[1024];

    memset(buf, 0, sizeof(buf));
    int bytesRead = recv(clientFd , buf, 1023, 0);
    if (bytesRead <= 0)
        disconnectClient(i, clientFd);
    else
    {
        _clientBuffers[clientFd].append(buf, bytesRead);
        size_t lineEnd = _clientBuffers[clientFd].find("\r\n");
        while (lineEnd != std::string::npos)
        {
            line = _clientBuffers[clientFd].substr(0, lineEnd);


            std::cout << "[PARSER] Commande extraite : [" << line << "]" << std::endl;

            
            handle_command(clientFd, line); // you need to build this

            _clientBuffers[clientFd].erase(0,lineEnd + 2);
            lineEnd = _clientBuffers[clientFd].find("\r\n");
        } 
    }
}

Client* Server::getClientByFd(int fd)
{
    for (size_t i = 0; i < clients.size(); i++)
    {
        if (clients[i]->getFd() == fd)
            return clients[i];
    }
    return NULL;
}


std::string Server::getPassword()
{
    return _password;
}

bool Server::nickIsInUse(std::string nickname)const
{
    for (size_t i = 0; i  < clients.size(); i++)
    {
        if (clients[i]->getNick() == nickname)
            return true;
    }
    return false;
}

void Server::sendReply(Client* client, const std::string& code, 
                      const std::string& nick, 
                      const std::string& arg, 
                      const std::string& message)
{
    std::string fullmsg = std::string(":") + "server" + " " + code + " " + nick;

    if (!arg.empty()) fullmsg += " " + arg;
    fullmsg += " :" + message + "\r\n";
    client->sendRaw(fullmsg);
}


bool Server::is_command(std::string command)
{
    return (command == "PASS" || command == "NICK" || command == "USER");
}

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

bool Server::isValidNick(const std::string& nick)
{
    if (nick.empty() || nick.size() > 9)
        return false;

    const std::string allowedFirst = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz[]\\`^{}|";
    const std::string allowedRest  = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-[]\\`^{}|";

    if (allowedFirst.find(nick[0]) == std::string::npos)
        return false;

    for (size_t i = 1; i < nick.size(); ++i)
    {
        if (allowedRest.find(nick[i]) == std::string::npos)
            return false;
    }
    return true;
}

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

Channel* Server::getChannel(std::string name) const {
    std::map<std::string, Channel*>::const_iterator it = _channels.find(name);
    if (it != _channels.end())
        return it->second;
    return NULL;
}

void Server::addChannel(std::string chname, Channel* newChannel) {
    _channels[chname] = newChannel;
}


// bool allCmd(std::string cmd)
// {
//     return  cmd == "PRIVMSG" || cmd == "JOIN" || cmd == "KICK" || cmd == "MODE" || cmd == "TOPIC" || cmd == "INVITE";
// }

void Server::handle_command(int fd, std::string& line)
{
    Client* client = getClientByFd(fd);
    if (!client)
        return;

    std::string command;
    std::string param;
    std::istringstream iss(line);
    iss >> command;
    for (size_t i = 0; i < command.size(); ++i)
        command[i] = toupper(command[i]);
    size_t pos = line.find(' ');
    if (pos == std::string::npos)
        param =  "";
    else
        param =  line.substr(pos + 1);
    if (!client->getisAuthorized())
    {
        // if (allCmd(command))
        // {
        //     sendReply(client, "421", client->getNick(), command, 
        //     "");
        //     return ;

        // }
        // else


        if (!is_command(command)) // hnaya ila drt privmsg 9bl pass rtl3 unkown comand wlk mkhshach tl3 unkown command fixiha hadi
        { //421 == ERR_unkowncommand hadi bdltha
            sendReply(client, "421", client->getNick(), command, 
            "Unkown command");
            return ;
        }
        if (param == "") // fr9thom
        {
            sendReply(client, "461",  client->getNick(), 
                command, "Not enough parameters");
            return ;
        }
        if (command == "PASS") // check if pass dejat t3amer 
            handlePass(client, param);
        else if (command == "NICK")
            handleNick(client, param);
        else if (command == "USER")
            handleUser(client, param);
        // else
        // {
        //         // ERR_NOTREGISTERED (451)
        //     // :<server> 451 <nick or *> :You have not registered
        //     sendReply(client, "451", client->getNick(), "", "You have not registered");
        // }
        if (client->gethasUser() && client->getnickFilled() && client->getPasswordFilled())
        {
            client->setAuthorized(true);
            //
                 /*001    RPL_WELCOME
              "Welcome to the Internet Relay Network
               <nick>!<user>@<host>" */
            std::string wlc_msg = "Welcome to the Internet Relay Network " + client->getNick() + "!" + client->getUser() + "@" + client->getHost();
            sendReply(client, "001", client->getNick(), "", wlc_msg);
            // std::cout << "client :" << client->getNick() << " Successfully authentified" << std::endl;
        }
        
    }
    else
    {
        if (command == "PASS")
            handlePass(client, param);
        else if (command == "USER")
            handleUser(client, param);
        else if (command == "NICK")
            handleNick(client, param);
        else if (command == "PING")
            handlePing(client, param);
        else if (command == "QUIT")
            handleQuit(client);
        else if (command == "PRIVMSG")
            handlePrivmsg(client, param);
        // else if (command == "PONG")
        //     handlePong(client, param);

        else if (command == "JOIN")
            handleJoin(client, param);
        else if (command == "TOPIC")
            handleTopic(client, param);
        // else if (command == "INVITE")
        //     handleInvite(fd, line);
        // else if (command == "MODE")
        //     handleMode(fd, line);
        else if (command == "KICK")
            handleKick(client, param);

        else
        {
            sendReply(client, "421", client->getNick(), command, "Unknown command");
            return ;
        }
    }
}