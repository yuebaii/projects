#include<iostream>
#include"fun.h"
#include <stdio.h>
#include<cstdlib>
using namespace std;

int main(int argc,char **argv)
{
	//判断参数
	if(argc < 3)
	{
		cout<<"请输入服务器的IP地址以及端口号！"<<endl;
	}
	
	//获取ip,port
	char *ip = argv[1];
	//unsigned short port = atoi(argv[2]);
	int port;
	sscanf(argv[2], "%d", &port);
	fun(ip,port);

	return 0;
}
