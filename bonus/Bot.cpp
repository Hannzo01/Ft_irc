#include "Bot.hpp"

Bot::Bot( std::string ip, int port, std::string pass) : _ip(ip) , _port(port), _pass(pass)  {
}

Bot::~Bot(){
}
void Bot::printSnowbotBanner() 
{
    std::cout << "\033[1;37m";
    std::cout << std::endl;
    std::cout << " ʰᵉʸ ⁱ ᵃᵐ ˢⁿᵒʷᵇᵒᵗ \033[1;22m"  <<std::endl;
    std::cout << "      .-\"\"-.      " << std::endl;
    std::cout << "     / ,  , \\     " << std::endl;
    std::cout << "    |  \\__/  |    " << std::endl;
    std::cout << "     \\_    _/     " << std::endl;
    std::cout << "     /      \\     " << std::endl;
    std::cout << "    |        |    " << std::endl;
    std::cout << "     \\      /     " << std::endl;
    std::cout << "      `----`      " << std::endl;
    std::cout << "\033[0m" << std::endl; // Remet la couleur par défaut
}

std::string Bot::get_a_random_joke(){
 std::string jokes[4];
    jokes[0] = "Why was the math book sad? Because it had too many problems!";
    jokes[1] = "Why did the computer go to the doctor? Because it had a virus!";
    jokes[2] = "What do u call a bear with no teeth? A gummy bear!";
    jokes[3] = "What did the ocean say to the beach? Nothing, it just waved.";

    int num = rand() % 4;

    return jokes[num];
}

void Bot::init()
{
    srand(time(NULL));
    int ClientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (ClientSocket == -1)
        throw std::runtime_error("Socket failed");

    struct sockaddr_in Server_addr;
    memset(&Server_addr, 0, sizeof(Server_addr));
    Server_addr.sin_addr.s_addr = inet_addr(_ip.c_str());
    Server_addr.sin_port = (htons(_port));
    Server_addr.sin_family = (AF_INET);

    if (connect(ClientSocket, (sockaddr *)&Server_addr, sizeof(Server_addr)) == -1) {
            close(ClientSocket);
            throw std::runtime_error("Client failed to connect to server");
        }


    std::string PASS_cmd = "PASS " + _pass + "\r\n";
    send(ClientSocket, PASS_cmd.c_str(), PASS_cmd.length(), 0);

    std::string NICK_cmd = "NICK Snowbot\r\n";
    send(ClientSocket, NICK_cmd.c_str(), NICK_cmd.length(), 0);

    // USER <username> <hostname> <servername> :<realname>
    std::string USER_cmd = "USER Snowbot 0 * : Hey I am snowbot !\r\n";
    send(ClientSocket, USER_cmd.c_str(), USER_cmd.length(), 0);

    std::string buffer;
    std::string message;
   printSnowbotBanner();

    while (1)
    {
        char buff[1024];
        memset(buff, 0, sizeof(buff));
        int byteread = recv(ClientSocket, buff, 1023, 0);
        if (byteread <= 0)
            throw std::runtime_error("Server disconnected or error while receiving");
            
        buffer = buffer.append(buff, byteread);
        std::cout << buffer << std::endl; // n7ydha mn b3d
        size_t pos;
        while ((pos = buffer.find("\r\n")) != std::string::npos)
        {

            std::string line = buffer.substr(0, pos + 2);
            std::cout << "[DEBUG] : " << line;


            if (line.find("PING") != std::string::npos)
            {
                std::string pong_msg = "PONG :localhost\r\n"; 
                send(ClientSocket, pong_msg.c_str(), pong_msg.size(), 0);
            }
            size_t text_start = line.find(" :");
            if (text_start != std::string::npos)
            {

                std::string text = line.substr(text_start + 2);

                if (text == "!joke\r\n" || text.find("!joke ") == 0)
                {
                    size_t colon = line.find(":");
                    size_t exclamation_mark = line.find("!");

                    if (colon != std::string::npos && exclamation_mark != std::string::npos && colon < exclamation_mark)
                    {
                        std::string target = line.substr(colon + 1, exclamation_mark - colon - 1);
                        std::string joke = get_a_random_joke();
                        message = "PRIVMSG " + target + " :" + joke + "\r\n";
                        send(ClientSocket, message.c_str(), message.size(), 0);
                    }
                }
            }

            buffer.erase(0, pos + 2);
        }
    }
}