#include<iostream>
#include<json/json.h>
#include"view.h"
#include<errno.h>
#include<string.h>
#include<string>
#include<sys/socket.h>
#include"view_offline.h"
#include<mysql/mysql.h>
#include<typeinfo>
#include"redis.h"
using namespace std;

view_offline::view_offline(int cli_fd)
{
	cli_fds = cli_fd;
}

void view_offline::processs(Json::Value val,int cli_fd,string key)
{
	//先尝试从缓存redis删除用户
	cout<<"offline"<<endl;
	  //初始化redis
	Redis *r = new Redis();
	  //连接redis
	if(!r->connect("127.0.0.1", 6379))
	{
		cout<<"connect error!"<<endl;
		return;
	}
	
	//如果存在用户，则删除它，否则不干什么。
	
	std::string name = val["name"].asString();
	
	string str(r->get(name));
	if(str.compare("none") == 0)
	{
		r->del(name);
	}
	delete r;
	//再从在线表删除用户
	//cout<<"offmessage"<<endl;
	MYSQL *_mysql = mysql_init((MYSQL *)0);
	
	if(!mysql_real_connect(_mysql, "127.0.0.1", "root", "1q2w3e4r", NULL, 0, NULL, 0))
	{
		cerr<<"sql connect fail;errno:"<<errno<<endl;
		return;
	}
	
	if(mysql_select_db(_mysql, "chat"))
	{
		cerr<<"select fail：errno："<<errno<<endl;
		return;
	}
	
	char cmd[50] = "select fd from online where name='";
	strcat(cmd, name.c_str());
	strcat(cmd, "';");
	
	if(mysql_real_query(_mysql,cmd,strlen(cmd)))
	{
		cerr<<"login select online fail;errno:"<<errno<<endl;
		return;
	}
	//接受返回值
	MYSQL_RES *mp_res = mysql_store_result(_mysql);
	MYSQL_ROW mp_row = mysql_fetch_row(mp_res);
	
	char cmdd[50] = "delete from online values('";
	strcat(cmdd, name.c_str());
	strcat(cmdd, "','");
	strcat(cmdd, mp_row[0]);
	strcat(cmdd, "');");

	if(mysql_real_query(_mysql,cmd,strlen(cmd)))
	{
		cerr<<"exit delete online fail;errno:"<<errno<<endl;
		return;
	}
	Json::Value root;
	root["messages"] = "您退出成功！";
	
	string messages = EncryptionAES(root.toStyledString(),key);

	if(-1 == send(cli_fd,messages.c_str(),strlen(messages.c_str()),0))
	{
		cerr<<"send reason fail;errno:"<<errno<<endl;
	}
}

void view_offline::responce()
{
	send(cli_fds, "login success!", 15, 0);
}
