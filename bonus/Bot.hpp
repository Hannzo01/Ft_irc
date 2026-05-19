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
#include <cstdlib>
#include <ctime>
#include <map>

class Bot
{
    private:
        std::string _ip;
        int _port;
        std::string _password;

    public:
        Bot(std::string ip, int port, std::string pass);
        ~Bot();
        void    setupSocket();

        static  void        parse_input(std::string ip, std::string port, std::string password);
        static  void        printSnowbotBanner();
        static  std::string get_a_random_joke();
};

#endif

//handle mochkil ctrl c to the server will cause an infsetupSockete loop in the bot