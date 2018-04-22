all: sort thread_cond thread_mutex

sort:
	gcc sort.c -o sort

thread_cond:
	gcc thread_cond.c -lpthread -o thread_cond

thread_mutex:
	gcc thread_mutex.c -lpthread -o thread_mutex

clean:
	@rm -f sort thread_cond thread_mutex
