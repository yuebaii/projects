#ifndef VIEW_TALKONE_H
#define VIEW_TALKONE_H
#include<json/json.h>
#include"view.h"
#include<string>
class view_talkone:public view
{
	public:
		view_talkone(int cli_fd);
		void processs(Json::Value val,int cli_fd,string key);
		void responce();
	private:
	
		int cli_fds;
};

#endif
