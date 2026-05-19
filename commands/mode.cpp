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


void Server::handleMode(Client* client, std::string param)
{
    std::istringstream iss(param);
    std::string channelName, modes, arg, arg2;
    iss >> channelName >> modes >> arg >> arg2;

    // Not enough parameters 
    if (channelName.empty()) {
        std::string reply = ":localhost 461 " + client->getNick() + " MODE :Not enough parameters\r\n";
        client->sendRaw(reply);
        return;
    }

    // Is it a valid channel name? 
    if (channelName[0] != '#') {
        std::string reply = ":localhost 403 " + client->getNick() + " " + channelName + " :No such channel\r\n";
        client->sendRaw(reply);
        return;
    }
    channelName = channelName.substr(1);
    //  Channel exists? 
    Channel* channel = getChannel(channelName);
    if (!channel) {
        std::string reply = ":localhost 403 " + client->getNick() + " " + channelName + " :No such channel\r\n";
        client->sendRaw(reply);
        return;
    }

    //  GET MODES if only channel is given 
    if (modes.empty()) {
        std::string modeStr = /*channel->composeModeString()*/ "+nt"; // Replace with your mode string
        std::string reply = ":localhost 324 " + client->getNick() + " " + channelName + " " + modeStr + "\r\n";
        client->sendRaw(reply);
        return;
    }

    //  Only ops can change modes 
    if (!channel->isOperator(client)) {
        std::string reply = ":localhost 482 " + client->getNick() + " " + channelName + " :You're not channel operator\r\n";
        client->sendRaw(reply);
        return;
    }

    //  Handle mode setting/clearing 
    bool adding = true;
    // size_t argIdx = 0;
    std::vector<std::string> args;
    if (!arg.empty()) args.push_back(arg);
    if (!arg2.empty()) args.push_back(arg2);

    for (size_t i = 0, a = 0; i < modes.size(); ++i) {
        char c = modes[i];
        if (c == '+') adding = true;
        else if (c == '-') adding = false;
        else if (c == 'i') channel->setInviteOnly(adding);
        else if (c == 't') channel->setTopicOpOnly(adding);
        else if (c == 'o') {
            if (args.size() <= a) {
                // no target nick
                std::string reply = ":localhost 461 " + client->getNick() + " MODE :Not enough parameters\r\n";
                client->sendRaw(reply);
                break;
            }
            std::string targetNick = args[a++];
            Client* target = NULL;
            for (size_t j = 0; j < _clients.size(); ++j)
                if (_clients[j]->getNick() == targetNick)
                    target = _clients[j];
            if (!target || !channel->hasMember(target)) {
                // ERR_USERNOTINCHANNEL (441)
                std::string reply = ":localhost 441 " + client->getNick() + " " + targetNick + " " + channelName + " :They aren't on that channel\r\n";
                client->sendRaw(reply);
            } else {
                if (adding) channel->addOperator(target);
                // (removeOperator needed for -o)
            }
        }
        else if (c == 'k') {
            if (adding) {
                if (args.size() <= a) {
                    std::string reply = ":localhost 461 " + client->getNick() + " MODE :Not enough parameters\r\n";
                    client->sendRaw(reply);
                    break;
                }
                channel->setKey(args[a++]);
            } else
                channel->removeKey();
        }
        else if (c == 'l') {
            if (adding) {
                if (args.size() <= a) {
                    std::string reply = ":localhost 461 " + client->getNick() + " MODE :Not enough parameters\r\n";
                    client->sendRaw(reply);
                    break;
                }
                int lim = atoi(args[a++].c_str());
                channel->setLimit(lim);
            } else
                channel->removeLimit();
        }
        else {
            // Unknown mode
            std::string reply = ":localhost 472 " + client->getNick() + " " + std::string(1, c) + " :is unknown mode char\r\n";
            client->sendRaw(reply);
        }
    }
    // (Optionally: broadcast mode change to channel members)
}