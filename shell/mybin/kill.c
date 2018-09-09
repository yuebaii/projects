#include<stdio.h>
#include<assert.h>
#include<signal.h>

int main(int argc, char *argv[])
{
	if(argc < 2)
	{
		printf("please input pid\n");
	}

	int pid = 0;
	sscanf(argv[1], "%d", &pid);

	if(kill(pid, SIGKILL) == -1)
	{
		perror(NULL);
	}
}
