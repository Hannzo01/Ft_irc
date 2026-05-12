#include "Bot.hpp"

int main(int argc, char *argv[])
{
    if (argc != 4){
        std::cerr << "Usage : ./ircbot <ip> <port> <passowrd>" << std::endl;
        return 1;
    }

    try{
        //parse linput
        Bot hannzo(argv[1], atoi(argv[2]), argv[3]);
        hannzo.init();
    }
    catch(std::exception& e)
    {
        std::cerr << e.what() << std::endl;
    }
}