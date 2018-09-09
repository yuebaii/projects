#include"fun.h"
#include"public.h"
#include"openssl_test.h"
#include<iostream>
#include<string.h>
#include<fcntl.h>
#include<string>
#include<stdlib.h>
#include<assert.h>
#include<stdio.h>
#include<json/json.h>
#include<errno.h>
#include<string.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<signal.h>
#include<unistd.h>
#include"AES.h"
using namespace std;
#define LEN 128
int count = 0;
int cnt = 1;
// gcc -o main main.cpp fun.cpp openssl_test.cpp  -lstdc++ -ljson -lssl
int exitlogin_flag = 0;
//string key = "12345678";
const char key[17] = "asdfwetyhjuytrfd";
void *heartbeat(void *arg)
{
	Json::Value val;
	int fds = (int)arg;
	val["type"] = MSG_TYPE_HEART;
	val["message"] = "online";
	while(1)
	{
		send(fds, val.toStyledString().c_str(), strlen(val.toStyledString().c_str()), 0);
		sleep(1);
	}
	
}
void fun(char *ip,unsigned short port)
{
	//连接服务器
	int sockfd = socket(AF_INET, SOCK_STREAM , 0);
	assert(sockfd != -1);
      
	struct sockaddr_in ser, cli;
	memset(&ser, 0, sizeof(ser));
	ser.sin_family = PF_INET;
	ser.sin_port = htons(port);
	ser.sin_addr.s_addr = inet_addr(ip);

	int res = connect(sockfd, (struct sockaddr*)&ser, sizeof(ser));
	assert(res != -1);

	//启动一个线程发送心跳包。
	pthread_t first;
	pthread_create(&first, NULL, heartbeat, (void*)sockfd);

	char buff[2150] = {0};
	recv(sockfd, buff, 2150, 0);// 接收公钥
	
	string pubkey = buff;
	unsigned char des_string[2200] = {0};
	aes_box_encrypt((unsigned char*)buff, des_string);
	string encryptkey = rsa_pub_encrypt(key, pubkey); //用公钥加密密钥
 	send(sockfd, encryptkey.c_str(), strlen(encryptkey.c_str()), 0);//发送密钥至服务端


	int fd = sockfd;
	cout<<"客户端socket"<<sockfd<<endl;

	while(1)
	{
		cout<<"choice :"<<endl;
		cout<<"1.register"<<endl;
		cout<<"2.login"<<endl;
		cout<<"3.exit"<<endl;
		int choice;
		cin>>choice;
		switch(choice)
		{
			case 1:
				{
					Register(fd);
				}break;
			case 2:
				{
					login(fd);
				}break;
			case 3:
				{
					exit(fd);
				}break;
			default:
				{
					cout<<"no choice,please try again"<<endl;
				}break;
		}
	}
}

void Register(int fd)
{
	string name;
	string pw;
  while(1)
 {
	cout<<"please cin name:"<<endl;
	cin>>name;

	cout<<"please cin pw:"<<endl;
	cin>>pw;

	//用json打包type  name   pw
	Json::Value val;
	val["type"] = MSG_TYPE_REGISTER;
	val["name"] = name;
	val["pw"] = pw;
	
	string messages = EncryptionAES(val.toStyledString(),key);
	
	//给服务器发送json包
	if(-1 == send(fd, messages.c_str(), strlen(messages.c_str()), 0))
	{
		cerr<<"send reason fail;errno:"<<errno<<endl;
		exit(0);
	}

	//接受服务器的返回
	Json::Value root;
	Json::Reader read;
	char buff[100] = {0};
	if(0 < recv(fd,buff,99,0))
	{
		string buffer = buff;
		string messages = DecryptionAES(buffer, key.c_str());
		//解析json包
		if(-1 == read.parse(messages.c_str(),root))
		{
			cerr<<"json prase fail;errno:"<<errno<<endl;
			exit(0);
		}
		
		//输出json
		cout<<"rreason:"<<root["rreason"].asString()<<endl;
	}


	Json::Value roots;
	roots["rreason"] = "用户已存在！";
	const char *buffer = roots.toStyledString().c_str();
	
	if(strcmp(buff, buffer)==0)
	{
		cout<<"注册失败！"<<endl;
		
	}
	else
	{
		break;
	}
	}

}
void talkgroup(int fd)
{
	
}
void *run(void *arg)
{
	int fds = (int)arg;
	
while(1){
	char buffers[128] = {0};
	Json::Value rroot;
	Json::Reader rread;
	if(0 < recv(fds,buffers,127,0))
	{
		string buffer = buffers;
		string messages = DecryptionAES(buffer, key.c_str());
		//解析json包
		//fcntl(fds, F_SETFL, fcntl(fds, F_GETFL, 0) | O_NONBLOCK);设置非阻塞io
		if(-1 == rread.parse(messages.c_str(),rroot))
		{
			cerr<<"json prase fail;errno:"<<errno<<endl;
			return NULL;
		}
		//输出json
		if(strcmp(rroot["messages"].asString().c_str(), "file") == 0)
		{
			char *path = rroot["filename"].asString().c_str();
			int fdss = open(path, RDWR, 0666);
			while(1)
			{
				char buffe[128] = {0};
				recv(fds, buffe, 127, 0);
				if(strcmp(buffe, "end") == 0)	break;
				string buffss = buffe;
				string messages = DecryptionAES(buffss,key);
				write(fdss, strlen(messages.c_str()), messages.c_str());
			}
				
		}
		else{
			cout<<"He saids:"<<rroot["messages"].asString()<<endl;
		}
	}
	else
	{
		close(fds);
	}
}
}
void login(int fd)
{
	string name;
	string pw;
	
	
	cout<<"please cin name:"<<endl;
	cin>>name;
	cout<<"please cin pw:"<<endl;
	cin>>pw;
	
	//用json打包type  name   pw
	Json::Value val;
	val["type"] = MSG_TYPE_LOGIN;
	val["name"] = name;
	val["pw"] = pw;

	//加密消息
	
	string messages = EncryptionAES(val.toStyledString(),key);

	//给服务器发送json包
	if(-1 == send(fd, messages.c_str(), strlen(messages.c_str()), 0))
	{
		cerr<<"send reason fail;errno:"<<errno<<endl;
	}
	
	//接受服务器的返回
	Json::Value roots;
	Json::Reader read;
	char buff[128] = {0};
	if(0 < recv(fd,buff,127,0))
	{
		string buffer = buff;
		string messages = DecryptionAES(buffer, key.c_str());
		//解析json包
		if(-1 == read.parse(messages.c_str(),roots))
		{
			cerr<<"json prase fail;errno:"<<errno<<endl;
			exit(0);
		}

		//输出json
		cout<<"lreason:"<<roots["lreason"].asString()<<endl;
	}
	
	
	string s1 = "用户名不存在！";
	string s2 = "密码不正确！";
	
	//判断登录是否成功
	if(s1.compare(roots["lreason"].asString())==0 || s2.compare(roots["lreason"].asString())==0)
	{
		count++;
		if(count == 5)
		{
			int seconds = 60*cnt;
			cout<<"please waitting %d s"<<seconds;
			sleep(seconds);
			count = 0;
			cnt *= 5;
		}
		return;
	}
	else
	{
		//启动一个线程（只负责接收服务器端的消息，然后输出）
		
		while(1)
		{
			pthread_t first;
			pthread_create(&first, NULL, run, (void*)fd);
			cout<<"choice:"<<endl;
			cout<<"1.getlist"<<endl;
			cout<<"2.talkone"<<endl;
			cout<<"3.modify password"<<endl;
			cout<<"4.transfile"<<endl;
			cout<<"5.offline"<<endl;
			int choice;
			cin>>choice;
			switch(choice)
			{
				case 1:
					{
						getlist(fd);break;
					}
				case 2:
					{
						talkone(fd);break;
					}
				case 3:
					{
						modifypw(fd, name);break;
					}
				case 4:{
						transfile(fd);break;
					}
				case 5:
					{
						offline(fd);//下线
						if(exitlogin_flag == 1)	
						{
							exitlogin_flag = 0;
							return;
						}
						else	break;
					}
				default:
				{
					cout<<"no chioce,plase try agin"<<endl;
				}
			}
		}
	
	}
}
void getlist(int fd)
{
	//用json打包type  name   pw
	Json::Value vals;
	vals["type"] = MSG_TYPE_GETLIST;
	
	string messages = EncryptionAES(vals.toStyledString(),key);

	//给服务器发送json包
	if(-1 == send(fd, messages.c_str(), strlen(messages.c_str()), 0))
	{
		cerr<<"send reason fail;errno:"<<errno<<endl;
		return;
	}

	//接受服务器的返回
	char buff[100] = {0};
	int num = 0;
	while(1)
	{
		num = recv(fd,buff,99,0);
		if(0 < num)
		{
			//解析json包
			cout<<buff<<endl;
			cout<<endl;
		}
		else if(0 == num)
		{
			//break;
		}
		else 
		{
			close(fd);
		}
	}
}

void modifypw(int fd)
{
	cout<<"please input original password!"<<endl;
	string str;
	cin>>str;
	
	Json::Value vals;
	vals["type"] = MSG_TYPE_MODIFYPW;
	vals["password"] = str;
	vals["name"] = name;
	string messages = EncryptionAES(vals.toStyledString(),key);
	
	if(-1 == send(fd, messages.c_str(), strlen(messages.c_str()), 0))
	{
		cerr<<"send message fail;errno:"<<errno<<endl;
		return;
	}
	
	Json::Value roots;
	Json::Reader read;
	char buff[128] = {0};
	if(0 < recv(fd,buff,127,0))
	{
		string buffer = buff;
		string messages = DecryptionAES(buffer, key.c_str());
		
		if(-1 == read.parse(messages.c_str(),roots))
		{
			cerr<<"json prase fail;errno:"<<errno<<endl;
			exit(0);
		}
	}
	string s1 = "密码正确！";
	if(s1.compare(roots["messages"].asString())==0)
	{
		cout<<"please input new password"<<endl;
		string new_str;
		cin>>new_str;
		vals["type"] = MSG_TYPE_MODIFYPW;
		vals["password"] = new_str;
	
		messages = EncryptionAES(vals.toStyledString(),key);

		if(-1 == send(fd, messages.c_str(), strlen(messages.c_str()), 0))
		{
			cerr<<"send message fail;errno:"<<errno<<endl;
			return;
		}
	
	}
	char buffs[128] = {0};
	if(0 < recv(fd,buffs,127,0))
	{
		string buffer = buffs;
		string messages = DecryptionAES(buffer, key.c_str());
		
		if(-1 == read.parse(messages.c_str(),roots))
		{
			cerr<<"json prase fail;errno:"<<errno<<endl;
			exit(0);
		}
		cout<<roots["messages"].asString()<<endl;
	}
}
void funs(int sign)
{
	exit(0);
	signal(SIGINT, SIG_DFL);
}
void talkone(int fd)
{
	cout<<"please input your friend name!"<<endl;
	string name;
	cin>>name;

	//用json打包type  name   pw
	Json::Value vals;
	vals["type"] = MSG_TYPE_TALKONE;
	vals["name"] = name;
	string messages = EncryptionAES(vals.toStyledString(),key);
	
	//给服务器发送json包
	if(-1 == send(fd, messages.c_str(), strlen(messages.c_str()), 0))
	{
		cerr<<"send message fail;errno:"<<errno<<endl;
		return;
	}
	
	cout<<"如果你想进行聊天请按数字1，输入'end'退出聊天！"<<endl;

	//Json::Value roott;
	//Json::Reader readd;

	int choice = 0;
	cin>>choice;
	if(choice == 1){
	while(1)
	{
		char buff[128] = {0};
		fgets(buff, 127, stdin);
	
		vals["messages"] = buff;
		string messages = EncryptionAES(vals.toStyledString(),key);
			
		//给服务器发送json包
		if(-1 == send(fd, messages.c_str(), strlen(messages.c_str()), 0))
		{
			cerr<<"send reasons fail;errno:"<<errno<<endl;
			return;
		}
		if(strncmp(buff, "end", 3) == 0)	break;
	}
	}
}

void transfile(int fd)
{
	cout<<"please input your friend name!"<<endl;
	string friname;
	cin>>name;
	cout<<"please input your filename!"<<endl;
	string filename;

	//用json打包type  name   pw
	Json::Value vals;
	vals["type"] = MSG_TYPE_TALKONE;
	vals["name"] = name;
	vals["filename"] = filename;

	string messages = EncryptionAES(vals.toStyledString(),key);
	//给服务器发送json包
	if(-1 == send(fd, messages.c_str(), strlen(messages.c_str()), 0))
	{
		cerr<<"send message fail;errno:"<<errno<<endl;
		return;
	}
	char path[] = filename.c_str();
	int fds = open(path, RDONLY, 0666);
	char buffer[128];
	int len = 0;
	while((read(fds, 127, buff)) > 0)
	{
		string buffs  = buff;
		string messages = EncryptionAES(buffs,key);
		//给服务器发送json包
		if(-1 == send(fd, messages.c_str(), strlen(messages.c_str()), 0))
		{
			cerr<<"send message fail;errno:"<<errno<<endl;
			return;
		}
	}

	string flag = "end";
	string messages = EncryptionAES(buffs,key);
		//给服务器发送json包
	if(-1 == send(fd, messages.c_str(), strlen(messages.c_str()), 0))
	{
		cerr<<"send message fail;errno:"<<errno<<endl;
		return;
	}
	cout<<"send finished!"<<endl;
	
}
void offline(int fd)
{
	cout<<"你确定要退出登录吗? 请选择Y or N."<<endl;
	char choice;
	cin>>choice;
	if(choice == 'Y')
	{	
		exitlogin_flag = 1;
		string name;
		cout<<"请输入用户名："<<endl;
		cin>>name;
		Json::Value val;
		Json::Value root;
		Json::Reader read;
		val["type"] = MSG_TYPE_OFFLINE;
		val["name"] = name;
		
		string messages = EncryptionAES(val.toStyledString(),key);

		//给服务器发送json包
		if(-1 == send(fd, messages.c_str(), strlen(messages.c_str()), 0))
		{
			cerr<<"send message fail;errno:"<<errno<<endl;
			return;
		}
		
	}
}
void exits(int fd)
{
	cout<<"你确定要退出此系统吗? 请选择Y or N."<<endl;
	char choice;
	cin>>choice;
	if(choice == 'Y')
	{
		close(fd);
		exit(0);
	}
	if(choice == 'N')
	{
		cout<<"您退出系统成功！"<<endl;
		return;
	}
}


