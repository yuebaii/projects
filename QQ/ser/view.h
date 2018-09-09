#ifndef VIEW_H
#define VIEW_H
#include<json/json.h>
class view
{
	public:
		//view(){}
		virtual void processs(Json::Value val,int cli_fd)=0;
		virtual void responce()=0;
};

#endif
