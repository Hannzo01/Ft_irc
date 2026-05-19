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

        std::vector<Client *>           clients;
        std::vector<struct pollfd>      _pollFds;
        std::map<int, std::string>      _clientBuffers;
        std::map<std::string, Channel*> _channels;

        int             _port;
        int             _serverSocket;
        struct pollfd   _serverPollFd;
        std::string     _password;
        

        static bool are_equal(const std::string& a,const std::string& b);

        
        // Commands
        void    handlePass(Client* client, std::string param);
        void    handleNick(Client* client, std::string param);
        void    handleUser(Client* client, std::string param);
        void    handlePing(Client* client, std::string param);
        void    handlePrivmsg(Client* client, std::string param);
        void    handleQuit(Client* client);
        // void handlePong(Client* client, std::string param);
        void    handleJoin(Client* client, std::string param);
        void    handleTopic(Client* client, std::string param);
        void    handleKick(Client* client, std::string param);
        void handleInvite(Client* client, std::string param);
        void handleMode(Client* client, std::string param);

    public:

        Server(int port, std::string password);
        ~Server();

        void    setupSocket();
        void    runEventLoop();
        void    acceptNewConnection();
        void    readDataFromClient(size_t &i, int current_fd);
        void    handle_command(int fd, std::string& line);


        bool        nickIsInUse(std::string nickname)const;
        Client*     getClientByFd(int fd);        
        std::string getPassword();

        
        void sendReply(Client* client, const std::string& code, 
            const std::string& nick, 
            const std::string& arg, 
            const std::string& message);
        
        
        void    clean();
        void    disconnectClient(size_t &i, int current_fd);


        static bool     keepRunning;
        static bool     is_command(std::string command);
        static bool     checkPassword(std::string& param);
        static bool     isValidNick(const std::string& nick);
        static void     printFtIrcBanner();
        static void     signal_handler(int signum);
        static int      parse_port(std::string a1);
        
        ///*channels handler*///
        Channel*    getChannel(std::string name) const;
        void        addChannel(std::string chname, Channel* newChannel);

};
