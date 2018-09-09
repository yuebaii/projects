#ifndef VIEW_MODIFYPW_H
#define VIEW_MODIFYPW_H
#include<json/json.h>
#include"view.h"
class view_modifypw:public view
{
	public:
		view_modifypw(int cli_fd);
		void processs(Json::Value val,int cli_fd);
		void responce();
	private:
		int cli_fds;
};

#endif
