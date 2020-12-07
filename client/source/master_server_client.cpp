#include "master_server_client.h"
#include <fstream>
#include "charlie_messages.hpp"
#include "config.h"

namespace charlie
{
	MasterServerClient::MasterServerClient() : buffer_()
	{
		read_master_server_address();
	}

	void MasterServerClient::read_master_server_address()
	{
		std::ifstream file(config::MASTER_SERVER_FILE.c_str());
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

	void MasterServerClient::request_server() const
	{
		const SOCKET client = socket(AF_INET, SOCK_STREAM, 0);

		const int result = send(client, REQUESTSERVER.c_str(), (int)REQUESTSERVER.size(), 0);

		if (result == -1)
		{
			printf("ServerRegister: Failed to send register request to master server \n");
		}
		closesocket(socket_.id_);
	}

	bool MasterServerClient::receive_messages()
	{
		network::NetworkStream stream;

		const SOCKET client = socket(AF_INET, SOCK_STREAM, 0);

		if (recvfrom(client, (char*)stream.buffer_, stream.length_, flags_, (sockaddr*)nullptr, nullptr))
		{
			network::NetworkStreamReader reader_(stream);

			while (reader_.position() < reader_.length()) {
				network::NetworkMessageMasterServer packet;
				if (!packet.read(reader_)) {
					printf("data packet read failed! \n");
					return false;
				}
				server_ = network::IPAddress(packet.a_, packet.b_, packet.c_, packet.d_, config::PORT);
				closesocket(socket_.id_);
				return true;
			}
		}
		return false;
	}
}
