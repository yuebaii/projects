#include<stdio.h>
#include<string.h>
#include<unistd.h>
#include<fcntl.h>
#include<assert.h>

//mkdir abc
int main(int argc, char *argv[])
{
	mkdir(argv[1], 0664);
}
