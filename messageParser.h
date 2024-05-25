#ifndef MESSAGEPARSER_H
#define MESSAGEPARSER_H
#include <string>
#include <vector>
#include <memory>
#include <mutex>
#include <condition_variable>
#include <errno.h>
#include <thread>
using std::string;
using std::vector;
using std::shared_ptr;
using std::thread;
using std::mutex;
using std::condition_variable;

struct machineInfo
{
	string machine_id;
	int voltage;
	//more info
};

struct message 
{
	bool isReq;
	string sender;
	machineInfo info;
};

class messageParser
{
private:
	vector<string> m_oriList;
	mutex m_mutex;
	condition_variable m_cv;
	messageParser();
	messageParser(const messageParser&);
	message parse2Message(char* ori);
	vector<thread> m_threadList;

public:
	~messageParser();
	static shared_ptr<messageParser> getInstance();
	void addOri(char* ori);
};

#endif
