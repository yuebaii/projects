#include<stdio.h>
#include<string.h>
#include<assert.h>
#include<signal.h>
#include<dirent.h>
#include<sys/stat.h>
#include<unistd.h>
#include<stdlib.h>
#include<sys/types.h>
#include<pwd.h>
#include<grp.h>
#include<time.h>
int main(int argc, char *argv[])
{
	 char path[128];
	 DIR *dp = opendir(getcwd(path, 128));
	 struct dirent* entry;
	 struct stat statbuf;
	 while((entry=readdir(dp)) != NULL)
        {
	 	 lstat(entry->d_name, &statbuf);
		 if(argc == 1)
		 {
		 if(strcmp(".",entry->d_name)==0 || strcmp("..",entry->d_name)==0)
		 {
			 continue;
		 }

		 if(S_ISDIR(statbuf.st_mode))
		 {
			 printf("\033[1;34m%s  \033[0m", entry->d_name);
		 }
		 else if(S_ISREG(statbuf.st_mode))
		 {
			 if(statbuf.st_mode & S_IXUSR || statbuf.st_mode & S_IXGRP || statbuf.st_mode & S_IXOTH)
			{
				printf("\033[1;32m%s  \033[0m", entry->d_name);
			}
			else
			{printf("%s ", entry->d_name);}
		 }
		 else
		 {
            		 printf("%s ", entry->d_name);
		 }
		 }
	 
	     else{   	 
		 if(strncmp(argv[1], "-i", 2) == 0)
		 {
			if(strcmp(".",entry->d_name)==0 || strcmp("..",entry->d_name)==0)
		 {
			 continue;
		 }
			 if(S_ISDIR(statbuf.st_mode))
		 {
			 printf("%d \033[1;34m%s  \033[0m", entry->d_ino, entry->d_name);
		 }
		 else if(S_ISREG(statbuf.st_mode))
		 {
			 if(statbuf.st_mode & S_IXUSR || statbuf.st_mode & S_IXGRP || statbuf.st_mode & S_IXOTH)
			{
				printf("%d \033[1;32m%s  \033[0m", entry->d_ino, entry->d_name);
			}
			else
			{printf("%d %s %d ", entry->d_ino, entry->d_name, statbuf.st_size);}
		 }
		 }

		 if(strncmp(argv[1], "-al", 2) == 0 || strncmp(argv[1], "-l", 2) == 0)
		 {
			 if(strncmp(argv[1], "-l", 2)==0 &&(strcmp(".",entry->d_name)==0 || strcmp("..",entry->d_name)==0))
		          {
			              continue;
		           }
			 
			 if(S_ISREG(statbuf.st_mode))
			 {
				 printf("-");
			 }
			 else if(S_ISDIR(statbuf.st_mode))
			 {
				 printf("d");
			 }
			 else if(S_ISCHR(statbuf.st_mode))
			 {
				 printf("c");
			 }
			 else if(S_ISBLK(statbuf.st_mode))
			 {
				 printf("b");
			 }
			 else if(S_ISFIFO(statbuf.st_mode))
			 {
				 printf("p");
			 }
			 else if(S_ISLNK(statbuf.st_mode))
			 {
				 printf("l");
			 }
			 else if(S_ISSOCK(statbuf.st_mode))
			 {
				 printf("s");
			 }
			 else
			 {
				 printf("error");
			 }

			 int n;
			 for(n = 8; n >= 0; n--)//权限位共9位
			 {
				 if(statbuf.st_mode&(1<<n))//判断每位是0还是1
				 {
					 switch(n%3)
					 {
						 case 2:
							 printf("r");
							 break;
						 case 1:
							 printf("w");
							 break;
						 case 0:
							 printf("x");
							 break;
						 default:
							 break;
					 }
				 }
				 else
				 {
					 printf("-");
				 }
			 }
				 printf("%d ", statbuf.st_nlink);
				 struct passwd *pw = getpwuid(getuid());//根据用户ID返回用户的数据的结构体（含有用户名/用户ID和组ID）
				 printf("%s ", pw->pw_name);
				 struct group *gp = getgrgid(pw->pw_gid);//根据组ID返回组的数据的结构体（含有组名/组ID和组密码）  pw->pw_gid
				 printf("%s ", gp->gr_name);
				 printf("%d ", statbuf.st_size);
				 long *t = &statbuf.st_atime;
				 struct tm* time = localtime(t);
				 printf("%d %d %d:%d  ",time->tm_mon+1, time->tm_mday, time->tm_hour, time->tm_min);
			//	 printf("%d ", statbuf.st_atime);
			//	 printf("%d ", statbuf.st_mtime);
			//	 printf("%d ", statbuf.st_ctime);
			 if(statbuf.st_mode & S_IXUSR || statbuf.st_mode & S_IXGRP || statbuf.st_mode & S_IXOTH)
			{
				printf("\033[1;32m%s  \033[0m",  entry->d_name);
			}
			 else
			 {
				 printf("%s", entry->d_name);
			 }

				 printf("\n");
		 }
		 }
	 }

	closedir(dp);
		
     printf("\n");
	 
 }
		 

