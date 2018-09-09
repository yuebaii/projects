#ifndef VIEW_REGISTER_H
#define VIEW_REGISTER_H
#include<json/json.h>
#include"view.h"
#include<string>
class view_register:public view
{
	public:
		view_register(int cli_fd);
		void processs(Json::Value val,int cli_fd,string key);
		void responce();
	private:
	
		int cli_fds;
};

#endif
