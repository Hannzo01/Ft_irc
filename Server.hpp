#pragma once 

#include <sys/socket.h>
#include <iostream>
#include <exception>
#include <unistd.h> // pour close function
#include <arpa/inet.h>  // For htons(), htonl(), etc.
#include <netinet/in.h> // For struct sockaddr_in
#include <cstring>
#include <string>
#include <poll.h>
#include <vector>
#include <ctype.h>
#include <algorithm>
#include <map>
#include <fcntl.h>
#include <signal.h>
#include "Client.hpp"
#include "Channel.hpp"



class Client;
class Channel;
class Server
{
    private:
        std::vector<Client *> clients;
        std::map<std::string, Channel*> _channels; // ✅ pour handle_command

        int _port;
        std::string _pass;
        int _ss;
        struct pollfd _spfd;
        std::map<int, std::string> _fds_buff;
        std::vector<struct pollfd> _v;
        int _ns;
        std::string _cmd;



    public:
        static bool keep_running;
        Server(int port, std::string password);
        ~Server();
        void init();
        void build_and_listen();
        void add_nsocket();
        void receive_cmd(size_t &i, int current_fd);
        void handle_command(int fd, const std::string& cmd);
        Client* getClientByFd(int fd);
};
