#include "../include/User.h"
#include "../include/Session.h"
#include "../include/Action.h"
#include "../include/Watchable.h"
#include <vector>
#include <string>
#include <unordered_set>
#include <unordered_map>

using namespace std;

User::User(): history(), name("default"){} //default constructor

User::User(string _name): history(), name(_name){}

User::~User() {
    clear();
}

User::User(const User &other): User(){
    myCopy(other);
}

User& User::operator=(const User &other) {
    if(this!=&other){
        clear();
        myCopy(other);
    }
    return *this;
}

User::User(User &&other): history(move(other.history)), name(move(other.name)) {
}

User& User::operator=(User &&other){
    if(this != &other){
        clear();
        history.swap(other.history);
        name = move(other.name);
    }
    return *this;
}

vector<Watchable*> &User::getHistory(){
    return history;
}
const string &User::getName() const {
    return name;
}
void User::setName(string _name) {
    name=std::move(_name);
}

void User::clear() {
    history.clear();

}

void User::myCopy(const User &other) {  //todo: here we changed from-deep copy to shellow copy in the history vector
    setName(other.name);
    for(auto& x: other.history){
        history.push_back(x);
    }
}

//////////////////////////////////////////////////////////////////////

LengthRecommenderUser::LengthRecommenderUser(): User(),average(0){} //default cons

LengthRecommenderUser::LengthRecommenderUser(string name) :User(name), average(0){}



bool LengthRecommenderUser::isInHistory(Watchable *w) {

    for(size_t i =0; i < history.size(); i++) {
        if (history[i] == w)
            return true;
    }
    return false;
}

void LengthRecommenderUser::setAverage() {
    int sum=0;
    for(size_t i =0; i < history.size(); i++){
        sum= sum+history[i]->getLength();
    }
    average=sum /history.size();
}
Watchable* LengthRecommenderUser::findFirstNotInHistory(vector<Watchable *> content, vector <Watchable*>history) {
    for(size_t i =0; i < content.size(); i++){
        if(!isInHistory(content[i]))
            return content[i];
    }
    return nullptr;
}

Watchable* LengthRecommenderUser::recByAlgo(Session &s) {
    Watchable* output=findFirstNotInHistory(s.getContent(),history);
    if(output == nullptr)
        return nullptr;
    int minAvg = abs(average-output->getLength());
    for(size_t i =0; i < s.getContent().size(); i++) {
        if(abs(average-s.getContent()[i]->getLength())<minAvg && !isInHistory(s.getContent()[i])) {
            output=s.getContent()[i];
            minAvg=abs(average-output->getLength());
        }
    }
    return output;
}

Watchable* LengthRecommenderUser::getRecommendation(Session &s) {   //need to return pointer

    Watchable *last = s.getActiveUser()->getHistory()[history.size() - 1];
    return last->getRecAfterMe(s);
}

User* LengthRecommenderUser::clone() {
    return new LengthRecommenderUser(*this);
}

//////////////////////////////////////////////////////////////////////////////////////
RerunRecommenderUser::RerunRecommenderUser(): User(), lastRecommendedInHistory(-1) {}

RerunRecommenderUser::RerunRecommenderUser(string _name) :User(_name),lastRecommendedInHistory(0){}

Watchable* RerunRecommenderUser::getRecommendation(Session &s) {
    return recByAlgo(s);
}
Watchable* RerunRecommenderUser::recByAlgo(Session &) {
    Watchable* ret;
    if(history.size() != 0) {
       int tmp = lastRecommendedInHistory;
       lastRecommendedInHistory++;
        ret = history[tmp % history.size()];
    }
    return ret;
}

User* RerunRecommenderUser::clone() {
    return new RerunRecommenderUser(*this);

}

bool RerunRecommenderUser::isInHistory(Watchable* w) {

    for(size_t i = 0; i < history.size(); i++) {
        if (history[i] == w)
            return true;
    }
    return false;
}

//////////////////////////////////////////////////////

GenreRecommenderUser::GenreRecommenderUser(): User(),tagsMap(){}

GenreRecommenderUser::GenreRecommenderUser(string name): User(std::move(name)),tagsMap(){}

bool GenreRecommenderUser::isInHistory(Watchable *w) {

    for(size_t i = 0; i < history.size(); i++) {
        if (history[i] == w)
            return true;
    }
    return false;
}

unordered_map<string, int> &GenreRecommenderUser::getTagsMap() {
    return tagsMap;
}

string GenreRecommenderUser::findMostPopTag(unordered_map<string, int> &tmp) {
    string mostPopTagS =  "";
    int mostPopInt = 0;
    for(auto it = tmp.begin();it != tmp.end(); ++it){
        if(it->second > mostPopInt){
            mostPopInt = it-> second;
            mostPopTagS = it->first;
       }
        else if(it->second==mostPopInt){
            if(it->first.compare(mostPopTagS) < 0)
                mostPopTagS = it->first;
        }
    }
    return mostPopTagS;
}

Watchable* GenreRecommenderUser::getRecommendation(Session &s) {
    return recByAlgo(s);
}
Watchable* GenreRecommenderUser::recByAlgo(Session& s) {
    unordered_map<string, int> tmp = getTagsMap();
    unordered_map<string, int>::iterator it;
    Watchable* ret;
    for(it = tmp.begin(); it != tmp.end(); ++it) {
        string mostPopTag = findMostPopTag(tmp);
        for(size_t i = 0; i < s.getContent().size(); i++) {
            if (s.getContent()[i]->hasTag(mostPopTag)) {
                if(!isInHistory(s.getContent()[i]))
                    ret = s.getContent()[i];
            }
        }
        tmp.find(mostPopTag)->second = 0;
    }
    return ret;
}

User* GenreRecommenderUser::clone() {
    return new GenreRecommenderUser(*this);

}



void GenreRecommenderUser::setTagsMap(Watchable * w) {
    if(!isInHistory(w)) {
        vector<string> wTags = w->getTags();
        for (size_t i = 0; i < wTags.size(); i++) {
            if (tagsMap.find(wTags[i]) != tagsMap.end())
                tagsMap.find(wTags[i])->second++;
            else
                tagsMap.insert({{wTags[i], 1}});
        }
    }
}

