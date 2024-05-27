#include "messageParser.h"

shared_ptr<messageParser> messageParser::getInstance()
{
    static shared_ptr<messageParser> parser(new messageParser());
    return parser;
}

void messageParser::addOri(char* ori)
{
	using std::lock_guard;
	lock_guard<mutex> lock(m_mutex);
	m_oriList.push_back(string(ori));
	m_cv.notify_one();
}

messageParser::messageParser()
{
	using std::unique_lock;
	int i = 0;
	while (i < 10)
	{
		m_threadList.push_back(thread([this]() {
			while (true)
			{
				unique_lock<mutex> lock(m_mutex);
				m_cv.wait(lock, [this]() {return m_oriList.size() > 0; });
				if (m_oriList.size() > 0)
				{
					message msg = parse2Message((char*)m_oriList[0].c_str());
					m_oriList.erase(m_oriList.begin());
					if (msg.isFromClient) 
					{
						if (msg.isEditRequest) 
						{
							//do edit in db
						}
						else
						{
							//send response
						}
					}
					else 
					{
						//do edit in ui
					}
				}
			}
		}));
		i++;
	}
	for (auto& t : m_threadList)
	{
		t.join();
	}
}

messageParser::~messageParser()
{
}


message messageParser::parse2Message(char* ori)
{
    message msg;
	string str(ori);
	int pos = str.find(" ");
	msg.isFromClient = str.substr(0, pos).compare("TRUE");
	str = str.substr(pos + 1);
	int pos = str.find(" ");
	msg.sender = str.substr(0, pos);
	str = str.substr(pos + 1);
	pos = str.find(" ");
	msg.info.machine_id = str.substr(0, pos);
	str = str.substr(pos + 1);
	msg.info.voltage = atoi(str.c_str());
	return msg;
}
