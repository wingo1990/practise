#include <stdio.h>
#include <signal.h>
#include <execinfo.h>
#include <stdlib.h>
#include <unistd.h>

#define SEGV_SIZE (20)

void sigsegv_handler(int sig)
{
	if (sig != SIGSEGV) {
		printf("signal is not SIGSEV: %d", sig);
		return;
	}
	
	char *buf[SEGV_SIZE];
	int size = backtrace((void **)buf, SEGV_SIZE);
	char **lines = backtrace_symbols((void *const *)buf, size);
	if (lines == NULL) {
		printf("backtrace_symbols error.");
		return;
	}
	for (int i = 0; i < size; i++) {
		printf("%s\n", lines[i]);
	}
	return ;
}



int myfunc(int ncalls) {
	if (ncalls == 1) {
		while (1) {sleep(1);}
	}

	myfunc(ncalls - 1);
}

int main(int argc, char *argv[])
{
	signal(SIGSEGV, sigsegv_handler);
	
	if (argc != 2) {
		fprintf(stderr, "%s num-calls\n", argv[0]);
		return EXIT_FAILURE;
	}
	
	myfunc(atoi(argv[1]));
	return 	EXIT_SUCCESS;

}
