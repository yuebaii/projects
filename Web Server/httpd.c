#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <assert.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <sys/stat.h>
/*
 * TCP
 * socket  bind (80)   listen   accept  recv   send  close
 */

void send_404(int c)
{
	char sendbuff[1024] = {0};
	strcpy(sendbuff, "HTTP/1.1 404 Not Found\r\n");
	strcat(sendbuff, "Server: myhttpd/1.0\r\n");
	strcat(sendbuff, "Content-Length: 14\r\n");
	strcat(sendbuff, "Content-Type: text/html;charset=gbk\r\n");
	strcat(sendbuff, "\r\n");
	strcat(sendbuff, "Page Not Found");

	send(c, sendbuff, strlen(sendbuff), 0);

}
void senderr(int c)
{
	send_404(c);
}
void send_success(int c, int fd)
{
	struct stat st;
	fstat(fd, &st);
	char sendbuff[1024] = {0};
	strcpy(sendbuff, "HTTP/1.1 200 OK\r\n");
	strcat(sendbuff, "Server: myhttpd/1.0\r\n");

	strcat(sendbuff, "Content-Length: ");
	sprintf(sendbuff+strlen(sendbuff), "%d", st.st_size);
	strcat(sendbuff, "\r\n");

	strcat(sendbuff, "Content-Type: text/html;charset=UTF-8\r\n");
	strcat(sendbuff, "\r\n");

	send(c, sendbuff, strlen(sendbuff), 0);

	//发送文件内容
	while(1)
	{
		char buff[10] = {0};
		//int n = read(fd, buff, 9);
		int n = sendfile(c, fd, NULL, st.st_size);
		if(n <= 0)
		{
			break;
		}
		//send(c, buff, n, 0);
	}

	close(fd);	
}

void main()
{
	int sockfd = socket(AF_INET, SOCK_STREAM, 0);
	assert(sockfd != -1);

	struct sockaddr_in ser, cli;
	memset(&ser, 0, sizeof(ser));
	ser.sin_family = AF_INET;
	ser.sin_port = htons(80);
	ser.sin_addr.s_addr = inet_addr("127.0.0.1");
	int res = bind(sockfd, (struct sockaddr*)&ser, sizeof(ser));
	assert(res != -1);

	listen(sockfd, 5);

	while(1)
	{
		int len = sizeof(cli);
		int c = accept(sockfd, (struct sockaddr*)&cli, &len);
		assert(c != -1);
		printf("one client link\n");

		while(1)
		{
			char buff[1024] = {0};
			int n = recv(c, buff, 1023, 0);
			if(n <= 0)
			{
				break;
			}
			//printf("%s\n", buff);
			char *p = strtok(buff, " ");
			if(p == NULL)
			{
				senderr(c);
				continue;
			}
			p = strtok(NULL, " ");
			if(p == NULL)
			{
				senderr(c);
				continue;
			}

			char path[128] = "/var/www/html";
			strcat(path, p);
			int fd = open(path, O_RDONLY);
			if(fd == -1)
			{
				send_404(c);
				continue;
			}
			//成功   发送应答报文段和文件内容
			send_success(c, fd);
		}

		close(c);
		printf("one client unlink\n");
	}
}
