#include<iostream>
#include<json/json.h>
#include"view_heart.h"
#include"view.h"
#include<errno.h>
#include<string.h>
#include<string>
#include<stdio.h>
#include<sys/socket.h>
#include<mysql/mysql.h>
using namespace std;

view_heart::view_heart(int cli_fd)
{
	cli_fds = cli_fd;
}
void view_heart::processs(Json::Value val, int cli_fd)
{	
	std::string name = val["message"].asString();
	cout<<name<<endl;
}
