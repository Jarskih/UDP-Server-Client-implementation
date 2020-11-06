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
		void register_server() const;
		Time timer_;
		Time update_delay_;
		network::IPAddress master_server_;
		std::string HEARTBEAT = "0";
	};
}
