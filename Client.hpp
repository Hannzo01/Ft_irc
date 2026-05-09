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
    void setRealName(const std::string& realName);

    void setHasUser(bool hasUser);
    void setAuthorized(bool authorized);
    void setPassFilled(bool passfilled);
     void setNickFilled(bool nickffiled);
    void setCurrentChannel(Channel* channel);

    // Getters
    int getFd() const;
    const std::string& getNick() const;
    const std::string& getUser() const;

    bool isAuthorized() const;
    bool hasUser()const;
    bool passFilled()const;
    bool nickFilled() const;
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
    std::string _realName;
    bool _authorized;
    bool _hasUser;
    bool _passFilled;
    bool _nickFilled;
    std::string _buffer;             // Used for partial reads

    std::vector<Channel*> _joinedChannels;
    Channel* _currentChannel;
};

#endif