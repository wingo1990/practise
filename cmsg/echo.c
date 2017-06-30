#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <assert.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <errno.h>

int main()
{
	char iov_buff1[64] = {0};
	char iov_buff2[64] = {0};
	int myfds[2] = {-1, -1};

	struct msghdr msg;
	memset(&msg, 0, sizeof msg);
	msg.msg_name = NULL;
	msg.msg_namelen = 0;

	// msg vec
	struct iovec io_vec[2];
	io_vec[0].iov_base = iov_buff1;
	io_vec[0].iov_len = 5;
	io_vec[1].iov_base = iov_buff2;
	io_vec[1].iov_len = 64;
	msg.msg_iov = io_vec;
	msg.msg_iovlen = sizeof(io_vec)/sizeof(io_vec[0]);
	
	// msg control
	char buf[CMSG_SPACE(sizeof myfds)];
	msg.msg_control = buf;
	msg.msg_controllen = sizeof buf;
	fprintf(stderr, "msg_len:%lu\n", sizeof buf);

	// start read
	int size = recvmsg(STDIN_FILENO, &msg, MSG_DONTWAIT);
	if (size < 0) {
		fprintf(stderr,"recv msg: %d, error: (%d)%s\n", size, errno, strerror(errno));
		// for the connected pipe, the name must be setted.
		fprintf(stderr,"name: %p, len: %d\n", msg.msg_name, msg.msg_namelen);
	}
	
	fprintf(stderr,"recv iov1: %s\n", iov_buff1);
	fprintf(stderr,"recv iov2: %s\n", iov_buff2);

	struct cmsghdr *cmsg = CMSG_FIRSTHDR(&msg);
	
	if (cmsg->cmsg_level == SOL_SOCKET &&
		cmsg->cmsg_type == SCM_RIGHTS) {
		int *pfd = (int *)CMSG_DATA(cmsg);
		memcpy(myfds, pfd, sizeof myfds);
	} else {
		fprintf(stderr, "Not support this cmsg type.");
		return -1;
	}
	
	// the file descriptor tranferred will be changed...
	fprintf(stderr, "fd: %d %d\n", myfds[0], myfds[1]);
	write(myfds[0], iov_buff1, strlen(iov_buff1));
	write(myfds[1], iov_buff2, strlen(iov_buff2));

	return 0;
}
