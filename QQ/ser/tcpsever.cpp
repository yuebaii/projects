#include<iostream>
#include"tcpsever.h"
#include"mpthread.h"
#include<cstdlib>
#include<assert.h>
#include<vector>
#include<string.h>
#include<arpa/inet.h>
#include<sys/socket.h>
#include<event.h>
#include<map>
#include<json/json.h>
#include<unistd.h>
#include <fcntl.h>
#include<errno.h>
#include<stdio.h>
#include"openssl_test.h"
//pthread_mutex_t mutex;
using namespace std;
#define LEN pth_numm
//gcc -o  main main.cpp tcpsever.cpp mpthread.cpp contral.cpp view_register.cpp view_login.cpp view_talkone.cpp view_talkgroup.cpp view_getlist.cpp view_exit.cpp view_offline.cpp openssl_test.cpp -lstdc++ -levent -ljson -lmysqlclient -L/usr/lib/mysql `mysql_config --cflags --libs` -lhiredis
//export PATH=/usr/local/gcc-4.9.1/bin/:$PATH
static int number = 0;

tcpsever::tcpsever(char *ip,unsigned short port,int pth_num)
{
	//构建服务器     bind    listen
	ipp = ip;
	portt = port;
	pth_numm = pth_num;
        _listen_fd = socket(AF_INET, SOCK_STREAM, 0);
	assert(_listen_fd != -1);

	struct sockaddr_in ser, cli;
	memset(&ser, 0, sizeof(ser));
	ser.sin_family = AF_INET;
	ser.sin_port = htons(portt);
	ser.sin_addr.s_addr = inet_addr(ipp); 
	
	int res = bind(_listen_fd, (struct sockaddr *)&ser, sizeof(ser));
	assert(res != -1);
	listen(_listen_fd, 500);
	
	_base = event_base_new();
}

void listen_cb(int fd,short event,void *arg)
{
	//accept    ->cli_fd 
	Ptcpsever pthis = (Ptcpsever)arg;         
	struct sockaddr_in ser;
	memset(&ser, 0, sizeof(ser));
	ser.sin_family = AF_INET;
	ser.sin_port = htons(pthis->portt);
	ser.sin_addr.s_addr = inet_addr(pthis->ipp); 

	socklen_t len = sizeof(ser);
	int cli_fd = accept(fd, (sockaddr *)&ser, &len);

	std::string keys[2];  
    	generateRSAKey(keys); // 产生公钥和私钥
	send(cli_fd, keys[0].c_str(), strlen(keys[0].c_str()), 0);//公钥发给客户端

	char buffer[2100] = {0};
	recv(cli_fd, buffer, 2100, 0);//服务器接受在客户端经公钥加密的密钥
	string b = buffer;
	int fds = open("key.text", O_RDWR | O_CREAT, 0666);//创建文件，
	string key = rsa_pri_decrypt(b, keys[1]);//用私钥对经公钥加密的密钥进行解密，
	write(fds, key.c_str(), strlen(key.c_str()));//将解密的原密钥写进文件。
	
	if(-1 == cli_fd) //防御式编程 
	{
		cout<<"listen_cb accept fail;error:"<<errno<<endl;
		exit(0);
	}

	//在_pth_event_map   找到将听量最小的子线程
	char buff[10] = {0};  
	
	map<int, int>::iterator it= (pthis->_pth_event_num).begin();
	
	map<int, int>::iterator min = (pthis->_pth_event_num).begin();
	for(; it != (pthis->_pth_event_num).end(); it++)
	{
		if((it->second) < (min->second))
		{
			min = it;
		}
	}
	sprintf(buff, "%d", cli_fd);

	send(min->first, buff, sizeof(buff), 0);
	cout<<"send"<<(min->first)<<endl;
}

void sock_1_cb(int fd,short event,void *arg)
{
	//recv  ->pth_num
	Ptcpsever sthis = (Ptcpsever)arg;
	char buff[100];
	++number;
	
	if(0<recv(fd,buff,100,0))
	{
		 (sthis->_pth_event_num)[fd] = atoi(buff);
		if(number < 9000)
		{
		cout<<fd<<"连接量为 ："<<(sthis->_pth_event_num)[fd]<<endl;
		}
	}
	else
	{
		close(fd);
	}
	
}


void tcpsever::create_socketpair()
{
	int arr[2];
	for(int i = 0; i<pth_numm; i++)
	{
		socketpair(AF_UNIX, SOCK_STREAM, 0, arr);
		
		//将socket0放入map表，对应线程监听量0
		_pth_event_num.insert(make_pair(arr[0],0));
		
		//将sock_0加入到libevent
		struct event* sock_0_event = event_new(_base, arr[0], EV_READ|EV_PERSIST,sock_1_cb, this);
		
		if(NULL == sock_0_event)
		{
			cerr<<"sock_0_event create fail;errno:"<<errno<<endl;
			exit(0);
		}
		event_add(sock_0_event, NULL);
		
		//将socketpair保存起来
		_socketpair_base.push_back(arr); 
		
		tmp.push_back(_socketpair_base[i][1]);
	}
}

void tcpsever::create_pth()
{
	int i = 0;
	cout<<pth_numm<<endl;
	for(int i = 0; i< pth_numm; i++)
	{
		cout<<"create_pth start"<<endl;
		 Mpthread* pth = new mpthread(tmp[i]);
		sleep(1);
	}
}

void tcpsever::run()
{
	//申请socketpair
	create_socketpair();

	//启动线程
	create_pth();

	//将listen_fd加入到 libevent
	struct event* listen_event = event_new(_base,_listen_fd,EV_READ|EV_PERSIST,listen_cb,this);

	event_add(listen_event, NULL);

	event_base_dispatch(_base);//while(1){epoll_wait();}
}
