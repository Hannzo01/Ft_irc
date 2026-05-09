// Client.cpp

#include "Client.hpp"
#include "Channel.hpp"
#include <algorithm>

Client::Client(int fd)
    : _fd(fd), _authorized(false), _passFilled(false),_nickFilled(false), _hasUser(false), _currentChannel(NULL) {}

Client::~Client() {}

void Client::setNick(const std::string& nick) { _nickname = nick; }
void Client::setUser(const std::string& user) { _username = user; }
void Client::setAuthorized(bool authorized) { _authorized = authorized; }
void Client::setCurrentChannel(Channel* channel) { _currentChannel = channel; }

int Client::getFd() const { return _fd; }
const std::string& Client::getNick() const { return _nickname; }
const std::string& Client::getUser() const { return _username; }
bool Client::isAuthorized() const { return _authorized; }
Channel* Client::getCurrentChannel() const { return _currentChannel; }
std::string& Client::getBuffer() { return _buffer; }

void Client::joinChannel(Channel* channel) {
    if (std::find(_joinedChannels.begin(), _joinedChannels.end(), channel) == _joinedChannels.end())
        _joinedChannels.push_back(channel);
}

void Client::leaveChannel(Channel* channel) {
    std::vector<Channel*>::iterator it = std::find(_joinedChannels.begin(), _joinedChannels.end(), channel);
    if (it != _joinedChannels.end())
        _joinedChannels.erase(it);
}

const std::vector<Channel*>& Client::getChannels() const { return _joinedChannels; }

bool Client::hasUser()const
{
    return _hasUser;
}
bool Client::passFilled()const
{
    return _passFilled;
}
void Client::setRealName(const std::string& realName)
{
    _realName = realName;
}
void Client::setHasUser(bool hasUser)
{
    _hasUser = hasUser;
}
void Client::setPassFilled(bool passfilled)
{
    _passFilled = passFilled;
}
bool Client::nickFilled() const
{
    return _nickFilled;
}
void Client::setNickFilled(bool nickffiled)
{
    _nickFilled = nickffiled;
}
