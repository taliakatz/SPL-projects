#ifndef USER_H_
#define USER_H_
#include "Action.h"
#include "Session.h"
#include "Watchable.h"
#include <iostream>
#include <vector>
#include <unordered_set>
#include <unordered_map>
#include <string>



class Watchable;
class Action;

class User{
public:
    User();
    User(std::string name);
    virtual ~User();
    User(const User &other);
    User& operator=(const User &other);
    User(User&& other);
    User& operator=(User &&other);
    virtual User* clone()=0;
    virtual Watchable* getRecommendation(Session& s) = 0;
    virtual bool isInHistory(Watchable* w)=0;
    virtual Watchable* recByAlgo(Session&)=0;
    std::vector<Watchable*> &getHistory();
    const std::string &getName() const;
    void setName(std::string newName );
    virtual void setAverage()=0;
    virtual void setTagsMap(Watchable*)=0;
    void clear();
    void myCopy(const User&);
protected:
    std::vector<Watchable*> history;
private:
    std::string name;

};

class LengthRecommenderUser : public User {
public:
    LengthRecommenderUser();
    LengthRecommenderUser(std::string name);
    virtual ~LengthRecommenderUser() = default;
    LengthRecommenderUser (const LengthRecommenderUser &other) = default;
    LengthRecommenderUser& operator=(const LengthRecommenderUser &other) = default;
    LengthRecommenderUser(LengthRecommenderUser&& other) = default;
    LengthRecommenderUser& operator=(LengthRecommenderUser &&other) = default;
    virtual Watchable* recByAlgo(Session&);
    virtual void setAverage();
    virtual Watchable* getRecommendation(Session& s);
    bool isInHistory(Watchable *w);
    Watchable* findFirstNotInHistory(std::vector<Watchable*>,std::vector<Watchable*>);
    virtual User* clone();
    virtual void setTagsMap(Watchable*){};
private:
    double average;
};

class RerunRecommenderUser : public User {
public:
    RerunRecommenderUser();
    RerunRecommenderUser(std::string name);
    virtual ~RerunRecommenderUser() = default;
    RerunRecommenderUser(RerunRecommenderUser &other) = default;
    RerunRecommenderUser& operator=(RerunRecommenderUser &other) = default;
    RerunRecommenderUser(RerunRecommenderUser&& other) = default;
    RerunRecommenderUser& operator=(RerunRecommenderUser &&other) = default;
    virtual Watchable* recByAlgo(Session&);
    virtual Watchable* getRecommendation(Session& s);
    bool isInHistory(Watchable *w);
    virtual User* clone();
    virtual void setAverage(){};
    virtual void setTagsMap(Watchable*){};
private:
    int lastRecommendedInHistory;
};

class GenreRecommenderUser : public User {
public:
    GenreRecommenderUser();
    GenreRecommenderUser(std::string name);
    virtual ~GenreRecommenderUser() = default;
    GenreRecommenderUser(GenreRecommenderUser &other) = default;
    GenreRecommenderUser& operator=(GenreRecommenderUser &other) = default;
    GenreRecommenderUser(GenreRecommenderUser&& other) = default;
    GenreRecommenderUser& operator=(GenreRecommenderUser &&other) = default;
    virtual Watchable* getRecommendation(Session& s);
    virtual Watchable* recByAlgo(Session&);
    bool isInHistory(Watchable *w);
    virtual User* clone();
    std::unordered_map<std::string, int> &getTagsMap();
    std::string findMostPopTag(std::unordered_map<std::string,int>&);
    virtual void setAverage(){};
    virtual void setTagsMap(Watchable*);
private:
    std::unordered_map<std::string,int> tagsMap;
};

#endif
