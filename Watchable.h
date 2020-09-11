#ifndef WATCHABLE_H_
#define WATCHABLE_H_
#include "Action.h"
#include "User.h"
#include "Session.h"
#include <iostream>
#include <vector>
#include <unordered_map>
#include <string>


class Action;
class User;

class Watchable{
public:
    Watchable();
    Watchable(long id, int length, const std::vector<std::string>& tags);
    virtual ~Watchable(){};
    virtual std::string toString() const=0;
    virtual Watchable* getNextWatchable(Session&) const=0;
    long getId() const;
    int getLength()const;
    std::vector<std::string> getTags() const;
    virtual Watchable* getRecAfterMe(Session &s) const=0;
    virtual bool hasTag(std::string)=0;
    virtual std::string tagsToString()=0;
    virtual void setNextEpisodeId(long)=0;
    virtual Watchable* clone()=0;
private:
    const long id;
    int length;
    std::vector<std::string> tags;
};

class Movie : public Watchable{
public:
    Movie();
    Movie(long id, const std::string& name, int length, const std::vector<std::string>& tags);
    virtual ~Movie(){};
    virtual std::string toString() const;
    virtual Watchable* getNextWatchable(Session&) const;
    virtual Watchable* getRecAfterMe(Session &s) const;
    virtual bool hasTag(std::string);
    virtual std::string tagsToString();
    virtual void setNextEpisodeId(long);
    virtual Movie* clone();
private:
    std::string name;
};


class Episode: public Watchable{
public:
    Episode();
    Episode(long id, const std::string& seriesName,int length, int season, int episode ,const std::vector<std::string>& tags);
    virtual ~Episode(){};
    virtual std::string toString() const;
    virtual Watchable* getNextWatchable(Session&) const;
    virtual void setNextEpisodeId(long);
    long getNextEpisodeId() const;
    virtual Watchable* getRecAfterMe(Session &s) const;
    virtual bool hasTag(std::string);
    virtual std::string tagsToString();
    virtual Episode* clone();
private:
    std::string seriesName;
    int season;
    int episode;
    long nextEpisodeId;

};

#endif
