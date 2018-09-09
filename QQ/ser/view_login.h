#ifndef VIEW_LOGIN_H
#define VIEW_LOGIN_H
#include<json/json.h>
#include"view.h"
class view_login:public view
{
	public:
		view_login(int cli_fd);
		void processs(Json::Value val,int cli_fd,string key);
		void responce();
	private:
	
		int cli_fds;
};

#endif
