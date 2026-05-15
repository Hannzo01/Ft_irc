// Client.cpp

#include "Client.hpp"
#include "Channel.hpp"
#include <algorithm>
#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
Client::Client(int fd)
    : _fd(fd), _nickname("*"), _passFilled(false),_nickFilled(false),
    _hasUser(false),_authorized(false),
    _currentChannel(NULL) {}

Client::~Client() {}

void Client::setNick(const std::string& nick) { _nickname = nick; }
void Client::setUser(const std::string& user) { _username = user; }
void Client::setAuthorized(bool authorized) { _authorized = authorized; }
void Client::setCurrentChannel(Channel* channel) { _currentChannel = channel; }

int Client::getFd() const { return _fd; }
const std::string& Client::getNick() const { return _nickname; }
const std::string& Client::getUser() const { return _username; }
bool Client::getisAuthorized() const { return _authorized; }
Channel* Client::getCurrentChannel() const { return _currentChannel; }

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

bool Client::gethasUser()const
{
    return _hasUser;
}
bool Client::getpassFilled()const
{
    return _passFilled;
}
const std::string& Client::getHost() const
{
    return _host;
}

void Client::setRealName(const std::string& realName)
{
    _realName = realName;
}
void Client::setHost(const std::string& host)
{
    _host = host;
}
void Client::setHasUser(bool hasUser)
{
    _hasUser = hasUser;
}
void Client::setPassFilled(bool passFilled)
{
    _passFilled = passFilled;
}
bool Client::getnickFilled() const
{
    return _nickFilled;
}
void Client::setNickFilled(bool nickffiled)
{
    _nickFilled = nickffiled;
}

void Client::sendRaw(const std::string& msg) {
    // Note: Doesn't handle partial writes or socket errors robustly!
    ssize_t result = send(_fd, msg.c_str(), msg.length(), 0);
    if (result < 0) {
        // Optionally, log or handle error here
        std::cerr << "sendRaw failed" << std::endl;
    }
}