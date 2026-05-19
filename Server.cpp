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
        _clients.push_back(newClient);
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
    for (size_t i = 0; i < _clients.size(); i++)
    {
        delete _clients[i];
    }
    _clients.clear();
    
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
    for (size_t j = 0; j < _clients.size(); j++)
    {
        if (_clients[j]->getFd() == clientFd)
        {
            delete _clients[j];
            _clients.erase(_clients.begin() + j);
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


            std::cout << "[PARSER] Commande extraite : [" << line << "]" << std::endl; // delete later

            
            processCommand(clientFd, line);

            _clientBuffers[clientFd].erase(0,lineEnd + 2);
            lineEnd = _clientBuffers[clientFd].find("\r\n");
        } 
    }
}

Client* Server::getClientByFd(int fd)
{
    for (size_t i = 0; i < _clients.size(); i++)
    {
        if (_clients[i]->getFd() == fd)
            return _clients[i];
    }
    return NULL;
}

std::string Server::getPassword(){
    return _password;
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

Channel* Server::getChannel(std::string name) const {
    std::map<std::string, Channel*>::const_iterator it = _channels.find(name);
    if (it != _channels.end())
        return it->second;
    return NULL;
}

void Server::addChannel(std::string chname, Channel* newChannel) {
    _channels[chname] = newChannel;
}
