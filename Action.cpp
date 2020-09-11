#include "../include/Action.h"
#include "../include/User.h"
#include <string>
#include <vector>
#include <unordered_map>
#include <iostream>

using namespace std;

BaseAction::BaseAction():errorMsg(), status(PENDING){}

ActionStatus BaseAction::getStatus() const {
    return status;
}
std::string BaseAction::getErrorMsg() const {
    return errorMsg;
}
void BaseAction::complete() {
    status = COMPLETED;
}
void BaseAction::error(const std::string &_errorMsg) {
    errorMsg = _errorMsg;
    status=ERROR;
    cout << "Error - " << getErrorMsg() << endl;

}

/////////////////////////////

CreateUser::CreateUser():BaseAction(), name(), algo(){} //deafault constructor

CreateUser::CreateUser(std::string _name,std::string _algo):BaseAction(),name(_name), algo(_algo) {// constructor //
    if(algo != "len" && algo != "rer" &&algo != "gen") {
        error("recommendation algorithm is invalid");
    }
}

void CreateUser:: act(Session& sess) {

    if(getStatus() == ERROR)
        cout<<getErrorMsg()<<endl;
    else{
        if (sess.getUserMap().find(name) != sess.getUserMap().end()) {
            error("username is already in use");
        }
        else {
            User* newUser;
            if (algo == "len")
                newUser = new LengthRecommenderUser(name);
            else if (algo == "rer")
                newUser = new RerunRecommenderUser(name);
            else
                newUser = new GenreRecommenderUser(name);
            complete();
            sess.getUserMap().insert({name,newUser});
        }
    }

}
std::string CreateUser::toString() const {
    string s = "";
    if(getStatus()==COMPLETED)
        s = "CreateUser COMPLETED";
    else if(getStatus()==ERROR)
        s = "CreateUser ERROR: " + getErrorMsg();
    else
        s = "CreateUser PENDING";
    return s;
}

BaseAction* CreateUser::clone() {
    return new CreateUser(*this);
}
/////////////////////////////

ChangeActiveUser::ChangeActiveUser(): BaseAction(), name(){}

ChangeActiveUser::ChangeActiveUser(std::string _name) :BaseAction(),name(_name) {}

void ChangeActiveUser:: act(Session& sess) {
    if(sess.getUserMap().find(name) != sess.getUserMap().end()) {
        sess.setActiveUser(sess.getUserMap()[name]);   //need to check
        complete();
    }
    else {
        error("user does not exist");
    }
}

std::string ChangeActiveUser::toString() const {
    string s = "";
    if(getStatus()==COMPLETED)
        s = "ChangeUser COMPLETED";
    else if(getStatus()==ERROR)
        s = "ChangeUser ERROR: " + getErrorMsg();
    else
        s = "ChangeUser PENDING";
    return s;
}
BaseAction* ChangeActiveUser::clone() {
    return new ChangeActiveUser(*this);
}

/////////////////////

DeleteUser::DeleteUser(): BaseAction(), name(){}

DeleteUser::DeleteUser(std::string _name): BaseAction(), name(_name) {}

void DeleteUser::act(Session &sess) {
    if(sess.getUserMap().find(name)!=sess.getUserMap().end()) {
        if(name==sess.getActiveUser()->getName()) {
            error("Can't delete active user");
        }
        else {
            delete sess.getUserMap()[name];
            sess.getUserMap()[name] = nullptr;
            sess.getUserMap().erase(name);
        }
        complete();
    }
    else {
        error("user does not exist");
    }
}
std::string DeleteUser::toString() const {
    string s = "";
    if(getStatus()==COMPLETED)
        s = "DeleteUser COMPLETED";
    else if(getStatus()==ERROR)
        s = "DeleteUser ERROR: " + getErrorMsg();
    else
        s = "DeleteUser PENDING";
    return s;
}
BaseAction* DeleteUser::clone() {
    return new DeleteUser(*this);
}
//////////////////////////////////////////////////////////////////////

DuplicateUser::DuplicateUser() :BaseAction(),name(),nameToCopy(){}

DuplicateUser::DuplicateUser(std::string _name, std::string _nameToCopy):BaseAction(), name(_name),nameToCopy(_nameToCopy) {}

void DuplicateUser::act(Session &sess) {
    if(sess.getUserMap().find(name) != sess.getUserMap().end() && sess.getUserMap().find(nameToCopy) == sess.getUserMap().end()) {
        User* newUser=(sess.getUserMap().find(name))->second->clone();
        newUser->setName(nameToCopy); // doesnt enter userMap
        sess.getUserMap().insert({nameToCopy,newUser});
        complete();
    }
    else if(sess.getUserMap().find(name) != sess.getUserMap().end() && sess.getUserMap().find(nameToCopy) != sess.getUserMap().end()) {
        error("username is already in use");
    }
    else {
        error("name does not exist");
    }

}
std::string DuplicateUser::toString() const {
    string s = "";
    if(getStatus()==COMPLETED)
        s = "DuplicateUser COMPLETED";
    else if(getStatus()==ERROR)
        s = "DuplicateUser ERROR: " + getErrorMsg();
    else
        s = "DuplicateUser PENDING";
    return s;
}
BaseAction* DuplicateUser::clone() {
    return new DuplicateUser(*this);
}
////////////////////////////////////////////////////////////////////////////

PrintContentList::PrintContentList() {}

void PrintContentList::act(Session &sess) {
    for(size_t i = 0; i < sess.getContent().size(); i++){
        std::cout<<sess.getContent()[i]->getId()<<". "<<sess.getContent()[i]->toString()<<" "<<std::to_string(sess.getContent()[i]->getLength())<<" minutes "<<sess.getContent()[i]->tagsToString()<<std::endl;
    }
    complete();
}

std::string PrintContentList::toString() const {
    string s = "";
    if(getStatus()==COMPLETED)
        s = "PrintContentList COMPLETED";
    else if(getStatus()==ERROR)
        s = "PrintContentList ERROR: " + getErrorMsg();
    else
        s = "PrintContentList PENDING";
    return s;
}

BaseAction* PrintContentList::clone() {
    return new PrintContentList(*this);
}
//////////////////////////////////////////////////////////////////////////

PrintWatchHistory::PrintWatchHistory(): BaseAction() {}

void PrintWatchHistory::act(Session &sess) {
    std::cout<<"Watch history for "<<sess.getActiveUser()->getName()<<endl;
    for(size_t i = 0; i < sess.getActiveUser()->getHistory().size(); i++){
        std::cout<<i+1<<". "<<sess.getActiveUser()->getHistory()[i]->toString()<<std::endl;
    }
    complete();
}
std::string PrintWatchHistory::toString() const {
    string s = "";
    if(getStatus()==COMPLETED)
        s = "PrintWatchHistory COMPLETED";
    else if(getStatus()==ERROR)
        s = "PrintWatchHistory ERROR: " + getErrorMsg();
    else
        s = "PrintWatchHistory PENDING";
    return s;
}

BaseAction* PrintWatchHistory::clone() {
    return new PrintWatchHistory(*this);
}

////////////////////////////////////////////////////////////////////////

Watch::Watch(): BaseAction(), idPrinted(-1){}

Watch::Watch(long _idPrinted):BaseAction(), idPrinted(_idPrinted) {}

void Watch::act(Session &sess) {
    if((size_t )idPrinted<sess.getContent().size()) {
        Watchable *toWatch = sess.getContent()[idPrinted];
        std::cout << "Watching " << toWatch->toString() << std::endl;
        sess.getActiveUser()->setTagsMap(toWatch);
        sess.getActiveUser()->getHistory().push_back(toWatch);
        sess.getActiveUser()->setAverage();
        complete();
        Watchable *rec = toWatch->getNextWatchable(sess);
        std::cout << "We recommend watching " << rec->toString() << ", continue watching? [y/n]" << std::endl;
    }
    else
        error("the id is not valid");
}

std::string Watch::toString() const {
    string s = "";
    if(getStatus()==COMPLETED)
        s = "Watch COMPLETED";
    else if(getStatus()==ERROR)
        s = "Watch ERROR: " + getErrorMsg();
    else
        s = "Watch PENDING";
    return s;
}

BaseAction* Watch::clone() {
    return new Watch(*this);
}
//////////////////////////////////////////////////////////////////////////

PrintActionsLog::PrintActionsLog(): BaseAction() {}

void PrintActionsLog::act(Session &sess) {
    for(size_t i = 0;i < sess.getActionsLog().size(); i++){
        cout<<sess.getActionsLog()[i]->toString()<<endl;
    }
    complete();
}
std::string PrintActionsLog::toString() const {
    string s = "";
    if(getStatus()==COMPLETED)
        s = "PrintActionsLog COMPLETED";
    else if(getStatus()==ERROR)
        s = "PrintActionsLog ERROR: " + getErrorMsg();
    else
        s = "PrintActionsLog PENDING";
    return s;
}
BaseAction* PrintActionsLog::clone() {
    return new PrintActionsLog(*this);
}
/////////////////////////////////////////////////////////////////////

Exit::Exit(): BaseAction() {}

void Exit::act(Session &sess) {
    sess.setTerminated("true");
    complete();
}
std::string Exit::toString() const {
    string s = "";
    if(getStatus()==COMPLETED)
        s = "Exit COMPLETED";
    else if(getStatus()==ERROR)
        s = "Exit ERROR: " + getErrorMsg();
    else
        s = "Exit PENDING";
    return s;
}

BaseAction* Exit::clone() {
    return new Exit(*this);
}