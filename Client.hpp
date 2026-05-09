#ifndef CLIENT_HPP
#define CLIENT_HPP

// Client.hpp

#include <string>
#include <vector>

class Channel; // Forward declaration

class Client {
public:
    Client(int fd);
    ~Client();

    // Setters
    void setNick(const std::string& nick);
    void setUser(const std::string& user);
    void setAuthorized(bool authorized);
    void setCurrentChannel(Channel* channel);

    // Getters
    int getFd() const;
    const std::string& getNick() const;
    const std::string& getUser() const;
    bool isAuthorized() const;
    Channel* getCurrentChannel() const;
    std::string& getBuffer();

    // Channel list management
    void joinChannel(Channel* channel);
    void leaveChannel(Channel* channel);
    const std::vector<Channel*>& getChannels() const;

private:
    int _fd;                         // Socket file descriptor
    std::string _nickname;
    std::string _username;
    bool _authorized;
    std::string _buffer;             // Used for partial reads

    std::vector<Channel*> _joinedChannels;
    Channel* _currentChannel;
};

#endif