#include <iostream>
#include <thread>

void hello()
{
	std::cout << "hello concurrent world!" << std::endl;
}

int main()
{
	std::thread t(hello);

	while (1) {}
	t.join();
	return 0;
}
