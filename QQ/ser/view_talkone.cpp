#include<iostream>
#include<json/json.h>
#include"view_talkone.h"
#include"view.h"
#include<errno.h>
#include<string.h>
#include<string>
#include<sys/socket.h>
#include<mysql/mysql.h>
#include<stdlib.h>
#include<unistd.h>
#include"redis.h"

using namespace std;

view_talkone::view_talkone(int cli_fd)
{
	cli_fds = cli_fd;
}

void view_talkone::processs(Json::Value val,int cli_fd,string key)
{
	cout<<"talk_one"<<endl;
	//初始化redis
	Redis *r = new Redis();
	//连接redis
	if(!r->connect("127.0.0.1", 6379))
	{
		printf("connect error!\n");
		return;
	}

	std::string name = val["name"].asString();

	int fdd = 0;
	Json::Value root;
	MYSQL_ROW mp_row;
	string str(r->get(name));
	// array[5] = "none";
	if(str.compare("none") != 0)
	{
		//数据库初始化
		MYSQL *_mysql = mysql_init((MYSQL *)0);
		//数据库连接
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

		//在线用户的表检索fd的sql语句  select fd from online where name =  值； 
		char cmd[100] = "select fd from online where name=";
		strcat(cmd,"'");
		strcat(cmd, name.c_str());
		strcat(cmd,"'");
		strcat(cmd, ";");
		if(mysql_real_query(_mysql,cmd,strlen(cmd)))
		{
			cerr<<"login select online fail;errno:"<<errno<<endl;
			return;
		}
		//接受返回值
		MYSQL_RES *mp_res = mysql_store_result(_mysql);
		mp_row = mysql_fetch_row(mp_res);

		//所有用户的表
		char ccmd[50] = "select pw from usr where name=";
		strcat(ccmd,"'");
		strcat(ccmd, name.c_str());
		strcat(ccmd,"';");
		
		if(mysql_real_query(_mysql,ccmd,strlen(ccmd)))
		{
			cerr<<"login select online fail;errno:"<<errno<<endl;
			return;
		}	
		//接受返回值
		MYSQL_RES *msl = mysql_store_result(_mysql);
		MYSQL_ROW myrow = mysql_fetch_row(msl);

		Json::Value root;
		if(mp_row==0 && myrow==0)
		{
			root["messages"] = "用户不存在，请重新选择用户！";

			string messages = EncryptionAES(root.toStyledString(),key);
			if(-1 == send(cli_fd,messages.c_str(),strlen(messages.c_str()),0))
			{
				cerr<<"send reason fail;errno:"<<errno<<endl;
			}
		}
		else if(mp_row==0 && myrow!=0)
		{
			root["messages"] = "您的好友离线中，请给好友发送离线消息！";

			string messages = EncryptionAES(root.toStyledString(),key);

			if(-1 == send(cli_fd,messages.c_str(),strlen(messages.c_str()),0))
			{
				cerr<<"send reason fail;errno:"<<errno<<endl;
			}
			
			while(1)
			{
				Json::Value vvall;
				Json::Reader rread;
				char buff[128] = {0};
				if(0 < recv(cli_fd, buff, 127, 0))
				{

					string buffer = buff;
					string messages = DecryptionAES(buffer, key.c_str());
					//解析json包
					if(-1 == rread.parse(messages.c_str(),vvall))
					{
						std::cout<<"json prase fail!"<<std::endl;
						return;
					}
					
					if(strncmp(vvall["messages"].asString().c_str(), "end", 3) == 0)	break;
					cout<<vvall["messages"].asString().c_str()<<endl;
					
					char cmdd[100] = "insert into offline values(";
					strcat(cmdd,"'");
					strcat(cmdd,name.c_str());
					strcat(cmdd,"','");
					strcat(cmdd,vvall["messages"].asString().c_str());
					strcat(cmdd,"');");
			
					if(mysql_real_query(_mysql,cmdd,strlen(cmdd)))
					{
						cerr<<"send  offline message fail;errno:"<<errno<<endl;
						return;
					}
					
				}
			}
			cout<<"离线消息发送成功！"<<endl;

			return;
		}
		else
		{
			root["messages"] = "您的好友在线中，请给好友发送在线消息！";
			
			string messages = EncryptionAES(root.toStyledString(),key);

			if(-1 == send(cli_fd,messages.c_str(),strlen(messages.c_str()),0))
			{
				cerr<<"send reason fail;errno:"<<errno<<endl;
		
			}
			r->set(name, mp_row[0]);
			fdd = atoi(mp_row[0]);

			delete r;
		}
	}	
	else{
		fdd = atoi(str.c_str());
		root["messages"] = "您的好友在线中，请给好友发送在线消息！";
	
		string messages = EncryptionAES(root.toStyledString(),key);

		if(-1 == send(cli_fd,messages.c_str(),strlen(messages.c_str()),0));
		{
			cerr<<"send reason fail;errno:"<<errno<<endl;
		}
	}
	//cout<<fdd<<endl;
	
	Json::Value vall;
	Json::Reader read;
	
	char buff[255] = {0};
	while(1)
	{
		
		if(0 < recv(cli_fd, buff, 255, 0))//接受来自客户端A的数据
		{
			//给在线用户发送在线消息 
			if(-1 == send(fdd,buff,strlen(buff),0))
			{
				cerr<<"send reasons fail;errno:"<<errno<<endl;
				return;
			}
			
		}
		else
		{
			close(cli_fd);
		}
	}

}

void view_talkone::responce()
{
	//send(cli_fds, "login success!", 15, 0);
}
