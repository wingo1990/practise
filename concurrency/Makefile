all: hello_world thread_construct
	

hello_world: hello_world.cpp
	g++ -std=c++11 hello_world.cpp -lpthread -o hello_world

thread_construct: thread_construct.cpp
	g++ -std=c++11 thread_construct.cpp -lpthread -o thread_construct 

clean:
	@rm -f hello_world	thread_construct
