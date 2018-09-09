#include<stdio.h>
#include<unistd.h>
#include<string.h>
int main(int argc, char *argv[])
{
	int times = atoi(argv[1]);
	//unsigned int time = (unsigned int)(argv[1]-'0');
	unsigned int time =(unsigned int)times;
	printf("start.");
	sleep(time);
	printf("end!");
	return 0;
}
