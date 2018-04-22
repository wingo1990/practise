#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define MSG_LEN_MAX 31

typedef struct tagNode 
{
	char szMsg[MSG_LEN_MAX+1];
	struct tagNode *next;
}NODE_S;


typedef struct tagList
{
	pthread_mutex_t mutex;
	pthread_cond_t cond;
	NODE_S *head;
}LIST_S;

static LIST_S g_MsgList = {0};

void list_init(LIST_S *list) 
{
	list->head = NULL;
	pthread_mutex_init(&list->mutex, NULL);
	pthread_cond_init(&list->cond, NULL);
}

void *dealmsg_thread(void *data)
{
	int count = 0;

	while (1) {
		pthread_mutex_lock(&g_MsgList.mutex);
		while (g_MsgList.head == NULL) {
			printf("can not find msg, wait....\n");
			pthread_cond_wait(&g_MsgList.cond, &g_MsgList.mutex);
			printf("got wake up msg, start work...\n");
		}
	
		NODE_S *node = g_MsgList.head;
		g_MsgList.head = g_MsgList.head->next;
		printf("got msg: %s\n", node->szMsg);
		free(node);
		count++;
		
		pthread_mutex_unlock(&g_MsgList.mutex);
	}
	
	return NULL;
}

void receive_msg(char *msg, int msglen)
{
	NODE_S *node = (NODE_S *)malloc(sizeof(NODE_S));
	snprintf(node->szMsg, MSG_LEN_MAX, "%s", msg);
	
	pthread_mutex_lock(&g_MsgList.mutex);
	node->next = g_MsgList.head;
	g_MsgList.head = node;
	pthread_mutex_unlock(&g_MsgList.mutex);
	
	printf("send msg: %s\n", msg);
	pthread_cond_signal(&g_MsgList.cond);

	return;
}


int main()
{
	pthread_t iThreadId;
	void *ret = NULL;
	char szMsg[MSG_LEN_MAX + 1];

	list_init(&g_MsgList);
	pthread_create(&iThreadId, NULL, dealmsg_thread, NULL);

	for (int i=0; i < 100; i++) {
		sprintf(szMsg, "%d: hello", i);
		receive_msg(szMsg, strlen(szMsg));
	}

	//pthread_detach(iThreadId);
	pthread_join(iThreadId, &ret);	

	printf("main exit.\n");
	return 0;
}
