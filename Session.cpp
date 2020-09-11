#include "../include/Session.h"
#include "../include/json.hpp"
#include <cstring>
#include <vector>
#include <iostream>
#include <fstream>
#include <unordered_map>

using namespace std;

Session::Session():content(),actionsLog(),userMap(),activeUser(nullptr) ,terminated(false){} //deafult constructor

Session::Session(const std::string &configFilePath): content(),actionsLog(),userMap(), activeUser(new LengthRecommenderUser("default")), terminated(false){ //add all the fields
    userMap.insert({activeUser->getName(),activeUser});
    using json = nlohmann::json;
    std::ifstream i(configFilePath);                   
    json j;
    i >> j;
    long id =0;
    long movies = (j.at("movies")).size();
    for(long l = 0; l < movies; ++l){
        std::string name = j.at("movies")[l].at("name");
        int length = j.at("movies")[l].at("length");
        std::vector<std::string> tags;
        for(size_t tag = 0; tag<j.at("movies")[l].at("tags").size(); ++tag){
            tags.push_back(j.at("movies")[l].at("tags")[tag]);
        }

        content.push_back(new Movie(id, name, length, tags));
        id++;
    }

    for(size_t series = 0; series < j.at("tv_series").size(); ++series){
        std::string seriesName = j.at("tv_series")[series].at("name");
        int length = j.at("tv_series")[series].at("episode_length");
        std::vector<std::string> tags;
        for(size_t tag = 0; tag<j.at("tv_series")[series].at("tags").size(); ++tag){
            tags.push_back(j.at("tv_series")[series].at("tags")[tag]);
        }
        for(size_t season = 0; season < j.at("tv_series")[series].at("seasons").size(); ++season){
            int episodes = j.at("tv_series")[series].at("seasons")[season];
            for(int k = 0; k < episodes; ++k){
                Watchable* ep = new Episode(id, seriesName, length, season+1, k+1, tags);
                content.push_back(ep);
                if((season==j.at("tv_series")[series].at("seasons").size()-1) & (k == episodes-1))
                    ep->setNextEpisodeId(-1);
                else
                    ep->setNextEpisodeId(id+1);
                id++;
            }

        }
    }
}
Session::~Session() {
    clear();
}

Session::Session(const Session &other): Session() {
   myCopy(other);

}

Session& Session::operator=(const Session& other) {
    if(this!=&other){
        clear();
        this->myCopy(other);
    }
    return *this;
}

Session::Session(Session &&other): content(move(other.content)), actionsLog(move(other.actionsLog)), userMap(move(other.userMap)), activeUser(move(other.activeUser)), terminated(move(other.terminated)) {
    other.activeUser= nullptr;
}

Session& Session::operator=(Session &&other) {
    if(this != &other) {
        clear();
        content.swap(other.content);
        actionsLog.swap(other.actionsLog);
        userMap.swap(other.userMap);
        activeUser = other.activeUser;
        other.activeUser = nullptr;
    }
    return *this;
}

User* Session::getActiveUser() const {
    return activeUser;
}

std::unordered_map<std::string,User*>& Session::getUserMap() {
    return userMap;
}

std::vector<Watchable*> &Session::getContent() {
    return content;
}

std::vector<BaseAction*> &Session::getActionsLog() {
    return actionsLog;
}

void Session::setActiveUser(User* user) {
    activeUser = user;

}
void Session::setTerminated(bool s) {
    terminated = s;
}

std::vector<std::string> Session::splitString(std::string s) {

    std::string word;
    std::vector<std::string> vector;
    for(size_t i = 0; i < s.size(); i++){
        if(s[i] != ' ')
            word = word + s[i];
        else {
            vector.push_back(word);
            word = "";
        }
    }
    vector.push_back(word);
return vector;
}

void Session::clear() {
    for (size_t i = 0; i < actionsLog.size(); i++)
        delete actionsLog[i];
    actionsLog.clear();

    for (size_t i = 0; i < content.size(); i++)
        delete content[i];
    content.clear();

    for(auto& x: userMap)
        delete(x.second);
    userMap.clear();

}

void Session::myCopy(const Session& other){
    for(auto& x: other.content){
        content.push_back(x->clone());
    }
    for(auto& x: other.actionsLog){
        actionsLog.push_back(x->clone());
    }
    for(auto& x: other.userMap) {
        userMap.insert({x.first, x.second->clone()});
        for (auto &y: userMap[x.first]->getHistory()) {
            y = this->placeInHistory(*y);
        }
    }
    if(other.activeUser!= nullptr)
        activeUser = this->userMap.at(other.activeUser->getName());

}
Watchable* Session::placeInHistory(Watchable &w) {
    Watchable* ret;
    for(size_t i = 0; i < content.size(); ++i){
        if(w.getId()==content[i]->getId())
            ret = content[i];
    }
    return ret;
}
void Session::start() {
    std::cout << "SPLFLIX is now on!" << endl;
    if(terminated)
        terminated=false;
    while(!terminated){
        std::string str;
        std::getline(std::cin,str);
        std::vector<std::string>  userInput = splitString(str);
        if(userInput[0]=="createuser"){
            BaseAction* createuser = new CreateUser(userInput[1],userInput[2]);
            if(createuser->getStatus() != ERROR) {
                createuser->act(*this);
            }
            actionsLog.push_back(createuser);
        }
        else if(userInput[0]=="changeuser"){
            BaseAction* changeuser = new ChangeActiveUser(userInput[1]);
            changeuser->act(*this);
            actionsLog.push_back(changeuser);
        }
        else if(userInput[0]=="deleteuser"){
            BaseAction* deleteuser = new DeleteUser(userInput[1]);
            deleteuser->act(*this);
            actionsLog.push_back(deleteuser);
        }
        else if(userInput[0]=="dupuser"){
            BaseAction* dupuser = new DuplicateUser(userInput[1], userInput[2]);
            dupuser->act(*this);
            actionsLog.push_back(dupuser);
        }
        else if(userInput[0]=="content"){
            BaseAction* content = new PrintContentList();
            content->act(*this);
            actionsLog.push_back(content);
        }
        else if(userInput[0]=="watchhist"){
            BaseAction* watchhist = new PrintWatchHistory();
            watchhist->act(*this);
            actionsLog.push_back(watchhist);
        }
        else if(userInput[0]=="watch"){
            long id = std::stol(userInput[1]);
            BaseAction* watch = new Watch(id);
            watch->act(*this);
            actionsLog.push_back(watch);
        }
        else if(userInput[0]=="log"){
            BaseAction* log = new PrintActionsLog();
            log->act(*this);
            actionsLog.push_back(log);
        }
        else if(userInput[0]=="exit"){
            BaseAction* exit = new Exit();
            exit->act(*this);
            actionsLog.push_back(exit);
        }
        else if(userInput[0]=="y"){
            std::vector<Watchable*> userHistory = this->getActiveUser()->getHistory();
            long id = userHistory[userHistory.size()-1]->getNextWatchable(*this)->getId();
            BaseAction* watch = new Watch(id);
            watch->act(*this);
            actionsLog.push_back(watch);
        }


    }
}
