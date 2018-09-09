#ifndef CONTRAL_H
#define CONTRAL_H
#include"public.h"
#include"view.h"
#include<string>
#include<map>
class contral
{
	public:
		contral(int fd);
		//~contral(){};
		
		void process(unsigned char *buff,int cli_fd, std::string key);

	private:
		std::map<int, view*> _map;
		int fds;
};//Contral,*Pcontral;

#endif
