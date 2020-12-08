#include "master_server_client.h"
#include <fstream>
#include <WinSock2.h>

#include "charlie_messages.hpp"
#include "config.h"

namespace charlie
{
	MasterServerClient::MasterServerClient()
	{
		read_master_server_address();
	}

	void MasterServerClient::read_master_server_address()
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

		if (!socket_.open())
		{
			printf("MasterServerClient::request_server: Failed to open socket \n");
		}
	}

	void MasterServerClient::update(const Time& dt)
	{
		if (!is_looking_for_server_)
		{
			return;
		}

		timer_ += dt;

		if (timer_.as_seconds() > update_delay_.as_seconds())
		{
			printf("MasterServerClient::update: Requesting game server from master server \n");
			request_server();
			receive_game_server_address();
			timer_ = Time(0.0);
		}
	}

	void MasterServerClient::request_server()
	{
		network::NetworkStream stream;
		network::NetworkStreamWriter writer_(stream);

		if (!writer_.serialize(REQUESTSERVER))
		{
			printf("MasterServerClient::request_server: write failed! \n");
			return;
		}

		if (!socket_.send(master_server_, writer_.stream_.buffer_, writer_.length()))
		{
			printf("MasterServerClient::request_server: send failed! \n");
		}
	}

	bool MasterServerClient::receive_game_server_address()
	{
		network::NetworkStream stream;

		socket_.receive(master_server_, stream.buffer_, stream.length_);

		network::NetworkStreamReader reader_(stream);

		while (reader_.position() < reader_.length()) {
			if (reader_.peek() == network::NETWORK_MESSAGE_MASTER_SERVER)
			{
				network::NetworkMessageMasterServer packet;
				if (!packet.read(reader_)) {
					printf("Data packet read failed! \n");
					return false;
				}
				game_server_ = network::IPAddress(packet.a_, packet.b_, packet.c_, packet.d_, config::PORT);
				is_looking_for_server_ = false;
				return true;
			}
			return false;
		}
		return false;
	}
}
