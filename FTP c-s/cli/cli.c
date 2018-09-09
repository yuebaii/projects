#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <fcntl.h>

#define LEN 128

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

void main()
{
	int sockfd = socket(AF_INET, SOCK_STREAM, 0);
	assert(sockfd != -1);

	struct sockaddr_in ser, cli;
	memset(&ser, 0, sizeof(ser));
	ser.sin_family = PF_INET;
	ser.sin_port = htons(6500);
	ser.sin_addr.s_addr = inet_addr("192.168.52.129");


	int res = connect(sockfd, (struct sockaddr*)&ser, sizeof(ser));
	assert(res != -1);

	while(1)
	{
		printf("------------please choose:-------------\n"); 
		printf("------ls  up  down  cd   pwd  end------\n");
		printf("---------------------------------------\n");
		printf("please input: ");
		fflush(stdout);
		char buff[LEN] = {0};
		fgets(buff, LEN, stdin);
		buff[strlen(buff) - 1] = 0;

		if(strncmp(buff, "end", 3) == 0)
		{
			close(sockfd);
			break;
		}
		else if(strncmp(buff, "ls", 2) == 0 ||
				strncmp(buff, "cd", 2) == 0 ||
				strncmp(buff, "pwd", 3) == 0)
		{
			Data val;
			val.option = CMD;
			strcpy(val.data, buff);
			send(sockfd, &val, strlen(buff)+ sizeof(Op), 0);

			while(1)
			{
				char recvbuff[LEN] = {0};
				int n = recv(sockfd, recvbuff, LEN-1, 0);
				if(strncmp(recvbuff+strlen(recvbuff)-3, "###", 3) == 0)
				{
					recvbuff[strlen(recvbuff)-1] = 0;
					recvbuff[strlen(recvbuff)-1] = 0;
					recvbuff[strlen(recvbuff)-1] = 0;
					printf("%s", recvbuff);
					break;
				}

				printf("%s", recvbuff);
			}
		}
		else if(strncmp(buff, "up", 2) == 0)
		{
			
			char *p = strtok(buff, " ");
			p = strtok(NULL, " ");
			if(p == NULL)
			{
				printf("please input filename!\n");
				continue;
			}
			
			Data val;
			val.option = UP;
			strcpy(val.data, p);
			send(sockfd, &val, sizeof(Op)+strlen(val.data), 0);//发送up filename给服务器
			printf("uploading...\n");	
			int fd = open(p, O_RDONLY);

			if(fd == -1)
			{
				printf("local open file fail\n");
				continue;
			}

			struct stat st;
			fstat(fd, &st);
			char buff[128] = "OK#";
			sprintf(buff+3, "%d", st.st_size);
			strcat(buff, "#");
			send(sockfd, buff, strlen(buff), 0);//每次给服务器发送数据前先发送"OK#st_size#".
			while(1)
			{
				memset(buff, 0, 128);
				int n = read(fd, buff, 127);
				if(n == 0)
				{
					close(fd);
					break;
				}

				send(sockfd, buff, n, 0);
			}

		}
		else if(strncmp(buff, "down", 4) == 0)
		{
			char *p = strtok(buff," ");
			p = strtok(NULL," ");
			if(p == NULL)
			{
				printf("please input filename\n");
				continue;
			}

			int fd = open(p,O_WRONLY | O_CREAT , 0666);//O_TRNUC打开的同时清空
			if(fd == -1)
			{
				printf("local open file fail\n");
				continue;
			}

			Data val;
			val.option = DOWN;
			strcpy(val.data,p);
			send(sockfd,&val,strlen(val.data)+sizeof(Op),0);

			char recvbuff[LEN] = {0};
			recv(sockfd,recvbuff,LEN-1,0);//ER   OK#1000#this is

			if(strncmp(recvbuff,"ER",2) == 0)
			{
				printf("server open file fail\n");
				continue;
			}

			p = strtok(recvbuff,"#");
			p = strtok(NULL,"#");

			int size = 0,count = 0;
			sscanf(p,"%d",&size);
			if(p != NULL)
			{
				p = strtok(NULL,"\0");
			}
			if(p != NULL)
			{
				int count = strlen(p);
				write(fd,p,count);
			}

			printf("\033[?25l");//隐藏光标
			while(1)
			{
				printf("%.2f %%", count*1.0/size*100);
				fflush(stdout);
				printf("\033[100D\033[K");//清除光标到行尾的内容
				memset(recvbuff, 0, LEN);
				printf("b\n");
				int n = recv(sockfd,recvbuff,LEN-1,0);
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

		else
		{
			printf("conmmand error,please input again\n");
		}
	}
}
