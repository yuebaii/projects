#include<iostream>
#include<unistd.h>

using namespace std;

int main(int agrc, char *argv[])
{
	pid_t pid = fork();
	if(pid == 0)
	{
		execl("./main", "./main", "127.0.0.1", argv[1], (char *)0);
		
	}
}
