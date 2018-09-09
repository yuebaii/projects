#include"fun.h"
#include"public.h"
#include<iostream>
#include<string.h>
#include<string>
#include<stdlib.h>
#include<assert.h>
#include<stdio.h>
#include<json/json.h>
#include<errno.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<signal.h>
#include<unistd.h>
using namespace std;
#define LEN 128
/*#define MSG_TYPE_REGISTER 0
#define MSG_TYPE_LOGIN 1
#define MSG_TYPE_TALKONE 2
#define MSG_TYPE_TALKGROUP 3
#define MSG_TYPE_GETLIST 4
#define MSG_TYPE_EXIT 5*/

// gcc -o main main.cpp fun.cpp  -lstdc++ -ljson 

void fun(char *ip,unsigned short port)
{
	//连接服务器
	int sockfd = 0;
	int i = 0;
	
	int array[10000] = {0};
	while(i <= 9999)
{       sockfd = socket(AF_INET, SOCK_STREAM, 0);
	assert(sockfd != -1);
	cout<<i<<endl;

	//if(i%100==0)	
	//{usleep(500);}
	struct sockaddr_in ser, cli;
	memset(&ser, 0, sizeof(ser));
	ser.sin_family = PF_INET;
	ser.sin_port = htons(port);
	ser.sin_addr.s_addr = inet_addr(ip);
	
	int res = connect(sockfd, (struct sockaddr*)&ser, sizeof(ser));
	assert(res != -1);

	cout<<"客户端socket"<<sockfd<<endl;
	array[i] = sockfd;
	i++;

}

	for(int j = 0; j < 10000; j++)
	{
		login(array[j],j+1);
	}

}
	
void login(int fd, int n)
{
	//int i = 1;

	char names[10] = {0};
	sprintf(names, "%d", n);
	string name = names;
	string pw = "111111";
	
	
	//用json打包type  name   pw
	Json::Value val;
	val["type"] = MSG_TYPE_LOGIN;
	val["name"] = name;
	val["pw"] = pw;

	//给服务器发送json包
	if(-1 == send(fd, val.toStyledString().c_str(), strlen(val.toStyledString().c_str()), 0))
	{
		cerr<<"send reason fail;errno:"<<errno<<endl;
	}
	
	
	//接受服务器的返回
	Json::Value roots;
	Json::Reader read;
	char buff[128] = {0};
	if(0 < recv(fd,buff,127,0))
	{
		//解析json包
		if(-1 == read.parse(buff,roots))
		{
			cerr<<"json prase fail;errno:"<<errno<<endl;
			exit(0);
		}

		//输出json
		cout<<"lreason:"<<roots["lreason"].asString()<<endl;
	}
	
	
	string s1 = "用户名不存在！";
	string s2 = "密码不正确！";
	
	//判断登录是否成功
	if(s1.compare(roots["lreason"].asString())==0 || s2.compare(roots["lreason"].asString())==0)
	{
		cout<<"login fail;"<<endl;
	}
	else
	{	cout<<"登录成功！"<<endl;
	}

}
