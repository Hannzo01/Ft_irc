#include "../Server.hpp"

///* MODE <#channel> {[+|-]modechars} [param [param..]] *///
//Get modes: MODE #chan
//Set modes: MODE #chan +i
//Set password: MODE #chan +k password
//Set limit: MODE #chan +l 5
//Op user: MODE #chan +o nick
//Remove op: MODE #chan -o nick
//Remove key/limit: MODE #chan -k / -l


/*Who can change channel modes?

Only channel operators (@).*/


static bool isPositiveInteger(const std::string& s) {
    if (s.empty()) return false;
    for (size_t i = 0; i < s.size(); ++i)
        if (!isdigit(s[i])) return false;
    return true;
}

void Server::handleMode(Client* client, std::string param)
{
    std::istringstream iss(param);
    std::string channelName, modes;
    iss >> channelName >> modes;

    std::vector<std::string> args;
    std::string word;
    while (iss >> word)
        args.push_back(word);
    if (nickIsInUse(channelName) && modes == "+i")
        return;
    for (size_t i = 0; i < channelName.size(); i++)
        channelName[i] = std::tolower(channelName[i]);
    // Not enough parameters
    if (channelName.empty()) {
        std::string reply = ":localhost 461 " + client->getNick() + " MODE :Not enough parameters\r\n";
        client->sendRaw(reply);
        return;
    }

    // Is it a channel?
    if (channelName[0] != '#') {
        std::string reply = ":localhost 403 " + client->getNick() + " " + channelName + " :No such channel\r\n";
        client->sendRaw(reply);
        return;
    }
    // Remove #
    channelName = channelName.substr(1);

    // Channel exists?
    Channel* channel = getChannel(channelName);
    if (!channel) {
        std::string reply = ":localhost 403 " + client->getNick() + " #" + channelName + " :No such channel\r\n";
        client->sendRaw(reply);
        return;
    }

    // GET MODES if only channel is given
    if (modes.empty()) {
        std::string modeStr = channel->composeModeString(); // implement this to return e.g. "+itkl 10 mykey"
        std::string reply = ":localhost 324 " + client->getNick() + " #" + channelName + " " + modeStr + "\r\n";
        client->sendRaw(reply);
        return;
    }

    // Only ops can change channels
    if (!channel->isOperator(client)) {
        std::string reply = ":localhost 482 " + client->getNick() + " #" + channelName + " :You're not channel operator\r\n";
        client->sendRaw(reply);
        return;
    }

    bool adding = true;
    size_t argi = 0;
    for (size_t i = 0; i < modes.size(); ++i) {
        char c = modes[i];
        if (c == '+') { adding = true; continue; }
        if (c == '-') { adding = false; continue; }
        if (c == 'i') {
            channel->setInviteOnly(adding);
        }
        else if (c == 't') {
            channel->setTopicOpOnly(adding);
        }
        else if (c == 'o') {
            if (argi >= args.size()) {
                std::string reply = ":localhost 461 " + client->getNick() + " MODE :Not enough parameters\r\n";
                client->sendRaw(reply);
                break;
            }
            std::string targetNick = args[argi++];
            Client* target = NULL;
            for (size_t j = 0; j < _clients.size(); ++j)
                if (_clients[j]->getNick() == targetNick)
                    target = _clients[j];
            if (!target || !channel->hasMember(target)) {
                std::string reply = ":localhost 441 " + client->getNick() + " " + targetNick + " #" + channelName + " :They aren't on that channel\r\n";
                client->sendRaw(reply);
                continue;
            }
            if (adding)
                channel->addOperator(target);
            else
                channel->removeOperator(target);
        }
        else if (c == 'k') {
            if (adding) {
                if (argi >= args.size() || args[argi].empty()) {
                    std::string reply = ":localhost 461 " + client->getNick() + " MODE :Not enough parameters\r\n";
                    client->sendRaw(reply);
                    break;
                }
                channel->setKey(args[argi++]);
            } else {
                channel->removeKey();
            }
        }
        else if (c == 'l') {
            if (adding) {
                if (argi >= args.size() || !isPositiveInteger(args[argi])) {
                    std::string reply = ":localhost 461 " + client->getNick() + " MODE :Not enough parameters\r\n";
                    client->sendRaw(reply);
                    break;
                }
                int lim = atoi(args[argi++].c_str());
                channel->setLimit(lim);
            } else {
                channel->removeLimit();
            }
        }
        else {
            // Unknown mode char
            std::string reply = ":localhost 472 " + client->getNick() + " " + std::string(1, c) + " :is unknown mode char\r\n";
            client->sendRaw(reply);
        }
    }
}