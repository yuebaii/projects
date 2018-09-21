#include<iostream>
#include<json/json.h>
#include"view_transfile.h"
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

view_transfile::view_transfile(int cli_fd)
{
	cli_fds = cli_fd;
}

void view_transfile::process(Json::Value val,int cli_fd,string key)
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
	std::string filename = val["filename"].asString();
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

		r->set(name, mp_row[0]);
		fdd = atoi(mp_row[0]);
		delete r;
		
	Json::Value vall;
	Json::Reader read;
	char buffer[] = filename.c_str();
	int fds = open(buff, RDWR, 0666);
	char buff[255] = {0};
	while(1)
	{
		
		if(0 < recv(cli_fd, buff, 255, 0))//接受来自客户端A的数据
		{
			string buffer = buff;
			string messages = DecryptionAES(buffer,key);
			
			char *buf = messages.c_str();
			if(strcmp(buf, "end") == 0)	break;
			write(fds, strlen(buf), buff);
		}
	}
	string file = "file";
	string filename = buff;
	vall["messages"] = file;
	vall["filename"] = filename;
	string messages = EncryptionAES(vals.toStyledString(),key);
			
	if(-1 == send(fdd, messages.c_str(), strlen(messages.c_str()), 0))
	{
			cerr<<"send reasons fail;errno:"<<errno<<endl;
			return;
	}
	char buffers[128] = {0};
	while(read(fds, 128, buffers) > 0)
	{
		string buffss = buffers;
		string messages = EncryptionAES(buffss,key);
		if(-1 == send(fdd, messages.c_str(), strlen(messages.c_str()), 0))
		{
			cerr<<"send reasons fail;errno:"<<errno<<endl;
			return;
		}
	}
	char buff[] = "end";
	send(fdd, buff, strlen(buff), 0);
	close(fds);
}

void view_transfile::responce()
{
}





