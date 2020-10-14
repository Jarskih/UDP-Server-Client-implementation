#include "charlie_networkinfo.hpp"

#include "charlie_network.hpp"

namespace charlie {
	Networkinfo::Networkinfo():
		rtt_min_(100.0)
		, rtt_total_(0)
		, rtt_avg_(0)
		, packets_sent_(0)
		, packets_recv_(0)
		, packets_sent_avg_(0)
		, packets_recv_avg_(0)
		, bytes_recv_(0)
		, bytes_sent_(0)
		, kibibytes_recv_avg_(0)
		, kibibytes_sent_avg_(0)
		, input_misprediction_(0)
	{
	}

	void Networkinfo::update(Time dt, const network::Connection connection)
	{
		rtt_counter_ += dt;
	
		if(connection.round_trip_time() > Time(0.0))
		{
			if(connection.round_trip_time() > rtt_max_)
			{
				rtt_max_ = connection.round_trip_time();
			}
			if(connection.round_trip_time() < rtt_min_)
			{
				rtt_min_ = connection.round_trip_time();
			}
		}

		if(rtt_counter_ < Time(1.0))
		{
			rtt_sum_ += connection.round_trip_time();
			rtt_total_++;
			return;
		}
			
		if(rtt_total_ > 0)
		{
			rtt_avg_ = rtt_sum_.as_milliseconds() / (float)rtt_total_;
			kibibytes_recv_avg_ = bytes_recv_/(float)1024;
			kibibytes_sent_avg_ = bytes_sent_/(float)1024;
		}
		rtt_total_ = 0;
		rtt_sum_ = Time(0.0);
		rtt_counter_ = Time(0.0);
		bytes_recv_ = 0;
		bytes_sent_ = 0;
	}

	void Networkinfo::render(Renderer renderer, const network::Connection connection)
	{
		
		char buffer[30] = "";
		sprintf_s(buffer, "latency: %ld ms", (long)connection.latency().as_milliseconds());
		renderer.render_text({ 100, 2 }, Color::White, 1, buffer);

		sprintf_s(buffer, "input corr: %i", input_misprediction_);
		renderer.render_text({ 300, 2 }, Color::White, 1, buffer);

		sprintf_s(buffer, "rtt: %ld ms", (long)connection.round_trip_time().as_milliseconds());
		renderer.render_text({2, 12}, Color::Yellow, 1, buffer);

		sprintf_s(buffer, "max: %ld ms", (long)rtt_max_.as_milliseconds());
		renderer.render_text({100, 12}, Color::Yellow, 1, buffer);

		sprintf_s(buffer, "min: %ld ms", (long)rtt_min_.as_milliseconds());
		renderer.render_text({200, 12}, Color::Yellow, 1, buffer);	

		sprintf_s(buffer, "avg: %ld ms", (long)rtt_avg_);
		renderer.render_text({300, 12}, Color::Yellow, 1, buffer);

		sprintf_s(buffer, "in: %f KiB", (float)kibibytes_recv_avg_);
		renderer.render_text({2, 24}, Color::Magenta, 1, buffer);

		sprintf_s(buffer, "out: %f KiB", (float)kibibytes_sent_avg_);
		renderer.render_text({200, 24}, Color::Magenta, 1, buffer);

		sprintf_s(buffer, "in: %i", packets_recv_);
		renderer.render_text({350, 24}, Color::Magenta, 1, buffer);
		
		sprintf_s(buffer, "out: %i", packets_sent_);
		renderer.render_text({450, 24}, Color::Magenta, 1, buffer);

	}

	void Networkinfo::packet_received(int32 size)
	{
		packets_recv_++;
		bytes_recv_ += size;
	}

	void Networkinfo::packet_sent(int32 size)
	{
		packets_sent_++;
		bytes_sent_ += size;
	}
}
