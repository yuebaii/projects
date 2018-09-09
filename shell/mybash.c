#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <assert.h>
#include <signal.h>
#include <sys/types.h>
#include <pwd.h>
#include <sys/utsname.h>
#include <fcntl.h> 
#include <sys/types.h>
#include <sys/stat.h> 
#include <errno.h>
#include<termios.h>
#include "wshell.h"

#define LENGTH 20
#define TRUE 1
#define MAXPIDTABLE 1024
#define KEYCODE_R 0x43 
#define KEYCODE_L 0x44
pid_t BPTable[MAXPIDTABLE];
char *cmdArr[LENGTH]={" "};
int count = 0;
int num = 0;
char OLDPWD[128] = {0};

int kfd = 0;
struct termios cooked, raws;

void quit(int sig)
{
  (void)sig;
  tcsetattr(kfd, TCSANOW, &cooked);//在程序结束时在恢复原来的配置
  exit(0);
}
void sig_handler(int sig)
{
    pid_t pid;
    int i;
    for(i=0;i<MAXPIDTABLE;i++)
        if(BPTable[i] != 0) //only handler the background processes
        {
            pid = waitpid(BPTable[i],NULL,WNOHANG);
            if(pid > 0)
            {
                printf("process %d exited.\n",pid);
                BPTable[i] = 0; //clear
            }
            else if(pid < 0)
            {
	if(errno != ECHILD)
                    perror("waitpid error");
            }
            //else:do nothing.
            //Not background processses has their waitpid() in wshell.
         }
    return;
}

//提示符输出函数
void out_flag()
{
	char path[128] = {0};
	if(NULL == getcwd(path, 128))
	{
		printf("getcwd error\n");
		return;
	}

	char dir[128] = {0};
	char *p = strtok(path, "/");
	while(p != NULL)
	{
		strcpy(dir, p);
		p = strtok(NULL, "/");
	}

	if(strlen(dir) == 0)
	{
		strcpy(dir, "/");
	}
	
	struct passwd *pw = getpwuid(getuid());
	if(pw == NULL)
	{
		printf("error\n");
		return;
	}
	if(strcmp(pw->pw_dir, getcwd(path, 128)) == 0)
	{
		memset(dir, 0, 128);
		strcpy(dir, "~");
	}

	struct utsname host;
	uname(&host);

	char *hname = strtok(host.nodename, ".");
	if(hname == NULL)
	{
		printf("error\n");
		return;
	}

	char flag = '$';

	if(pw->pw_uid == 0)
	{
		flag = '#';
	}

	printf("[%s@%s %s]%c ", pw->pw_name, hname, dir, flag);
	fflush(stdout);
}

//命令切割函数
int cut_cmd(char *cmd)
{
	count = 0;
	memset(cmdArr, 0, LENGTH * sizeof(cmdArr[0]));
	char *p = strtok(cmd, " ");
	while(p != NULL)
	{
    		cmdArr[count++] = p;
		p = strtok(NULL, " ");
	}
	
	return count;
}

//cd命令实现
void my_cd()
{
	/*
	 * cd 绝对路径    相对路径    chdir(路径);
	 * cd ~    cd                直接进入当前用户的家目录
	 * cd -            切换到上一次所在的目录
	*/

	char nowpwd[128] = {0};
	getcwd(nowpwd, 128);

	if(cmdArr[1] == NULL || strncmp(cmdArr[1], "~", 1) == 0)
	{
		struct passwd *pw = getpwuid(getuid());
		if(chdir(pw->pw_dir) == -1)
		{
			perror(pw->pw_dir);
			return;
		}
	}
	else if(strncmp(cmdArr[1], "-", 1) == 0)
	{
		if(strlen(OLDPWD) == 0)
		{
			printf("OLDPWD not set\n");
			return;
		}
		printf("%s\n", OLDPWD);
		if(chdir(OLDPWD) == -1)
		{
			perror(OLDPWD);
			return;
		}
	}
	else
	{
		if(chdir(cmdArr[1]) == -1)
		{
			perror(cmdArr[1]);
			return;
		}
	}

	strcpy(OLDPWD, nowpwd);
}

void *fun(void *arg)
{
	char c;

  // get the console in raw mode 
  
  for(;;)
  {
    // get the next event from the keyboard 
    if(read(kfd, &c, 1) < 0)
    {
      perror("read():");
      exit(-1);
    }
  tcgetattr(kfd, &cooked); // 得到 termios 结构体保存，然后重新配置终端
  memcpy(&raws, &cooked, sizeof(struct termios));
  raws.c_lflag &=~ (ICANON | ECHO);
  // Setting a new line, then end of file 
  raws.c_cc[VEOL] = 1;
  raws.c_cc[VEOF] = 2;
  tcsetattr(kfd, TCSANOW, &raws);

    switch(c)
    {
      case KEYCODE_L:
        printf("\033[1D");
	tcsetattr(kfd, TCSANOW, &cooked);
        break;
      case KEYCODE_R:
        printf("\033[1C");
        break;
    }
  }
  return;
}
int main(int argc, char *argv[])
{
	//signal(SIGINT,quit);
	//signal(SIGINT, SIG_IGN);
	if(signal(SIGCHLD,sig_handler) == SIG_ERR)
        perror("signal() error");
	
    	int ParaNum;
	int status;
    	struct parse_info info;
	 pid_t ChdPid,ChdPid2;
	int pipe_fd[2];
	int out_fd;
	int in_fd;

	while(1)
	{
		out_flag();
		//char cmd[128] = {0};
		//fgets(cmd, 128, stdin);

		read_history(NULL);//读当前文件所在目录的历史命令记录
		printf("a \n");
		pthread_t thrid;
		pthread_create(&thrid, NULL, fun, NULL);
		char *cmd = readline(NULL);//输入命令，并tab键补全文件名
		add_history(cmd);//上下键查找此终端的历史命令
		write_history(NULL);//将历史命令写入记录文件中
		

		//cmd[strlen(cmd) - 1] = 0;
		if(strlen(cmd) == 0)
		{
			continue;
		}

		ParaNum = cut_cmd(cmd);
		free(cmd);
		parsing(cmdArr,ParaNum,&info);//解析部分命令

		
		if(cmdArr[0] == NULL)
		{
			continue;
		}
		
		if(strncmp(cmdArr[0], "cd", 2) == 0)
		{
			my_cd();
			continue;
		}
		if(strncmp(cmdArr[0], "exit", 4) == 0)
		{
			exit(0);
		}
		
		if(info.flag & IS_PIPED) //command2 is not null有管道
        	{                
            		if(pipe(pipe_fd)<0)
            		{
                		printf("Wshell error:pipe failed.\n");
                		exit(0);
            		}
       		}  
/*----------------------------------------------------------------*/		
		if((ChdPid = fork())!=0) //wshell父进程
        	{
            		if(info.flag & IS_PIPED)//如果有命令2
            		{
                		if((ChdPid2=fork()) == 0) //command2
                		{
                    			close(pipe_fd[1]);//先关闭写端，
                    			close(fileno(stdin)); 
                    			dup2(fileno(stdin)， pipe_fd[0]);//读端等待有数据，然后将读端重定位到标准输入。。
                    			close(pipe_fd[0]);

		    			char path[128] = "/home/yuebai/Documents/projects/shell/mybin/";
					if(strstr(cmdArr[0], "/") != NULL)
					{
						memset(path, 0, 128);
					}
					strcat(path, info.command2);
					execv(path, info.parameters2);
                  			//  execvp(info.command2,info.parameters2);//执行第二个命令
               	 		}
                		else
                		{
                    			close(pipe_fd[0]);
                    			close(pipe_fd[1]);
                    			waitpid(ChdPid2,&status,0); //wait command2等待父进程中子进程的命令2执行完成
                		}
            		}

            		if(info.flag & BACKGROUND)//如果有后台进程,不等待子进程命令完成。但会有僵死进程，得用信号处理。。
            		{
                		printf("Child pid:%u\n",ChdPid);
                		int i;
                		for(i=0;i<MAXPIDTABLE;i++)
                    			if(BPTable[i]==0)
                        		BPTable[i] = ChdPid; //register a background process
                		if(i==MAXPIDTABLE)
                    		perror("Too much background processes\nThere will be zombine process");                    
            		}
            		else
            		{          
                		waitpid(ChdPid,&status,0);//wait command1等待子进程命令完成
            		} 
        	}
        	else //子进程
        	{
			if(info.flag & IS_PIPED) //command2 is not null 有管道
            		{                
                		if(!(info.flag & OUT_REDIRECT) && !(info.flag & OUT_REDIRECT_APPEND)) // ONLY PIPED 没有输出重定位的命令2 并且 没有输出追加重定位的命令2
	           		{
                    			close(pipe_fd[0]);//先关闭管道读端
                    			close(fileno(stdout)); 
                    			dup2(pipe_fd[1], fileno(stdout));//将标准输出重定向到写端，让父进程的读端去读取数据。。
                    			close(pipe_fd[1]);
                		}
                		else //OUT_REDIRECT and PIPED
	           		{
                    			close(pipe_fd[0]);
                    			close(pipe_fd[1]);//send a EOF to command2 ，先关闭管道的读端和写端
                    			if(info.flag & OUT_REDIRECT)
    	               				out_fd = open(info.out_file, O_WRONLY|O_CREAT|O_TRUNC, 0666);//是有输出重定位的命令2 
                   			else
    	               				out_fd = open(info.out_file, O_WRONLY|O_APPEND|O_TRUNC, 0666);//是有输出追加重定位的命令2
                    			close(fileno(stdout)); 
                    			dup2(out_fd, fileno(stdout));//将标准输出重定向到命令指定的文件中去
                    			close(out_fd);	        
                		}
            		}
            		else
            		{
                		if(info.flag & OUT_REDIRECT) // OUT_REDIRECT WITHOUT PIPE 没有管道的输出重定向
	           		{
                    			out_fd = open(info.out_file, O_WRONLY|O_CREAT|O_TRUNC, 0666); 
                    			close(fileno(stdout)); 
                    			dup2(out_fd, fileno(stdout));//直接将标准输出重定向到命令指定的文件中
                    			close(out_fd);
                		}
                		if(info.flag & OUT_REDIRECT_APPEND) // OUT_REDIRECT_APPEND WITHOUT PIPE 没有管道的输出追加重定向
	           		{
                    			out_fd = open(info.out_file, O_WRONLY|O_CREAT|O_APPEND, 0666);
                    			close(fileno(stdout)); 
                    			dup2(out_fd, fileno(stdout));//同上。。
                    			close(out_fd);
                		}
            		}
            
            		if(info.flag & IN_REDIRECT)//有输入重定向的命令
            		{
                		in_fd = open(info.in_file, O_CREAT |O_RDONLY, 0666);
                		close(fileno(stdin)); 
                		dup2(in_fd, fileno(stdin));//直接将标准输入重定向到命令指定的文件中
                		close(in_fd); 
            		}

			char path[128] = "/home/yuebai/Documents/projects/shell/mybin/";

			if(strstr(cmdArr[0], "/") != NULL)
			{
				memset(path, 0, 128);
			}
			strcat(path, cmdArr[0]);
			execv(path, cmdArr);//执行普通命令
		
            		//execvp(command,parameters);
        }
	}
}
