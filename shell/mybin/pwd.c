#include<stdio.h>
#include<string.h>
#include<assert.h>
#include<stdlib.h>
#include<unistd.h>

int main()
{
	char buf[128];
	char *path = getcwd(buf,128);
	printf("%s\n", path);
}

