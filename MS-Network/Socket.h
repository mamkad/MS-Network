#pragma once

#include <WinSock2.h>
#include <vector>
#include <string>

enum SocketType { CLIENT = 0, SERVER = 1, NOTASSIGN = 2 };

class Socket final
{
	SOCKET socketFd = 0;
	SocketType socketType = NOTASSIGN;
	bool isActive = false;

public:
	Socket(SocketType mode);
	Socket(Socket const&);
	Socket(Socket&&) = delete;
	Socket& operator = (Socket const&) = delete;
	Socket& operator = (Socket&&) = delete;
	~Socket();

	void Bind(std::string const& ip, int port);
	void Listen(int maxConnectionCount);
	void Connect(std::string const& ip, int port);
	std::pair<Socket, sockaddr_in> Accept();

	int Recv(Socket const& socket, std::vector<char>& buff);
	int Send(Socket const& socket, std::vector<char> const& buff);

	void Close();

	bool IsActive()      const noexcept;
	SocketType GetType() const noexcept;

private:
	Socket(SOCKET socketFd);
	void initAddress(sockaddr_in& address, std::string const& ip, int port);
};