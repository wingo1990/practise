#include <iostream>
#include <thread>

class callable {
// **note this public that is import
public:
	// this const keyword say that this function cannot modify any member parameters.
	void operator()() const {
		std::cout << "hello, this is callable object." << std::endl;	
	}
};


int main () {
	
	// 1. common init
	callable ca;
	std::thread tt(ca);
	tt.join();

	// cannot define a thread object like this.
	// the arg is temp variable, the following statement will be a function declaration.
	// * std::thread thread_func(callable());

	// 2. initialization syntax {}.
	std::thread t1{callable()};
	t1.join();
	
	// 3. lambda expression
	std::thread t2([]{
		std::cout << "hello this lambda init.." << std::endl;
	});
	// if no this statement, the program will be core.
	t2.join();
		
	return 0;
}
