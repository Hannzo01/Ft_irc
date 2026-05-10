#include "Bot.hpp"

int main(int argc, char *argv[])
{
    if (argc != 4){
        std::cerr << "Usage : ./ircbot <ip> <port> <passowrd>" << std::endl;
        return 1;
    }

    try{
        //parse linput
        Bot hannzo;
        hannzo.init();
    }
    catch(std::exception& e)
    {
        std::cerr << e.what() << std::endl;
    }
}