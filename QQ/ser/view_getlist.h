#ifndef VIEW_GETLIST_H
#define VIEW_GETLIST_H
#include<json/json.h>
#include"view.h"
class view_getlist:public view
{
	public:
		view_getlist(int cli_fd);
		void processs(Json::Value val,int cli_fd);
		void responce();
	private:
		int cli_fds;
};

#endif


