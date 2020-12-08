#pragma once
#include <string>
#include "charlie_network.hpp"

namespace charlie
{
	struct MasterServerClient
	{
		MasterServerClient();
		void read_master_server_address();
		void update(const Time& dt);
		void request_server();
		bool receive_game_server_address();
		network::IPAddress master_server_;
		network::IPAddress game_server_;
		network::UDPSocket socket_;
		uint8 REQUESTSERVER = 2;
		bool is_looking_for_server_ = true;
		Time timer_ = Time(5.0);
		Time update_delay_ = Time(5.0);
	};
}
