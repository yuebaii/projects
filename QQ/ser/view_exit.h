#ifndef VIEW_EXIT_H
#define VIEW_EXIT_H
#include<json/json.h>
#include"view.h"
class view_exit:public view
{
	public:
		view_exit(int cli_fd);
		void processs(Json::Value val,int cli_fd);
		void responce();
	private:
	
		int cli_fds;
};

#endif
