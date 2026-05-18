#include "Server.hpp"
#include <sstream>

bool Server::keep_running = true;

Server::Server(int port, std::string password) : _port(port), _password(password){}

Server::~Server() {}

void    Server::setupSocket()
{
    struct sockaddr_in localaddr;
    memset(&localaddr, 0, sizeof(localaddr));
    localaddr.sin_addr.s_addr = INADDR_ANY;
    localaddr.sin_family = AF_INET;
    localaddr.sin_port = htons(_port);
    // localaddr.sin_zero  NEED TO READ MORE ABOUT THIS VAR

    _serverSocket = socket(AF_INET, SOCK_STREAM ,0);
    if (_serverSocket < 0)
        throw std::runtime_error("Sokcet failed");
    fcntl(_serverSocket, F_SETFL, O_NONBLOCK);

    int opt = 1;
    setsockopt(_serverSocket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    if (bind(_serverSocket, (sockaddr *)&localaddr, sizeof(localaddr)) < 0)
        throw std::runtime_error("Bind failed");       
}

void Server::acceptNewConnection()
{
    int newSocket;
    struct sockaddr_in listenaddr;
    socklen_t len = sizeof(listenaddr);
    newSocket = accept(_serverSocket, (sockaddr*) &listenaddr, &len);

    if (newSocket < 0)
        std::cerr << "Accept failed" << std::endl;
    else
    {
        fcntl(newSocket, F_SETFL, O_NONBLOCK);
        Client* newClient = new Client(newSocket);
        clients.push_back(newClient);
        std::string client_ip = inet_ntoa(listenaddr.sin_addr);// ip ktwli string 
        newClient->setHost(client_ip);
        std::cout << "[SERVER] New Connection ! FD: " << newSocket << " IP: " << client_ip << std::endl;
                            
        struct pollfd spf;
        spf.fd = newSocket;
        spf.events = POLLIN;
        _pollFds.push_back(spf);
        _clientBuffers[newSocket] = "";
    }
}

void Server::readDataFromClient(size_t &i, int current_fd)
{
    std::string _cmd;
    char        buf[1024];

    memset(buf, 0, sizeof(buf));
    int byteread = recv(current_fd , buf, 1023, 0);
    if (byteread <= 0)
    {
        for (size_t j = 0; j < clients.size(); j++)
        {
            if (clients[j]->getFd() == current_fd)
            {
                delete clients[j];
                clients.erase(clients.begin() + j);
                break;
            }
        }
        _clientBuffers.erase(current_fd);
        close(current_fd);
        _pollFds.erase(_pollFds.begin() + i); // n7ydha mn vector
        i--; //v kyn9slo size n9si bch mtn9zich chi client
    }
    else
    {
        _clientBuffers[current_fd].append(buf, byteread);
        size_t delfound = _clientBuffers[current_fd].find("\r\n");
        while (delfound != std::string::npos)
        {
            _cmd = _clientBuffers[current_fd].substr(0, delfound);


            std::cout << "[PARSER] Commande extraite : [" << _cmd << "]" << std::endl;

            
            handle_command(current_fd, _cmd); // you need to build this

            _clientBuffers[current_fd].erase(0,delfound + 2);
            delfound = _clientBuffers[current_fd].find("\r\n");
        } 
    }
}

void Server::runEventLoop()
{
    _serverPollFd.fd = _serverSocket;
    _serverPollFd.events = POLLIN;
    _pollFds.push_back(_serverPollFd);

    if (listen(_serverSocket, 0) < 0)
        throw std::runtime_error("Listen failed");// taille dyal file dattente < 5 si 0 on laisse le system decide 
    while (keep_running)
    {
        if (poll(&_pollFds[0], _pollFds.size(), -1) < 0)
            break ;
        for (size_t i = 0; i < _pollFds.size(); i++)
        {
            if (_pollFds[i].revents & POLLIN)
            {
                int current_fd = _pollFds[i].fd;
                if (current_fd == _serverSocket)
                    acceptNewConnection();

                else
                    readDataFromClient(i, current_fd);
            }
        }
    }
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
        else if (command == "INVITE")
            handleInvite(client, param);
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