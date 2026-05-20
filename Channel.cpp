
// Channel.cpp
#include "Server.hpp"
#include "Channel.hpp"
#include "Client.hpp"
#include <algorithm>

Channel::Channel(const std::string& name)
    : _name(name),
      _topic(""),
      _inviteOnly(false),
      _topicOpOnly(false),
      _key(""),
      _keyEnabled(false),
      _userLimit(0),
      _limitEnabled(false)
{}

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

void Channel::removeOperator(Client* client) {
    std::vector<Client*>::iterator it =
        std::find(_operators.begin(), _operators.end(), client);
    if (it != _operators.end())
        _operators.erase(it);
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
            if (send(_members[i]->getFd(), message.c_str(), message.size(), 0) < 0)
                throw std::runtime_error("send failed");
        }
    }
}

std::string Channel::composeModeString() const {
    std::string modes = "+";
    std::string params;

    // +i (invite-only)
    if (_inviteOnly)
        modes += "i";
    // +t (topic only by op)
    if (_topicOpOnly)
        modes += "t";
    // +k (password/key set)
    if (_keyEnabled)
        modes += "k";
    // +l (user limit set)
    if (_limitEnabled)
        modes += "l";

    // Order of params follows order of mode chars that need arguments (+k +l)
    if (_keyEnabled)
        params += " " + _key;
    if (_limitEnabled) {
        std::ostringstream oss;
        oss << _userLimit;
        params += " " + oss.str();
    }

    return modes + params;
}

void Channel::broadcast(const std::string& message) {
    for (size_t i = 0; i < _members.size(); ++i) {
        _members[i]->sendRaw(message);
    }
}

// +i invite-only mode
void Channel::setInviteOnly(bool enabled) {
    _inviteOnly = enabled;
}
bool Channel::isInviteOnly() const {
    return _inviteOnly;
}

// +t topic settable by ops only
void Channel::setTopicOpOnly(bool enabled) {
    _topicOpOnly = enabled;
}
bool Channel::isTopicOpOnly() const {
    return _topicOpOnly;
}

// +k key (password) mode
void Channel::setKey(const std::string& key) {
    _key = key;
    _keyEnabled = true;
}
void Channel::removeKey() {
    _key.clear();
    _keyEnabled = false;
}
bool Channel::isKeyEnabled() const {
    return _keyEnabled;
}
const std::string& Channel::getKey() const {
    return _key;
}

// +l user limit mode
void Channel::setLimit(int lim) {
    _userLimit = lim;
    _limitEnabled = true;
}
void Channel::removeLimit() {
    _userLimit = 0;
    _limitEnabled = false;
}
bool Channel::isLimitEnabled() const {
    return _limitEnabled;
}
int Channel::getLimit() const {
    return _userLimit;
}

// Invite list logic for +i channels
void Channel::addInvite(Client* c) {
    if (!isInvited(c))
        _invited.push_back(c);
}

bool Channel::isInvited(Client* c) const {
    for (size_t i = 0; i < _invited.size(); ++i)
        if (_invited[i] == c)
            return true;
    return false;
}

// Remove after successful JOIN
void Channel::removeInvite(Client* c) {
    for (std::vector<Client*>::iterator it = _invited.begin(); it != _invited.end(); ++it)
        if (*it == c) {
            _invited.erase(it);
            break;
        }
}