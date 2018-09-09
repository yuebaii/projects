#ifndef VIEW_HEART_H
#define VIEW_HEART_H
#include<json/json.h>
#include"view.h"
class view_heart:public view
{
	public:
		view_heart(int cli_fd);
		void processs(Json::Value val,int cli_fd);
		void responce(){}
	private:
		int cli_fds;
};

#endif
