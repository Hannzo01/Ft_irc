#pragma 

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

class Server
{
    private:

    public:
        Server(int port, std::string password);
        ~Server();
        void parse_port(std::string a1);

};