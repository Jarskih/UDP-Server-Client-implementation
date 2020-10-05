#pragma once
#include "charlie.hpp"

namespace charlie
{
	namespace network {
		struct Connection;
	}

	struct Networkinfo
	{
		Networkinfo();

		void update(Time dt, network::Connection connection);
		void render(Renderer renderer, network::Connection connection);
		void packet_received(int32 size);
		void packet_sent(int32 size);
		Time rtt_;
		Time rtt_max_;
		Time rtt_min_;
		Time rtt_sum_;
		Time rtt_counter_;
		uint32 rtt_total_;
		float rtt_avg_;

		uint32 packets_sent_;
		uint32 packets_recv_;
		uint32 packets_sent_avg_;
		uint32 packets_recv_avg_;
		
		float bytes_recv_;
		float bytes_sent_;
		float kibibytes_recv_avg_;
		float kibibytes_sent_avg_;

		uint32 input_misprediction_;
	};
}
