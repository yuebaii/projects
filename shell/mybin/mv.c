#include<stdio.h>

int main(int argc, char *argv[])
{
	if(argc < 3)
	{
		printf("arguements excepted 3\n");
	}
	rename(argv[1], argv[2]);
}
