#include<iostream>
#include<json/json.h>
#include"view_login.h"
#include"view.h"
#include<errno.h>
#include<string.h>
#include<string>
#include<stdio.h>
#include<sys/socket.h>
#include<mysql/mysql.h>
#include"AES.h"
using namespace std;

view_login::view_login(int cli_fd)
{
	cli_fds = cli_fd;
}
void view_login::processs(Json::Value val, int cli_fd, string key)
{	
	cout<<"login"<<endl;
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
	char cmds[70] = "select * from usr;";

	//输入指令
	if(mysql_real_query(_mysql,cmds,strlen(cmds)))
	{
		cerr<<"login select usrs fail;errno:"<<errno<<endl;
		return;
	}
	
	//接受返回值
	MYSQL_RES *mp_res = mysql_store_result(_mysql);

	//接受返回值的一行
	MYSQL_ROW mp_row;
	while(mp_row = mysql_fetch_row(mp_res))
	{
		if(strcmp(mp_row[0],name.c_str()) == 0)
		{
			break;
		}
	}
	
	if(mp_row == 0)
	{
		cout<<name<<" not exist"<<endl;
		//回复用户
		Json::Value root;
		root["lreason"] = "用户名不存在！";
		string messages = EncryptionAES(root.toStyledString(),key);

		if(-1 == send(cli_fd,messages.c_str(),strlen(messages.c_str()),0))
		{
			//cerr<<"send reason fail;errno:"<<errno<<endl;
			cout<<"send reason fail!"<<endl;
		}
		return;
	}
	
	const char *tmp = pw.c_str();
	char *password = new char[strlen(tmp)+1];
	strcpy(password, tmp);
	Json::Value roots;
	if(strcmp(mp_row[1], password))
	{
		cout<<"pw not ok"<<endl;
		//回复用户
		roots["lreason"] = "密码不正确！";
		string messages = EncryptionAES(roots.toStyledString(),key);

		if(-1 == send(cli_fd,messages.c_str(),strlen(messages.c_str()),0))
		{
			cerr<<"send reason fail;errno:"<<errno<<endl;
		}

		return;
	}
   	
	roots["lreason"] = "登陆成功！"; 
	cout<<roots.toStyledString()<<endl;
	string messages = EncryptionAES(roots.toStyledString(),key);

	if(-1 == send(cli_fd,messages.c_str(),strlen(messages.c_str()),0))
	{
		cout<<"send reason fails!"<<endl;
	}

	//在离线消息缓存表的表内查找用户是否存在  
	char cmd[100] = "select reason from offline where name = ";
	strcat(cmd,"'");
	strcat(cmd, name.c_str());
	strcat(cmd,"';");

	if(mysql_real_query(_mysql,cmd,strlen(cmd)))
	{
		cerr<<"login select offline fail;errno:"<<errno<<endl;
	}
	else{//如果存在,将离线消息发送给客户端的用户
	MYSQL_RES *mp_ress = mysql_store_result(_mysql);
	MYSQL_ROW mp_rows = mysql_fetch_row(mp_ress);
	Json::Value rooter;
	
	if(mp_rows != 0)
	{	
		rooter["messages"] = mp_rows[0];
		cout<<"mp_rows[0]"<<mp_rows[0]<<endl;
		
		string messages = EncryptionAES(ro.toStyledString(),key);

		if(-1 == send(cli_fd,messages.c_str(),strlen(messages.c_str()),0))
		{
			cerr<<"send offmessages fail;errno:"<<errno<<endl;
			return;
		}
		
	//并从表中删除此记录     delete from offline where name = 值；
	char temp [100] = "delete from offline where name=";
	strcat(temp,"'");
	strcat(temp,name.c_str());
	strcat(temp,"';");
	mysql_real_query(_mysql,temp,strlen(temp));
	}
	
	//并将用户加入在线表
	char cmddd[50] = "insert into online values("; 
	strcat(cmddd,"'");
	strcat(cmddd,name.c_str());
	strcat(cmddd,"','");
	
	char buff[10] = {0};
	sprintf(buff, "%d", cli_fd);
	strcat(cmddd,buff);
	strcat(cmddd,"');");
	mysql_real_query(_mysql,cmddd,strlen(cmddd));
	}
}

void view_login::responce()
{
	send(cli_fds, "login success!", 15, 0);
}
