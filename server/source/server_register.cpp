#include "server_register.h"
#include <WinSock2.h>
#include <config.h>
#include <fstream>

namespace charlie
{
	ServerRegister::ServerRegister()
	{
		read_master_server_address();
	}

	void ServerRegister::read_master_server_address()
	{
		std::ifstream file(config::MASTER_SERVER_FILE);
		if (!file.is_open())
		{
			printf("ServerRegister: Failed to open master server file \n");
			return;
		}

		std::string line;
		DynamicArray<int> address;
		while (std::getline(file, line)) {
			address.push_back(std::stoul(line));
		}

		file.close();

		if (address.size() < 5)
		{
			printf("ServerRegister: Master server file is wrong format \n");
		}

		master_server_ = network::IPAddress((uint8)address[0], (uint8)address[1], (uint8)address[2], (uint8)address[3], (uint16)address[4]);
	}

	void ServerRegister::update(const Time& dt)
	{
		timer_ += dt;

		if (timer_.as_seconds() > update_delay_.as_seconds())
		{
			printf("ServerRegister: Send heartbeat to master server \n");
			register_server();
			timer_ = Time(0.0);
		}
	}

	void ServerRegister::register_server()
	{
		network::NetworkStream stream;
		network::NetworkStreamWriter writer_(stream);

		if (!writer_.serialize(HEARTBEAT))
		{
			printf("request_server write failed! \n");
			return;
		}

		if (!socket_.open())
		{
			printf("MasterServerClient::request_server: Failed to open socket \n");
		}
		if (!socket_.send(master_server_, writer_.stream_.buffer_, writer_.length()))
		{
			printf("request_server send failed! \n");
		}
		socket_.close();
	}
}
