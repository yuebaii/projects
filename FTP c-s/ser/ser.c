#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/epoll.h>
#include <fcntl.h>
#include <semaphore.h>
#include <pthread.h>
#define LEN 128
//注：下载操作与上传操作均在服务器程序完成。。。

#define  MAX  10
int fds[MAX];
sem_t sem;
pthread_mutex_t mutex;

struct epoll_event revents[MAX];
int j = 0;
void Init_fds()
{
	int i = 0;
	for(; i < MAX; ++i)
	{
		fds[i] = -1;
	}
}

int Add_fds(int c)
{
	pthread_mutex_lock(&mutex);

	int i = 0;
	for(; i < MAX; ++i)
	{
		if(fds[i] == -1)
		{
			fds[i] = c;
			pthread_mutex_unlock(&mutex);
			return 1;
		}
	}

	if(i == MAX)
	{
		pthread_mutex_unlock(&mutex);
		return 0;
	}
}

void Del_fds(int i)
{
	for(; i < MAX - 1; ++i)
	{
		fds[i] = fds[i + 1];

		if(fds[i+1] == -1)
		{
			break;
		}
	}
	fds[MAX - 1] = -1;
}

int Get_fds()
{
	pthread_mutex_lock(&mutex);

	int i = 0, c = -1;
	for(; i < MAX; ++i)
	{
		if(fds[i] != -1)
		{
			c = fds[i];
			Del_fds(i);
			break;
		}
	}

	pthread_mutex_unlock(&mutex);
	return c;
}

typedef enum 
{
	UP = 1,
	DOWN,
	CMD
}Op;

typedef struct
{
	Op option;
	char data[LEN];  // ls -l    /  cd  ..  
}Data;

void UpFile(int c, Data val)
{
	int fd = open(val.data,O_RDWR | O_CREAT | O_TRUNC, 0666);
	if(fd == -1)
	{
		printf("server open file fail\n");
		return;
	}

			char recvbuff[LEN] = {0};
			recv(c,recvbuff,LEN-1,0);//ER   OK#1000#this is//接受客户端的文件数据

			char *p = strtok(recvbuff,"#");
			p = strtok(NULL,"#");//得到文件大小,计算百分比

			int size = 0,count = 0;
			sscanf(p,"%d",&size);
			if(p != NULL)
			{
				p = strtok(NULL,"\0"); //得到文件内容
			}
			if(p != NULL)
			{
				 count = strlen(p);
				write(fd,p,count);
			}

			printf("\033[?25l");//隐藏光标
			while(1)
			{
				printf("%.2f %%", count*1.0/size*100);
				fflush(stdout);
				printf("\033[100D\033[K");//清除光标到行尾的内容
				memset(recvbuff, 0, LEN);

				int n = recv(c,recvbuff,LEN-1,0);
				write(fd,recvbuff,n);
				count += n;

				if(count == size)
				{
					close(fd);
					break;
				}
			}

			printf("\033[100D\033[K");
			printf("100.00 %\n");
			printf("\033[?25h");//恢复光标
}
void DownFile(int c, Data val) // val:  filename
{
	int fd = open(val.data, O_RDONLY);
	if(fd == -1)
	{
		send(c, "ER", 2, 0);
		return;
	}

	struct stat st;
	fstat(fd, &st);
	char buff[128] = "OK#";
	sprintf(buff+3, "%d", st.st_size);
	strcat(buff, "#");
	send(c, buff, strlen(buff), 0);
	while(1)
	{
		memset(buff, 0, 128);
		int n = read(fd, buff, 127);
		if(n == 0)
		{
			close(fd);
			break;
		}

		send(c, buff, n, 0);
	}
}
void ExecuteCmd(int c, Data val) //  ls   pwd   cd
{
	char buff[LEN] = {0};
	strcpy(buff, val.data);

	char *command[LEN] = {0};
	int count = 0;
	char *p = strtok(buff, " ");
	while(p != NULL)
	{
		command[count++] = p;
		p = strtok(NULL, " ");
	}

	if(strncmp(command[0], "cd", 2) == 0)
	{
		if(command[1] != NULL)
		{
			if(-1 == chdir(command[1]))
			{
				send(c, "ERR\n###", 7, 0);
			}
			else
			{
				send(c, "OK\n###", 6, 0);
			}
		}
		else
		{
			send(c, "ERR\n###", 7, 0);
		}
	}
	else
	{
		int fds[2];
		pipe(fds);
		pid_t n = fork();
		assert(n != -1);
		if(n == 0)
		{
			close(fds[0]);
			close(1);
			close(2);
			dup(fds[1]);
			dup(fds[1]);
			char path[LEN] = "/bin/";
			strcat(path, command[0]);
			execv(path, command);
			perror(" ");
			exit(0);
		}
		else
		{
			close(fds[1]);
			while(1)
			{
				char data[LEN] = {0};
				int n = read(fds[0], buff, LEN - 1);
				if(n <= 0)
				{
					close(fds[0]);
					send(c, "###", 3, 0);
					break;
				}

				send(c, buff, n, 0);
			}
		}
	}
}

void *DisposeClient(void *arg) // 业务逻辑处理函数
{
	while(1)
	{
	sem_wait(&sem);
	printf("c\n");
	int c = Get_fds();
	if(c == -1)
	{
		continue;
	}
	while(1)
	{
		Data val;
		memset(&val, 0, sizeof(val));
		int n = recv(c, &val, sizeof(val) - 1, 0);
		if(n <= 0)
		{
			close(c);
			revents[j].data.fd = -1;
			break;
		}

		switch(val.option)
		{
			case UP:
				{
					UpFile(c, val);
					break;
				}
			case DOWN:
				{
					DownFile(c, val);
					break;
				}
			case CMD:
				{
					ExecuteCmd(c, val);
					break;
				}
			default:
				{
					break;
				}
		}
	}
	}
}

void main()
{
	Init_fds();
	sem_init(&sem, 0, 0);
	pthread_mutex_init(&mutex, NULL);
	int i = 0;
	for(; i < 3; ++i)
	{
		pthread_t id;
		int res = pthread_create(&id, NULL, DisposeClient, NULL);
		assert(res == 0);
	}

	int sockfd = socket(AF_INET, SOCK_STREAM, 0);
	assert(sockfd != -1);

	struct sockaddr_in ser, cli;
	memset(&ser, 0, sizeof(ser));
	ser.sin_family = AF_INET;
	ser.sin_port = htons(6500);
	ser.sin_addr.s_addr = inet_addr("192.168.52.129");
	int res = bind(sockfd, (struct sockaddr *)&ser, sizeof(ser));
	assert(res != -1);
	listen(sockfd, 5);

	int epfd = epoll_create(5);
	assert(epfd != -1);

	struct epoll_event revents[MAX] = {0};//存放就绪文件描述符的数组
	
	struct epoll_event event;//指定事件
	event.events = EPOLLIN | EPOLLET;  //事件类型
	event.data.fd = sockfd;  //事件所从属的文件描述符

	epoll_ctl(epfd, EPOLL_CTL_ADD, sockfd, &event);


	while(1)
	{
		int n = epoll_wait(epfd, revents, MAX, -1);
		assert(n != -1);
	
		if(n == 0)
		{
			printf("time out\n");
			continue;
		}
    		j = 0;	
	    	printf("%d \n",n);
		for(; j < n; ++j)
		{
			printf("%d\n",j);
			if(revents[j].events & EPOLLIN)
			{
				int fd = revents[j].data.fd;
				int len = sizeof(cli);
		
				if(fd == sockfd)
				{
				int	c = accept(sockfd, (struct sockaddr*)&cli, &len);
					if(c < 0)
					{
						printf("LInk fail\n");
						continue;
					}
					printf("连接文件描述符：%d\n", c);
					event.data.fd = c;
					epoll_ctl(epfd, EPOLL_CTL_ADD, c, &event);
				}
				else
				{
					if(!Add_fds(fd))
					{
						send(fd, "please wait...", strlen("please wait..."), 0);
						close(fd);
						continue;
					}
			    	sem_post(&sem); // 对信号量执行V操作
					
				}
				sleep(3);
			}

		}
	}
}

	
