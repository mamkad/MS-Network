#include "Socket.h"

#include <WS2tcpip.h>
#include <stdexcept>
#pragma comment(lib, "Ws2_32.lib")


Socket::Socket(SocketType mode)
{
	WSADATA socketVersions;

	int startupReturnCode = WSAStartup(MAKEWORD(2, 2), &socketVersions);

	if (startupReturnCode != 0) {
		throw std::logic_error("Socket(): WSAStartup() error: " + WSAGetLastError());
	}

	socketFd = socket(AF_INET, SOCK_STREAM, 0);

	if (socketFd == INVALID_SOCKET) {
		closesocket(socketFd);
		WSACleanup();
		throw std::logic_error("Socket(): socket() error: " + WSAGetLastError());
	}

	isActive = true;
	socketType = mode;
}

Socket::Socket(Socket const& sct) : socketFd(sct.socketFd), isActive(sct.isActive)
{
}

Socket::~Socket()
{
	Close();
}

void Socket::Bind(std::string const& ip, int port)
{
	if (socketType != SERVER) {
		throw std::invalid_argument("Bind(): socket must have type SERVER to use Bind() function. Current type: " + socketType);
	}

	sockaddr_in address;
	initAddress(address, ip, port);

	int bindReturnCode = bind(socketFd, (sockaddr*)&address, sizeof(address));

	if (bindReturnCode != 0) {
		throw std::logic_error("Bind(): bind() error: " + WSAGetLastError());
	}
}

void Socket::Listen(int maxConnectionCount)
{
	if (socketType != SERVER) {
		throw std::invalid_argument("Listen(): socket must have type SERVER to use Listen() function. Current type: " + socketType);
	}

	int listenReturnCode = listen(socketFd, SOMAXCONN_HINT(maxConnectionCount));

	if (listenReturnCode != 0) {
		throw std::logic_error("Listen(): listen() error: " + WSAGetLastError());
	}
}

void Socket::Connect(std::string const& ip, int port)
{
	if (socketType != CLIENT) {
		throw std::invalid_argument("Connect(): socket must have type CLIENT to use Connect() function. Current type: " + socketType);
	}

	sockaddr_in address;
	initAddress(address, ip, port);

	int connectReturnCode = connect(socketFd, (sockaddr*)&address, sizeof(address));

	if (connectReturnCode != 0) {
		throw std::logic_error("Connect(): connect() error: " + WSAGetLastError());
	}
}

std::pair<Socket, sockaddr_in> Socket::Accept()
{
	sockaddr_in clientAddress;
	int clientAddressSize = sizeof(clientAddress);

	ZeroMemory(&clientAddress, clientAddressSize);

	SOCKET clientFd = accept(socketFd, (sockaddr*)&clientAddress, &clientAddressSize);

	if (clientFd == INVALID_SOCKET) {
		closesocket(clientFd);
		throw std::logic_error("Accept(): accept() error: " + WSAGetLastError());
	}

	return std::make_pair(Socket(clientFd), clientAddress);
}

int Socket::Recv(Socket const& socket, std::vector<char>& buff)
{
	return recv(socket.socketFd, buff.data(), buff.size(), 0);
}

int Socket::Send(Socket const& socket, std::vector<char> const& buff)
{
	return send(socket.socketFd, buff.data(), buff.size(), 0);
}

void Socket::Close()
{
	if (IsActive()) {
		closesocket(socketFd);
		WSACleanup();

		isActive = false;
	}
}

bool Socket::IsActive() const noexcept
{
	return isActive;
}

SocketType Socket::GetType() const noexcept
{
	return socketType;
}

Socket::Socket(SOCKET socketFd)
{
	this->socketFd = socketFd;

	isActive = true;
}

void Socket::initAddress(sockaddr_in& address, std::string const& ip, int port)
{
	in_addr ipStruct{};
	int inetptoneReturnCode = inet_pton(AF_INET, ip.data(), &ipStruct);

	if (inetptoneReturnCode <= 0) {
		throw std::invalid_argument("Bind(): inet_pton() error. Wrong ip address");
	}

	ZeroMemory(&address, sizeof(address));

	address.sin_family = AF_INET;
	address.sin_addr = ipStruct;
	address.sin_port = htons(port);
}