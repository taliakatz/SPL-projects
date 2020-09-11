#include "../include/Watchable.h"
#include "../include/Action.h"
#include "../include/User.h"
#include "../include/Session.h"
#include <vector>

Watchable::Watchable(): id(-1),length(-1),tags(){}

Watchable::Watchable(long _id, int _length, const std::vector<std::string> &_tags) :id(_id),length(_length),tags(_tags){}

long Watchable::getId() const {
    return id;
}

int Watchable::getLength() const {
    return length;
}

std::vector<std::string> Watchable::getTags() const {
    return tags;
}

std::string Watchable::tagsToString() {
    std::string ret = "[";
    for(size_t i = 0; i < getTags().size(); i++) {
        if(i < getTags().size()-1)
            ret = ret + getTags()[i] + ",";
        else
            ret = ret + getTags()[i];
    }
    ret = ret + "]";
    return ret;
}
///////////////
Movie:: Movie(): Watchable(),name(nullptr){}

Movie::Movie(long _id, const std::string& _name, int _length, const std::vector<std::string>& _tags): Watchable(_id,_length,_tags),name(_name){}

std::string Movie::toString() const {
    return name;
}

std::string Movie::tagsToString() {
    std::string ret = "[";
    for(size_t i = 0; i < getTags().size(); i++) {
        if(i < getTags().size()-1)
            ret = ret + getTags()[i] + ",";
        else
            ret = ret + getTags()[i];
    }
    ret = ret + "]";
    return ret;
}

Watchable* Movie::getNextWatchable(Session &sess) const {
   return getRecAfterMe(sess);
   //return (sess.getActiveUser()->getRecommendation(sess));  //keep like this

}
Watchable* Movie::getRecAfterMe(Session &s) const{
    return s.getActiveUser()->recByAlgo(s);
}

bool Movie::hasTag(std::string tag) {
    for(size_t i = 0;i< this->getTags().size(); i++){
        if(this->getTags()[i] == tag)
            return true;
    }
    return false;
}
void Movie::setNextEpisodeId(long) {
}

Movie* Movie::clone() {
    return new Movie(this->getId(),this->name,this->getLength(),this->getTags());

}
///////

Episode::Episode(): Watchable(), seriesName(""), season(-1), episode(-1), nextEpisodeId(-1){} //deafult con

Episode::Episode(long _id, const std::string &_seriesName, int _length, int _season, int _episode,
                 const std::vector<std::string> &_tags) : Watchable(_id, _length, _tags), seriesName(_seriesName),season(_season),episode(_episode), nextEpisodeId(-1) {
}


std::string Episode::toString() const {
    std::string str = seriesName;
    if(season<10)
        str=str+" S0"+std::to_string(season);
    else
        str=str+" S"+std::to_string(season);
    if(episode<10)
        str=str+"E0"+std::to_string(episode);
    else
        str=str+"E"+std::to_string(episode);
    return str;
}

std::string Episode::tagsToString() {
    std::string ret="[";
    for(size_t i = 0; i < getTags().size();i++) {
        if(i < getTags().size()-1)
            ret = ret + getTags()[i] + ",";
        else
            ret = ret + getTags()[i];
    }
    ret = ret + "]";
    return ret;
}

Watchable* Episode::getRecAfterMe(Session &s) const{
    if(nextEpisodeId == -1)
        return s.getActiveUser()->recByAlgo(s);
    else
        return s.getContent()[getNextEpisodeId()-1];
}

void Episode::setNextEpisodeId(long newId) {
    nextEpisodeId=newId;
}
Watchable* Episode::getNextWatchable(Session &sess) const {
    if(nextEpisodeId != -1)
        return sess.getContent()[nextEpisodeId];
    else
        return sess.getActiveUser()->getRecommendation(sess);
}
long Episode::getNextEpisodeId() const {
    return nextEpisodeId;
}
bool Episode::hasTag(std::string tag) {
    for(size_t i = 0; i< this->getTags().size(); i++){
        if(this->getTags()[i] == tag)
            return true;
    }
    return false;
}

Episode* Episode::clone() {
    return new Episode(this->getId(),this->seriesName,this->getLength(),this->season,this->episode,this->getTags());
}
