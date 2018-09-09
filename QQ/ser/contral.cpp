#include<iostream>
#include"contral.h"
#include"view.h"
#include"view_register.h"
#include<string>
#include"view_login.h"
#include"view_talkone.h"
#include"view_modifypw.h"
#include"view_getlist.h"
#include"view_offline.h"
#include"view_exit.h"
#include"view_heart.h"
#include"public.h"
#include<map>
#include"openssl_test.h"
#include<errno.h>
#include<json/json.h>
#include"AES.h"
using namespace std;

contral::contral(int fd)
{
	_map.insert(make_pair(MSG_TYPE_REGISTER, new view_register(fd)));

	_map.insert(make_pair(MSG_TYPE_LOGIN, new view_login(fd)));

	_map.insert(make_pair(MSG_TYPE_TALKONE, new view_talkone(fd)));
	
	_map.insert(make_pair(MSG_TYPE_MODIFYPW, new view_modifypw(fd)));

	_map.insert(make_pair(MSG_TYPE_GETLIST, new view_getlist(fd)));
	
	_map.insert(make_pair(MSG_TYPE_HEART, new view_heart(fd)));

	_map.insert(make_pair(MSG_TYPE_OFFLINE, new view_offline(fd)));

	_map.insert(make_pair(MSG_TYPE_EXIT, new view_exit(fd)));
	
	fds = fd;
}
//void contral::process(unsigned char *buff, int cli_fd)
void contral::process(unsigned char *buff, int cli_fd, string key)
{
	//解析buff -》json
	Json::Value vall;
	string buffer = (const char *)buff;
	//aes  解密消息
	string messages = DecryptionAES(buffer, key.c_str());
	cout<<messages<<endl;
	
	Json::Reader *pJsonParser = new Json::Reader(Json::Features::strictMode());
	buff = (unsigned char *)messages.c_str();
	if(!pJsonParser->parse((const char*)buff, vall))
	{
		std::cout<<"json prase fail!"<<std::endl;
		return;
	}

	
	map<int ,view*>::iterator it =_map.find(vall["type"].asInt());//map从0开始
	it->second->processs(vall,cli_fd, key);
}
