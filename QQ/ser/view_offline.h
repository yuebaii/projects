#ifndef VIEW_OFFLINE_H
#define VIEW_OFFLINE_H
#include<json/json.h>
#include"view.h"
class view_offline:public view
{
	public:
		view_offline(int cli_fd);
		void processs(Json::Value val,int cli_fd,string key);
		void responce();
	private:
	
		int cli_fds;
};

#endif
