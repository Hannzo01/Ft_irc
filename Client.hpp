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
    void setHost(const std::string& host);
    void setRealName(const std::string& realName);

    void setPassFilled(bool passfilled);
    void setNickFilled(bool nickffiled);
    void setHasUser(bool hasUser);

    void setAuthorized(bool authorized);
    void setCurrentChannel(Channel* channel);

    // Getters
    int getFd() const;
    const std::string& getNick() const;
    const std::string& getUser() const;
    const std::string& getHost() const;
    bool getpassFilled()const;
    bool gethasUser()const;
    bool getnickFilled() const;
    bool getisAuthorized() const;
    
    Channel* getCurrentChannel() const;

    // Channel list management
    void joinChannel(Channel* channel);
    void leaveChannel(Channel* channel);
    const std::vector<Channel*>& getChannels() const;

    void sendRaw(const std::string& msg);
private:
    int _fd;                         // Socket file descriptor
    std::string _nickname;
    std::string _username;
    std::string _realName;
    std::string _host;

    bool _passFilled;
    bool _nickFilled;
    bool _hasUser;

    bool _authorized;
    std::vector<Channel*> _joinedChannels;
    Channel* _currentChannel;
};

#endif