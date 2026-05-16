
// Channel.cpp
#include "Server.hpp"
#include "Channel.hpp"
#include "Client.hpp"
#include <algorithm>

Channel::Channel(const std::string& name) : _name(name) {}

Channel::~Channel() {}

void Channel::addMember(Client* client) {
    if (std::find(_members.begin(), _members.end(), client) == _members.end())
        _members.push_back(client);
}
void Channel::removeMember(Client* client) {
    std::vector<Client*>::iterator it = std::find(_members.begin(), _members.end(), client);
    if (it != _members.end())
        _members.erase(it);
}
bool Channel::hasMember(Client* client) const {
    return std::find(_members.begin(), _members.end(), client) != _members.end();
}
const std::string& Channel::getName() const { return _name; }
const std::string& Channel::getTopic() const { return _topic; }
void Channel::setTopic(const std::string& topic) { _topic = topic; }

void Channel::addOperator(Client* client) {
    if (std::find(_operators.begin(), _operators.end(), client) == _operators.end())
        _operators.push_back(client);
}
bool Channel::isOperator(Client* client) const {
    return std::find(_operators.begin(), _operators.end(), client) != _operators.end();
}
const std::vector<Client*>& Channel::getMembers() const { return _members; }

void Channel::broadcast_msg(Client* client, std::string message)
{
    for (size_t i = 0; i < _members.size(); i++)
    {
        if (_members[i]->getFd() != client->getFd()){
            send(_members[i]->getFd(), message.c_str(), message.size(), 0);
        }
    }
}
