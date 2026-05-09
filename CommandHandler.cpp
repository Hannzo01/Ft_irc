#include "Server.hpp"
#include <sstream>

bool is_command(std::string command)
{
    return (command == "PASS" || command == "NICK" || command == "USER");
}

bool checkPassword(std::string param)
{
    // Remove any leading whitespace
    size_t first = param.find_first_not_of(" \t\r\n");
    if (first != std::string::npos)
        param = param.substr(first);

    // Remove leading ':' (IRC trailing param convention, e.g. PASS :hunter2)
    if (!param.empty() && param[0] == ':')
        param = param.substr(1);

    // Should not be empty
    if (param.empty())
        return false;

    // IRC line total length max (including CRLF) is 512
    if (param.length() > 510) // being strict, real limit is >510 with command
        return false;

    // Optionally, forbid spaces, but RFC allows them if param was given as trailing
    // If you want to forbid, uncomment this:
    // if (param.find(' ') != std::string::npos) return false;

    return true; // Good password parameter format!
}

void CommandHandler(Client *client, std::string line)
{
    std::string command;
    std::string param;
    std::istringstream iss(line);
    iss >> command;
    for (size_t i = 0; i < command.size(); ++i)
        command[i] = toupper(command[i]);
    size_t pos = line.find(' ');
    if (pos == std::string::npos)
        param =  "";
    else
        param =  line.substr(pos + 1);
    if (!client->isAuthorized())
    {
        if (!is_command(command) || param == "")
            std::cout << "Wrong command: try this forma: command parametr!" << std::endl;
        if (command == "PASS")
        {
            if (!param.empty() && param[0] == ':')
                param = param.substr(1);
            if (client->isAuthorized())
                std::cout << "Error: Already registered, PASS ignored.\n";
            else if (!checkPassword(param))
                std::cout << "Error: PASS needs a valid non-empty parameter.\n";
            else if (param != server->getPassword())
                std::cout << "Error: Password mismatch.\n";
            else
                client->setAuthorized(true);
        }
        else if (command == "NICK")
        {
            if (param.empty()) {
                // ERR_NEEDMOREPARAMS (461)
                std::cout << "Error: NICK needs a parameter." << std::endl;
            } else if (server->nickIsInUse(param)) {
                // ERR_NICKNAMEINUSE (433)
                std::cout << "Error: Nickname is already in use." << std::endl;
            } else if (!server->isValidNick(param)) {
                // ERR_ERRONEUSNICKNAME (432)
                std::cout << "Error: Nickname is invalid." << std::endl;
            } else {
                client->setNick(param);
            }
        }
        else if (command == "USER")
        {
            if (client->hasUser()) {
                // ERR_ALREADYREGISTERED (462)
                std::cout << "Error: USER already set." << std::endl;
            } else if (param.empty()) {
                // ERR_NEEDMOREPARAMS (461)
                std::cout << "Error: USER needs a parameter." << std::endl;
            } else {
                std::istringstream pss(param);
                std::string username, mode, unused, realname;
                pss >> username >> mode >> unused;
                std::getline(pss, realname);
                if (!realname.empty() && realname[0] == ':')
                    realname = realname.substr(1);
                client->setUser(username);
                client->setRealName(realname);
            }
        }
    }else
    {

    }
    std::cout << "      " << command << "   " << param << std::endl;
}