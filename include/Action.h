#ifndef ACTION_H_
#define ACTION_H_


#include <string>

class User;
class Session;
class Watchable;

enum ActionStatus{
	PENDING, COMPLETED, ERROR
};


class BaseAction{
public:
	BaseAction();
    virtual ~BaseAction(){};
	ActionStatus getStatus() const; //their*******
	virtual void act(Session& sess)=0;
	virtual std::string toString() const=0;
	virtual BaseAction* clone()=0;
protected:
	void complete();
	void error(const std::string& errorMsg);
	std::string getErrorMsg() const;
private:
	std::string errorMsg;
	ActionStatus status;
};

class CreateUser  : public BaseAction {
public:
    CreateUser();
    CreateUser(std::string,std::string);
    virtual ~CreateUser(){};
    virtual void act(Session& sess);
	virtual std::string toString() const;
    virtual BaseAction* clone();
private:
    std::string name;
    std::string algo;
};

class ChangeActiveUser : public BaseAction {
public:
    ChangeActiveUser();
    ChangeActiveUser(std::string);
    virtual ~ChangeActiveUser(){};
    ChangeActiveUser(const CreateUser& ChangeActiveUser);
	virtual void act(Session& sess);
    virtual std::string toString() const;
    virtual BaseAction* clone();
private:
    std::string name;
};

class DeleteUser : public BaseAction {
public:
    DeleteUser();
    DeleteUser(std::string);
    virtual ~DeleteUser(){};
	virtual void act(Session & sess);
	virtual std::string toString() const;
    virtual BaseAction* clone();
private:
    std::string name;
};


class DuplicateUser : public BaseAction {
public:
    DuplicateUser();
    DuplicateUser(std::string, std::string);
    virtual ~DuplicateUser(){};
	virtual void act(Session & sess);
	virtual std::string toString() const;
    virtual BaseAction* clone();
private:
    std::string name;
    std::string nameToCopy;
};

class PrintContentList : public BaseAction {
public:
	PrintContentList();
    virtual ~PrintContentList(){};
    virtual void act (Session& sess);
	virtual std::string toString() const;
    virtual BaseAction* clone();
};

class PrintWatchHistory : public BaseAction {
public:
	PrintWatchHistory();
    virtual ~PrintWatchHistory(){};
    virtual void act (Session& sess);
	virtual std::string toString() const;
    virtual BaseAction* clone();
};

class Watch : public BaseAction {
public:
    Watch();
    Watch(long);
	virtual ~Watch(){};
    virtual void act(Session& sess);
	virtual std::string toString() const;
    virtual BaseAction* clone();
private:
    long idPrinted;
};

class PrintActionsLog : public BaseAction {
public:
	PrintActionsLog();
	virtual ~PrintActionsLog(){};
    virtual void act(Session& sess);
	virtual std::string toString() const;
    virtual BaseAction* clone();
};

class Exit : public BaseAction {
public:
    Exit();
    virtual ~Exit(){};
    virtual void act(Session& sess);
	virtual std::string toString() const;
    virtual BaseAction* clone();
};
#endif
