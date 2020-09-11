#ifndef SESSION_H_
#define SESSION_H_

#include "Action.h"
#include "User.h"
#include "Watchable.h"
#include <iostream>
#include <vector>
#include <unordered_map>
#include <string>

class User;
class Watchable;
class BaseAction;

class Session {
public:
    Session();
    Session(const std::string &configFilePath);
    ~Session();
    Session(const Session& other);
    Session& operator=(const Session& other);
    Session(Session&& other);
    Session& operator=(Session &&other);
    void start();
    std::unordered_map<std::string, User *> &getUserMap();
    std::vector<Watchable*> &getContent();
    std::vector<BaseAction*> &getActionsLog();
    User *getActiveUser() const;
    void setActiveUser(User *);
    void setTerminated(bool);
    std::vector<std::string> splitString(std::string);
    void clear();
    void myCopy(const Session&);
    Watchable* placeInHistory (Watchable &);
private:
    std::vector<Watchable *> content;
    std::vector<BaseAction *> actionsLog;
    std::unordered_map<std::string, User *> userMap;
    User *activeUser;
    bool terminated;
};
#endif

