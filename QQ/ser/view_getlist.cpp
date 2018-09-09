#include<iostream>
#include<json/json.h>
#include"view_getlist.h"
#include"view.h"
#include<errno.h>
#include<string.h>
#include<string>
#include<sys/socket.h>
#include<mysql/mysql.h>
#include<typeinfo>
using namespace std;

view_getlist::view_getlist(int cli_fd)
{
	cli_fds = cli_fd;
}

void view_getlist::processs(Json::Value val,int cli_fd)
{	
	cout<<"ggg"<<endl;
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

	//查询name是否存在于usr表
	char cmd[50] = "select name from usr;";
	//输入指令
	if(mysql_real_query(_mysql,cmd,strlen(cmd)))
	{
		cerr<<"login select usr fail;errno:"<<errno<<endl;
		return;
	}

	//接受返回值
	MYSQL_RES *mp_res = mysql_store_result(_mysql);
	
	//接受返回值的一行
	MYSQL_ROW mp_row;
	
	char *buff = "无在线人员";
	while(mp_row = mysql_fetch_row(mp_res))
	{
		
		cout<<mp_row[0]<<endl;
	
		send(cli_fd, mp_row[0], strlen(mp_row[0]), 0);
		send(cli_fd, " ", strlen(" "), 0);
	}
	if(mp_row == 0)
	{
			
		send(cli_fd, buff, strlen(buff), 0); 
	}	
	send(cli_fd, "end", 4, 0);
}

void view_getlist::responce()
{
	//send(cli_fds, "login success!", 15, 0);
}
