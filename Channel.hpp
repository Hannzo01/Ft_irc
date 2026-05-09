// Channel.hpp
#ifndef CHANNEL_HPP
#define CHANNEL_HPP
#include <string>
#include <vector>
#include <map>

class Client;  // Forward declaration

class Channel {
public:
    Channel(const std::string& name);
    ~Channel();

    // Member management
    void addMember(Client* client);
    void removeMember(Client* client);
    bool hasMember(Client* client) const;

    // Channel info
    const std::string& getName() const;
    const std::string& getTopic() const;
    void setTopic(const std::string& topic);

    // Modes, operators, invitees etc. can be added as needed
    void addOperator(Client* client);
    bool isOperator(Client* client) const;

    const std::vector<Client*>& getMembers() const;

private:
    std::string _name;
    std::string _topic;
    std::vector<Client*> _members;
    std::vector<Client*> _operators;
};

#endif /*CHANNEL_HPP*/