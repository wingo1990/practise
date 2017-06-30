#include<stdlib.h>
#include<unistd.h>
#include<stdio.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<sys/socket.h>
#include<errno.h>
#include<assert.h>
#include<string.h>

int main() 
{
	// init the first element of the array, 
	// and then the compiler will init with zero for other elements.
	int fd[2] = {0};
	
	printf("%d %d\n", fd[0], fd[1]);
	int ret = socketpair(AF_LOCAL, SOCK_STREAM, 0, fd);
	assert(ret == 0);

	ret = fork();
	assert(ret >= 0);

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
	
	char *ss = "hello baby!!!!!!!!!!!!!!!!!!!!!!!!!!!!";
	int fd_echo = open("echo.txt", O_CREAT|O_APPEND|O_RDWR, 0777);
	assert(fd_echo >= 0);
	int fd_test = open("test.txt", O_CREAT|O_APPEND|O_RDWR, 0777);
	assert(fd_test >= 0);
	printf("fd_echo:%d, fd_test:%d\n", fd_echo, fd_test);

	int myfds[] = {fd_echo, fd_test};
	struct msghdr msg;
	msg.msg_name = NULL;
	msg.msg_namelen = 0;

	struct iovec io_vec[1];
	io_vec[0].iov_base = ss;
	io_vec[0].iov_len = strlen(ss);
	msg.msg_iov = io_vec;
	msg.msg_iovlen = 1;

	char cmsg_buf[CMSG_SPACE(sizeof myfds)];
	msg.msg_control = cmsg_buf;
	msg.msg_controllen = sizeof cmsg_buf;
	struct cmsghdr *cmsg = CMSG_FIRSTHDR(&msg);
	cmsg->cmsg_level = SOL_SOCKET;
	cmsg->cmsg_type = SCM_RIGHTS;
	cmsg->cmsg_len = CMSG_LEN(sizeof myfds);
	int *pfd = (int *)CMSG_DATA(cmsg);
	memcpy(pfd, myfds, sizeof myfds);

	ret = sendmsg(fd[0], &msg, 0);
	printf("sendmsg ret %d\n", ret);
	return 0;	
}
