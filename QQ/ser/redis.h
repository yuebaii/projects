#ifndef _REDIS_H_
#define _REDIS_H_

#include <iostream>
#include <string.h>
#include <string>
#include <stdio.h>
#include <hiredis/hiredis.h>
using namespace std;
class Redis
{
public:
	Redis(){}
	~Redis()
	{
		this->_connect = NULL;
		this->_reply = NULL;            
	}
	//连接redis。 ip地址           端口号          
	bool connect(string host, int port)
	{
		this->_connect = redisConnect(host.c_str(), port);
		if(this->_connect != NULL && this->_connect->err)
		{
			printf("connect error: %s\n", this->_connect->errstr);
			return 0;
		}
		return 1;
	}


//获取数据
string get(string key)
{
//类型强转    执行命令的函数     redis指针  命令     键       
	this->_reply = (redisReply*)redisCommand(this->_connect, "GET %s", key.c_str());
	if(REDIS_REPLY_NIL == this->_reply->type)//判断对应的值是否存在
	{
		cout<<"without key="<<key<<endl;
		return "!!!!without!!!!error!!!!";
	}
	string str = this->_reply->str;//获取到的数据
	freeReplyObject(this->_reply);//释放获取数据的指针
	return str;
}

//存储数据
void set(string key, string value)
{
	//执行命令      redis指针      命令		    键          值
	redisCommand(this->_connect, "SET %s %s", key.c_str(), value.c_str());
}

//删除key存储的值
void del(string key)
{
	redisCommand(this->_connect,"DEL %s",key.c_str());
}


private:

	redisContext* _connect;//redis指针
	redisReply* _reply;//获取get结果的指针

};

#endif 
