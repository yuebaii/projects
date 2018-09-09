#ifndef MPTHREAD_H
#define MPTHREAD_H
#include<iostream>
#include<map>
#include<event.h>
typedef class mpthread
{
	public:
		mpthread(int sock_1);
		~mpthread(){};
		friend void cli_cb(int fd, short event, void *arg);
		friend void sock_11_cb(int fd, short event, void *arg);
		friend void *run(void *arg);
		
	private:
		 int _sock_1;
		struct event_base* _base;
		std::map<int,struct event*> _event_map;				
}Mpthread,*Pmpthread;

#endif
