#include "Server.hpp"

bool Server::keep_running = true;

Server::Server(int port, std::string password) : _port(port), _pass(password){
}

Server::~Server() {
}

void    Server::init()
{
    struct sockaddr_in localaddr;
    memset(&localaddr, 0, sizeof(localaddr));
    localaddr.sin_addr.s_addr = inet_addr("127.0.0.1"); //just for now; testing on my machine lese i need to put a macro instead of direct value
    localaddr.sin_family = AF_INET;
    localaddr.sin_port = htons(_port);
    // localaddr.sin_zero  NEED TO READ MORE ABOUT THIS VAR
    _ss = socket(AF_INET, SOCK_STREAM ,0);
    if (_ss < 0)
        throw std::runtime_error("Sokcet failed");
    fcntl(_ss, F_SETFL, O_NONBLOCK);
    int opt = 1;
    setsockopt(_ss, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    if (bind(_ss, (sockaddr *)&localaddr, sizeof(localaddr)) < 0)
        throw std::runtime_error("Bind failed");       
}

void Server::add_nsocket()
{
    struct sockaddr_in listenaddr;
    socklen_t len = sizeof(listenaddr);
    _ns = accept(_ss, (sockaddr*) &listenaddr, &len);
    if (_ns < 0)
        std::cerr << "Accept failed" << std::endl;
    else
    {
        fcntl(_ns, F_SETFL, O_NONBLOCK);
        std::cout << "Client connecté ! Son FD est : " << _ns << std::endl;
        Client* newClient = new Client(_ns);
        clients.push_back(newClient);

        struct pollfd spf;
        spf.fd = _ns;
        spf.events = POLLIN;
        _v.push_back(spf);
        _fds_buff[_ns] = "";
    }
}

void Server::receive_cmd(size_t &i, int current_fd)
{
    char buf[1024];
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
        _fds_buff.erase(current_fd);
        close(current_fd);
        _v.erase(_v.begin() + i); // n7ydha mn vector
        i--; //v kyn9slo size n9si bch mtn9zich chi client
    }
    else
    {
        _fds_buff[current_fd].append(buf, byteread);
        size_t delfound = _fds_buff[current_fd].find("\r\n");
        while (delfound != std::string::npos)
        {
            _cmd = _fds_buff[current_fd].substr(0, delfound);
            handle_command(current_fd, _cmd); // you need to build this

            _fds_buff[current_fd].erase(0,delfound + 2);
            delfound = _fds_buff[current_fd].find("\r\n");
        } 
    }

}

void Server::build_and_listen()
{
    _spfd.fd = _ss;
    _spfd.events = POLLIN;
    _v.push_back(_spfd);

    if (listen(_ss, 0) < 0)
        throw std::runtime_error("Listen failed");// taille dyal file dattente < 5 si 0 on laisse le system decide 
    while (keep_running)
    {
        if (poll(&_v[0], _v.size(), -1) < 0)
            throw std::runtime_error("Poll failed");
        for (size_t i = 0; i < _v.size(); i++)
        {
            if (_v[i].revents & POLLIN)
            {
                int current_fd = _v[i].fd;
                if (current_fd == _ss)
                    add_nsocket();

                else
                    receive_cmd(i, current_fd);
            }
        }
    }

    for (size_t i = 0; i < _v.size(); i++)
    {
        close(_v[i].fd);
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

void Server::handle_command(int fd, const std::string& cmd)
{
    std::cout << cmd << std::endl;
    Client* client = getClientByFd(fd);
    if (!client)
        return;
    // ton partner parse cmd ici
    // ex: if (cmd.substr(0,4) == "NICK") client->setNick(...)
}