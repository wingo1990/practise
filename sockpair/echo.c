#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <assert.h>

int main()
{
	while (1) {
		char buf[1024] = {0};
		int size = read(STDIN_FILENO, buf, sizeof(buf));
		
		size = write(STDOUT_FILENO, buf, strlen(buf) + 1);
		assert(size == (strlen(buf) + 1));
	}

	return 0;
}
