#include<iostream>
#include"mpthread.h"
#include<arpa/inet.h>
#include<sys/socket.h>
#include<event2/event.h>
#include<errno.h>
#include<string.h>
#include<stdlib.h>
#include <cstdlib>
#include <fcntl.h>
#include "contral.h"
#include<json/json.h>
#include<string>
#include<ctime>
#include<unistd.h>
#include <pthread.h>
using namespace std;
pthread_mutex_t mutex;
int tmp = 0;

int setnonblocking(int fd)
{
	int old_option = fcntl(fd, F_GETFL);
	int new_option = old_option | O_NONBLOCK;
	fcntl(fd, F_GETFL, new_option);
	return old_option;
}

void cli_cb(int fd, short event, void *arg)
{
	
	//recv -> buff
	unsigned char buff[200] = {0};//坑？？？缓冲区不能定义小了，不然读取数据不完整！！
	char buffer[1280] = {0};
	int  ret = recv(fd, buff, 200, 0))//不会阻塞住，因为连接socket上有事件发生才回调

	if(ret <= 0)//网络终端或者套接字关闭
	{
			cout<<"客户端已经断线了"<<endl;
			close(fd);
	}
	else if(ret > 0)
	{	
		//到终端
	        contral  con(fd);

		int fd = open("key.text", O_RDWR, 0666);
		read(fd, buffer, 1279);//读取到文件中的密钥
		string key = buffer;

		con.process(buff, fd, key); 
	}
	
}

void sock_11_cb(int fd, short event, void *arg)
{
	//强转  arg
	Pmpthread pthis = (Pmpthread)arg;

	//recv   ->cli_fd
	char buff[5] = {'0'};
	 recv(fd,buff,4,0);
	int cli_fd = atoi(buff);
	
	if(-1 == cli_fd)
	{
		cout<<"receive fail;error:"<<errno<<endl;
	}
	timeval two_sec = {2, 0};
	setnonblocking(cli_fd);
	//将cli_fd加入到libevent
	struct event* cli_event = event_new(pthis->_base, cli_fd, EV_TIMEOUT | EV_READ | EV_PERSIST, cli_cb,pthis->_base);//定时事件
	if(NULL == cli_event)
	{
		cout<<"cli_event create fail;errno:"<<errno<<endl;
		exit(0);
	}
	
	event_add(cli_event, &two_sec);
	
	
	(pthis->_event_map).insert(pair<int,struct event*>(cli_fd,cli_event));
	
	
	char buffer[10]={0};
	sprintf(buffer, "%d", (pthis->_event_map).size());
	
	send(fd, buffer, sizeof(buffer), 0);

	
}

void * run(void *arg)
{	
	
	Pmpthread mthis = (Pmpthread)arg;

	//为libevent申请空间 
	 mthis->_base = event_base_new();
	cout<<"sock[1]"<<mthis->_sock_1<<endl;

	//将sock_1加入到libevent
	struct event* sock_1_event = event_new(mthis->_base, mthis->_sock_1, EV_READ|EV_PERSIST, sock_11_cb, mthis); 
	
	if(NULL == sock_1_event)
	{
		cerr<<"sock_1_event create fail;errno:"<<errno<<endl;
		return 0;
	}
	

	event_add(sock_1_event,NULL);
	event_base_dispatch(mthis->_base);
}



mpthread::mpthread(int sock_1)
{
	this->_sock_1 = sock_1;
	
	//启动子线程
	pthread_t  first;
	
	pthread_create(&first, NULL, run, this);
}

	

