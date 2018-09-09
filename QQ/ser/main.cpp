#include <iostream>
#include <cstdlib>
#include "tcpsever.h"
#include <stdio.h>
using namespace std;
//class 	tcpsever;

int main(int argc,char* argv[])
{
	//ip   port   pth_num
	if(argc < 4)
	{
		cout<<"请输入IP地址,端口号或者线程数！"<<endl;
	}
	char *ip = argv[1];
	int port;
	sscanf(argv[2], "%d", &port);
	//unsigned short port = atoi(argv[2]);
	int pth_num;
	sscanf(argv[3], "%d", &pth_num);
	//int pth_num = atoi(argv[3]);
	//构造服务器对象
	tcpsever ser(ip, port, pth_num); 

	//运行服务器
	ser.run();

	return 0;
}
