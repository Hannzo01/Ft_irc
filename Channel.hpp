// Channel.hpp
#ifndef CHANNEL_HPP
#define CHANNEL_HPP
#include <string>
#include <vector>
#include <map>

class Client;  // Forward declaration


/*// Channel.hpp (add to private section)
bool _inviteOnly;      // +i invite-only mode
bool _topicOpOnly;     // +t topic settable by ops only
std::string _key;      // +k channel key
bool _keyEnabled;      // Is +k mode enabled?
int _userLimit;        // +l user limit value
bool _limitEnabled;    // Is +l mode enabled?
std::vector<Client*> _inviteList; // for handling invites

// Add public getters/setters as needed
void setInviteOnly(bool enabled);
bool isInviteOnly() const;

void setTopicOpOnly(bool enabled);
bool isTopicOpOnly() const;

void setKey(const std::string& key);
void removeKey();
bool isKeyEnabled() const;
const std::string& getKey() const;

void setLimit(int lim);
void removeLimit();
bool isLimitEnabled() const;
int getLimit() const;

void addInvite(Client* client);
bool isInvited(Client* client) const;
void clearInvite(Client* client);*/



class Channel {
public:
    Channel(const std::string& name);

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
    void removeOperator(Client* client);
    bool isOperator(Client* client) const;


    void broadcast_msg(Client*client,  std::string message);

    const std::vector<Client*>& getMembers() const;


    void setInviteOnly(bool enabled);
    bool isInviteOnly() const;

    void setTopicOpOnly(bool enabled);
    bool isTopicOpOnly() const;

    void setKey(const std::string& key);
    void removeKey();
    bool isKeyEnabled() const;
    const std::string& getKey() const;

    void setLimit(int lim);
    void removeLimit();
    bool isLimitEnabled() const;
    int getLimit() const;


    std::string composeModeString() const;
    void broadcast(const std::string& message);

    void addInvite(Client* c);
    bool isInvited(Client* c) const;
    void removeInvite(Client* c);

private:
    std::string _name;
    std::string _topic;
    std::vector<Client*> _members;
    std::vector<Client*> _operators;
    std::vector<Client*> _invited;

    bool _inviteOnly;      // +i invite-only mode
    bool _topicOpOnly;     // +t topic settable by ops only
    std::string _key;      // +k channel key
    bool _keyEnabled;      // Is +k mode enabled?
    int _userLimit;        // +l user limit value
    bool _limitEnabled;    // Is +l mode enabled?
    std::vector<Client*> _inviteList; // for handling invites


};

#endif /*CHANNEL_HPP*/