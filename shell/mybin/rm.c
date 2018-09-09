#include<stdio.h>
#include<stdlib.h>
#include<assert.h>
#include<unistd.h>
#include<string.h>
#include<sys/stat.h>
#include<dirent.h>

void deletedir(char *dir)
{
   		 DIR *dp;
		 struct dirent *entry;
		 struct stat statbuf;

		 dp = opendir(dir);
		if(dp == NULL)	printf("NULL");
		 chdir(dir);
		//printf("1\n");
		 while((entry = readdir(dp)) != NULL)
		 {
			 lstat(entry->d_name, &statbuf);
			 if(S_ISDIR(statbuf.st_mode))
			 {
				 if(strcmp(".",entry->d_name)==0 || strcmp("..", entry->d_name)==0)	
				continue;
				 //rmdir(entry->d_name);
				 deletedir(entry->d_name);
			 }
			 else
			 {
				 unlink(entry->d_name);
			 }

		 }
		//printf("2\n");
		 chdir("..");
		rmdir(dir);
		 closedir(dp);
}
int main(int argc, char *argv[])
{
	if(argc < 3)//删除文件
	{
     int flag = unlink(argv[1]);
     if(flag != 0)
    {
	 	 printf("No such file or directory");
    }
	}
	else
	{	
		deletedir(argv[2]);//删除非空目录
	}
}



