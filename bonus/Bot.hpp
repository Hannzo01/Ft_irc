#ifndef BOT_HPP
#define BOT_HPP

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

class Bot{
    private:
        int _port;
        std::string _pass;
        std::string _ip;
    public:
    Bot(std::string ip ,int port,  std::string pass);
    ~Bot();
    void init();
};

#endif

//handle mochkil ctrl c to the server will cause an infinite loop in the bot