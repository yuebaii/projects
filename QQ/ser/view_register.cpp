#include<iostream>
#include<json/json.h>
#include"view.h"
#include<errno.h>
#include<string.h>
#include<string>
#include<sys/socket.h>
#include<mysql/mysql.h>
#include"view_register.h"
using namespace std;

view_register::view_register(int cli_fd)
{
	cli_fds = cli_fd;
}

void view_register::processs(Json::Value val, int cli_fd, string key)
{
	cout<<"register"<<endl;
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

	std::string name = val["name"].asString();
	std::string pw = val["pw"].asString();
	//查询name是否存在于usr表
	char cmd[50] = "select * from usr;";
	//输入指令

	if(mysql_real_query(_mysql,cmd,strlen(cmd)))
	{
		//cerr<<"login select usr fail;errno:"<<errno<<endl;
		cout<<"login select usr fail!"<<endl;
		return;
	}
	//接受返回值
	MYSQL_RES *mp_res = mysql_store_result(_mysql);

	//接受返回值的一行
	MYSQL_ROW mp_row;
	//cout<<"fff"<<endl;
	while(mp_row = mysql_fetch_row(mp_res))
	{
		if(strcmp(mp_row[0],name.c_str()) == 0)
		{
		Json::Value root;
		root["rreason"] = "用户已存在！";
		string messages = EncryptionAES(root.toStyledString(),key);

		if(-1 == send(cli_fd,messages.c_str(),strlen(messages.c_str()),0))
		{
			//cerr<<"send reason fail;errno:"<<errno<<endl;
			cout<<"send reason fail!"<<endl;
		}
			return;
		}
			
	}
	
	//输入指令                   表名           表内内容
	

	char cmdd[50] = "insert into usr values(";
	strcat(cmdd,"'");
	strcat(cmdd,name.c_str());
	strcat(cmdd,"',");
	strcat(cmdd,"'");
	strcat(cmdd,pw.c_str());
	strcat(cmdd,"');");

	if(mysql_real_query(_mysql,cmdd,strlen(cmdd)))
	{
		cerr<<"0 query fail;errno:"<<errno<<endl;
		return;
	}
	
	Json::Value root;
	root["rreason"] = "注册成功！";
	string messages = EncryptionAES(root.toStyledString(),key);

	if(-1 == send(cli_fd,messages.c_str(),strlen(messages.c_str()),0))
	{
		//cerr<<"send reason fail;errno:"<<errno<<endl;
		cout<<"send reason fail!"<<endl;
	}
	
}

void view_register::responce()
{
}
