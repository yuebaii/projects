#ifndef TCPSEVER_H
#define TCPSEVER_H
#include<iostream>
#include<map>
#include<vector>
//typedef  map <int, int> MY_MAP
typedef class tcpsever
{
	public:
		tcpsever(char *ip, unsigned short port, int pth_num);
		~tcpsever(){}
		void run();
		void create_pth();
		void create_socketpair();
		std::map<int, int> _pth_event_num;
		char *ipp;
		unsigned short portt;
		
	private:
		int pth_numm;
		struct event_base* _base;//libevent
		int _listen_fd;
		//std::map<int, int> _pth_event_num;//对应线程监听量2
		std::vector<int *> _socketpair_base;
		std::vector<int> tmp;
		//std::string key;
}Tcpsever,*Ptcpsever;



#endif
