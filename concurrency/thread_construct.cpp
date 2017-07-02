#include <iostream>
#include <thread>

class callable {
public:
	// this const keyword say that this function cannot modify any member parameters.
	void operator()() const {
		std::cout << "hello, this is callable object." << std::endl;	
	}
};


int main () {
	callable ca;
	std::thread tt(ca);
	tt.join();
	return 0;
}
