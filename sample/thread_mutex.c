#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <ctype.h>
#include <pthread.h>

#define MAX_THREAD 3
static pthread_mutex_t mutex;
unsigned long long main_counter, counter[MAX_THREAD] = {0};

void *thread_worker(void *arg)
{
	int thread_num = (int)arg;
	printf("thread_id: %lu	counter[%d]\n", pthread_self(), thread_num);
	for (;;)
	{
		pthread_mutex_lock(&mutex);
		counter[thread_num]++;
		main_counter++;
		pthread_mutex_unlock(&mutex);
	}
}

int main(int argc, char *argv[])
{
	int i, rtn, ch;
	
	pthread_mutex_init(&mutex, NULL);

	pthread_t pthread_id[MAX_THREAD] = {0};
	for (i = 0; i < MAX_THREAD; i++) {
		pthread_create(&pthread_id[i], NULL, thread_worker, (void *)i);	
	}

	do {
		unsigned long long sum = 0;
		pthread_mutex_lock(&mutex);
		for (i = 0; i < MAX_THREAD; i++) {
			sum += counter[i];
			printf("No.%d: %llu\n", i, counter[i]);
		}
		sleep(1);
		pthread_mutex_unlock(&mutex);
		printf("%llu/%llu\n", main_counter, sum);
	}while((ch = getchar()) != 'q');

	pthread_mutex_destroy(&mutex);
	return 0;
}
