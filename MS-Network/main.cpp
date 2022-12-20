#include "Socket.h"
#include <iostream>


int main()
{
	Socket s(SERVER);

	s.Bind("127.0.0.1", 1111);
	s.Listen(456);

	std::cout << s.GetType() << '\n';
	auto inf = s.Accept();
	
	s.Close();

	return 0;
}