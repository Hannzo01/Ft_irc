#include "Bot.hpp"

Bot::Bot( std::string ip, int port, std::string pass) : _ip(ip) , _port(port), _pass(pass)  {
}

Bot::~Bot(){
}

void Bot::init()
{
    int ClientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (ClientSocket == -1)
        throw std::runtime_error("Socket failed");

    struct sockaddr_in Server_addr;
    memset(&Server_addr, 0, sizeof(Server_addr));
    Server_addr.sin_addr.s_addr = inet_addr(_ip.c_str());
    Server_addr.sin_port = (htons(_port));
    Server_addr.sin_family = (AF_INET);

    if (connect(ClientSocket, (sockaddr *)&Server_addr, sizeof(Server_addr)) == -1)
        std::cerr << "Client failed" << std::endl;


    std::string PASS_cmd = "PASS " + _pass + "\r\n";
    send(ClientSocket, PASS_cmd.c_str(), PASS_cmd.length(), 0);

    std::string NICK_cmd = "NICK Snowbot\r\n";
    send(ClientSocket, NICK_cmd.c_str(), NICK_cmd.length(), 0);

    // USER <username> <hostname> <servername> :<realname>
    std::string USER_cmd = "USER Snowbot 0 * : Hey I am snowbot !\r\n";
    send(ClientSocket, USER_cmd.c_str(), USER_cmd.length(), 0);

    std::string buffer;
    while (1){
        std::cout << "rni dkhltlhna awl mra" << std::endl;
        // std::string buff;
        char buff[1024];
        int byteread = recv(ClientSocket, buff, 1023, 0);
        if (byteread <= 0)
            throw std::runtime_error("Error while reciving");
        else
            buffer = buffer.append(buff, byteread);
        std::cout << buffer << std::endl;
        // :<expediteur>!<user>@<host> PRIVMSG snowbot :!joke\r\n
        // hna check wch kayn privmsg 
        // si oui nseftlo PRIVMSG <expediteur> :Pourquoi les plongeurs plongent-ils toujours en arrière et jamais en avant ? Parce que sinon ils tombent dans le bateau.\r\n
        

    }
}