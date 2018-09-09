#include<iostream>
#include<json/json.h>
#include"view_modifypw.h"
#include"view.h"
#include<errno.h>
#include<string.h>
#include<string>
#include<sys/socket.h>
#include<mysql/mysql.h>
#include<typeinfo>
using namespace std;

view_talkgroup::view_modifypw(int cli_fd)
{
	cli_fds = cli_fd;
}

void view_modifypw::processs(Json::Value val,int cli_fd)
{
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

	std::string password = val["password"].asString();
	std::string name = val["names"].asString();

	char cmd[100] = "select pw from usr where name=";
	strcat(cmd, name.c_str());
	strcat(cmd, ";");
	if(mysql_real_query(_mysql,cmd,strlen(cmd)))
	{
		cerr<<"login select usr fail;errno:"<<errno<<endl;
		return;
	}
	
	//接受返回值
	MYSQL_RES *mp_res = mysql_store_result(_mysql);
	MYSQL_ROW mp_row = mysql_fetch_row(mp_res);

	Json::value root;
	if(strcmp(mp_row[0].asString(), password) == 0)
	{
		root["messages"] = "密码正确！";
		string messages = EncryptionAES(root.toStyledString(),key);

		if(-1 == send(cli_fd,messages.c_str(),strlen(messages.c_str()),0))
		{
			cerr<<"send reason fail;errno:"<<errno<<endl;
		}
	}
	
	Json::Value vall;
	Json::Reader readd;
	char buff[128] = {0};
	if(0 < recv(cli_fd, buff, 127, 0))
	{

		string buffer = buff;
		string messages = DecryptionAES(buffer, key.c_str());
		//解析json包
		if(-1 == readd.parse(messages.c_str(),vall))
		{
			std::cout<<"json prase fail!"<<std::endl;
			return;
		}
	}
	string pw = vall["password"];
	char cmdd[50] = "update usr set pw = ";
	strcat(cmdd, pw.c_str());
	strcat(cmdd, "where name = ");
	strcat(cmdd, name.c_str());
	strcat(cmdd, ";");
	if(mysql_real_query(_mysql,cmd,strlen(cmd)))
	{
		cerr<<"login select usr fail;errno:"<<errno<<endl;
		return;
	}

	Json::Value root;
	root["messages"] = "修改成功！";
	string messages = EncryptionAES(root.toStyledString(),key);

	if(-1 == send(cli_fd,messages.c_str(),strlen(messages.c_str()),0))
	{
		cerr<<"send reason fail;errno:"<<errno<<endl;
	}
}

void view_talkgroup::responce()
{
	//cout<<"ok"<<endl;
	send(cli_fds, "login success!", 15, 0);
}
