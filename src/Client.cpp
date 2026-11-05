#include "Client.h"

void Client::connect()
{
	if (socket == nullptr)
	{
		socket = std::make_unique<sf::TcpSocket>();

	}
	if (socket->conect(*sf::IpAddress::fromString("127.0.0.1"), 53000) == sf::Socket::Status::Done)
	{
		std::cout << "Connected" << std::endl;
		connect = true;
	}
	else std::cout << "failed to conect to 127.0.0.1::53000" << std::endl;
}