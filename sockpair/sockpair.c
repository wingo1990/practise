#include<stdlib.h>
#include<unistd.h>
#include<stdio.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<errno.h>
#include<assert.h>

int main() 
{
	int fd[2] = {-1, -1};
	
	int ret = socketpair(AF_LOCAL, SOCK_STREAM, 0, fd);
	if (ret < 0) {
		printf("socketpair error: %d\n", errno);
		return -1;
	}

	ret = fork();
	if (ret < 0) {
		printf("socketpair error: %d\n", errno);
		return -1;
	}

	// child process
	if (ret == 0) {
		close(fd[0]);
		if (dup2(fd[1], STDIN_FILENO) == -1) {
			printf("dup2 IN error: %d\n", errno);
			return -1;
		}
	
		if (dup2(fd[1], STDOUT_FILENO) == -1) {
			printf("dup2 OUT error: %d\n", errno);
			return -1;
		}

		char *arg[] = {NULL};
		execv("./echo", arg);
		perror("child execv");
		return -1;
	}
	
	// father process
	close(fd[1]);
	char buf[1024] = {0};
	int i = 0;
	while(1) {
		int len = snprintf(buf, 1024, "hello world %d", i);
		int size = write(fd[0], buf, len + 1);
		assert(size == (len+1));

		size = (int)read(fd[0], buf, 1024);
		assert(size <= 1024);

		printf("echo: %s\n", buf);
		i++;

		sleep(1);
	}
}
