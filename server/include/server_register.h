#pragma once

#include <string>
#include "charlie_network.hpp"

namespace charlie {
	struct Time;

	struct ServerRegister
	{
		ServerRegister();
		void read_master_server_address();
		void update(const charlie::Time& dt);
		void register_server();
		network::IPAddress master_server_;
		network::UDPSocket socket_;
		uint8  HEARTBEAT = 1;
		Time timer_ = Time(5.0);
		Time update_delay_ = Time(5.0);
	};
}
