#pragma once
#include <string>
#include "charlie_network.hpp"
#include <WinSock2.h>

namespace charlie
{
	struct MasterServerClient
	{
		MasterServerClient();
		void read_master_server_address();
		void request_server();
		bool receive_messages();
		network::IPAddress master_server_;
		network::IPAddress server_;
		uint8 buffer_[32];
		int flags_ = 0;

		network::TCPSocket socket_;
		std::string REQUESTSERVER = "1";
	};
}
