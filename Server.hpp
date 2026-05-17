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
#include <sstream>
#include "Channel.hpp"



class Client;
class Channel;
class Server
{
    private:
        std::vector<Client *> clients;
        std::map<std::string, Channel*> _channels;

        int _port;
        std::string _pass;
        int _ss;
        struct pollfd _spfd;
        std::map<int, std::string> _fds_buff;
        std::vector<struct pollfd> _v;
        int _ns;
        std::string _cmd;
        


        static bool are_equal(const std::string& a,const std::string& b);
        
        // Commands
        void handlePass(Client* client, std::string param);
        void handleNick(Client* client, std::string param);
        void handleUser(Client* client, std::string param);
        void handlePing(Client* client, std::string param);
        void handlePrivmsg(Client* client, std::string param);
        void handleQuit(Client* client);
        // void handlePong(Client* client, std::string param);
        

        // void handleJoin(Client* client, std::string param);
        // void handleTopic(Client* client, std::string param);
        // void handleInvite(Client* client, std::string param);
        // void handleMode(Client* client, std::string param);
        // void handleKick(Client* client, std::string param);

    public:
        static bool keep_running;
        Server(int port, std::string password);
        ~Server();
        void init();
        void build_and_listen();
        void add_nsocket();
        void receive_cmd(size_t &i, int current_fd);
        void handle_command(int fd, std::string& line);
        Client* getClientByFd(int fd);

        /*la plus inportant*/
        std::string getPass();
        bool nickIsInUse(std::string nickname)const;
        void sendReply(Client* client, const std::string& code, 
                      const std::string& nick, 
                      const std::string& arg, 
                      const std::string& message);


        static bool is_command(std::string command);
        static bool checkPassword(std::string& param);
        static bool isValidNick(const std::string& nick);
        static void printFtIrcBanner();
};
