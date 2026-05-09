#include "Server.hpp"

int parse_port(std::string a1)
{
    int port = atoi(a1.c_str());
    for (size_t i = 0; i < a1.size(); i++)
    {
        if (!isdigit(a1[i]))
            throw std::logic_error("Valid ports are between 1024 and 65535");
    }
    if (port < 1024 || port > 65535)
        throw std::logic_error("Invalid port num");  
    else
        return port;
}

int main(int argc, char *argv[])
{
    // 1024kk
   if (argc != 3) {
        std::cerr << "Usage: ./ircserv <port> <password>" << std::endl;
        return 1;
    }
    try
    {
        int port = parse_port(argv[1]);
        if (strlen(argv[2]) == 0)
            throw std::logic_error("Enter a password");
        Server irc(port, argv[2]);

        irc.init();
        irc.build_and_listen();

    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
    
}