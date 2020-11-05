#include "server_register.h"
#include <WinSock2.h>
#include <config.h>
#include <fstream>

namespace charlie
{
	ServerRegister::ServerRegister()
	{
		update_delay_ = Time(5.0);
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

	void ServerRegister::update(const charlie::Time& dt)
	{
		timer_ += dt;

		if (timer_.as_seconds() > 5)
		{
			printf("ServerRegister: Send heartbeat to master server \n");
			register_server("register");
			timer_ = Time(0.0);
		}
	}

	void ServerRegister::register_server(const std::string& message) const
	{
		sockaddr_in local = {};
		local.sin_family = AF_INET;
		local.sin_port = htons(master_server_.port_);
		local.sin_addr.s_addr = htonl(master_server_.host_);

		const SOCKET client = socket(AF_INET, SOCK_DGRAM, 0);

		const int result = sendto(client, message.c_str(), (int)message.size(), 0, (const sockaddr*)&local, sizeof(SOCKADDR));

		closesocket(client);

		if (result == -1)
		{
			printf("ServerRegister: Failed to send register request to master server v");
		}
	}
}
