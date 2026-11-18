#include "Server.h"

Server::Server(std::shared_ptr<Board> _board) : board(_board)
{

}
Server::~Server(){}

void Server::init()
{
	if (listener == nullptr)
	{
		listener = std::make_unique<sf::TcpListener>();

	}
	if (listener->listen(53000) != sf::Socket::Status::Done)
	{
		std::cout << "Couldnt bind listener to port 53000" << std::endl;

	}
}

void Server::run()
{
	std::thread input_thread([&] {input(); });
	while (running)
	{
		sf::TcpSocket& c_sock = *connections.emplace_back(std::make_unique<sf::TcpSocket>());

		if (listener->accept(c_sock) != sf::Socket::Status::Done)
		{
			connections.pop_back();
			
			std::cout << "connection failed on server side" << std::endl;
			continue;
		}
		workers.emplace_back([&] {
			listen(c_sock);
			std::cout << "Disconected detected" << std::endl;
			std::lock_guard<std::mutex> lck(mutex);
			for (int i = 0; i < connections.size();i++)
			{
				if ((connections[i])->getRemotePort() == c_sock.getRemotePort())
				{
					connections.erase(std::next(connections.begin(), i));
					break;
				}
			}
			});

		std::cout << "client conected @"<< c_sock.getRemotePort() << std::endl;
	}
}

void Server::input()
{
	while (running)
	{
		std::string str;
		std::getline(std::cin, str);
		std::cin.clear();
		std::lock_guard<std::mutex> lck(mutex);
		sf::Packet packet = packData(str);
		for (std::unique_ptr<sf::TcpSocket>& connection : connections)
		{
			connection->send(packet);
			//i_socket.send(packet);
		}
	}
}

void Server::listen(sf::TcpSocket& c_socket)
{
	bool continue_receiving = true;
	//char data[1028];
	//std::size_t received;
	sf::Packet packet;
	while (continue_receiving)
	{
		sf::Socket::Status status = c_socket.receive(packet);
		if (status == sf::Socket::Status::Disconnected)
		{
			continue_receiving = false;
			std::cout << "disconect" << std::endl;
		}
		else
		{
			unpackData(packet, string, origin, destination);
		}
		//if (received < 1028) { data[received] = '\0'; }
		//std::cout << reinterpret_cast<char*>(data) << std::endl;

	}
	c_socket.disconnect();
}
void Server::relay(std::string buffer, sf::TcpSocket& c_socket)
{
	char* message = reinterpret_cast<char*>(buffer.data());
	std::lock_guard<std::mutex> lck(mutex);
	for (std::unique_ptr<sf::TcpSocket>& connection : connections)
	{
		if (connection->getRemotePort() != c_socket.getRemotePort())
		{
			connection->send(message, buffer.size());
		}
		if (string.length() > 0)
		{
			std::cout << string << std::endl;
		}

		if (origin.x >= 0 && destination.x >= 0)
		{
			std::lock_guard<std::mutex> lck(board_mutex);
			board->moveByCoords(origin, destination);
		}
	}
}

sf::Packet Server::packData(std::string _string, sf::Vector2f _origin, sf::Vector2f _destination) const
{
	sf::Packet packet;
	packet << _string << _origin.x << _origin.y << _destination.x << _destination.y;
	return packet;
}
void Server::unpackData(sf::Packet _packet, std::string _string, sf::Vector2f _origin, sf::Vector2f _destination)
{
	_packet >> _string >> _origin.x >> _origin.y >> _destination.x >> _destination.y;
	std::cout << "Reveived message: " << _string << std::endl;
}

void Server::send_move(sf::Packet packet)
{
	bool continue_receiving = true;
	while (continue_receiving)
	{
		for (std::unique_ptr<sf::TcpSocket>& connection : connections)
		{
			if (connection)
			{
				connection->send(packet);
			}
		}
	}
}