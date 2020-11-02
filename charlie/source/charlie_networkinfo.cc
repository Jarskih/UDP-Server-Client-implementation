#include "charlie_networkinfo.hpp"

#include "charlie_network.hpp"
#include "sdl_application.hpp"

namespace charlie {
	Networkinfo::Networkinfo() :
		text_(nullptr)
		, rect_()
		, rtt_min_(100.0)
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

		if (connection.round_trip_time() > Time(0.0))
		{
			if (connection.round_trip_time() > rtt_max_)
			{
				rtt_max_ = connection.round_trip_time();
			}
			if (connection.round_trip_time() < rtt_min_)
			{
				rtt_min_ = connection.round_trip_time();
			}
		}

		if (rtt_counter_ < Time(1.0))
		{
			rtt_sum_ += connection.round_trip_time();
			rtt_total_++;
			return;
		}

		if (rtt_total_ > 0)
		{
			rtt_avg_ = rtt_sum_.as_milliseconds() / (float)rtt_total_;
			kibibytes_recv_avg_ = bytes_recv_ / (float)1024;
			kibibytes_sent_avg_ = bytes_sent_ / (float)1024;
		}
		rtt_total_ = 0;
		rtt_sum_ = Time(0.0);
		rtt_counter_ = Time(0.0);
		bytes_recv_ = 0;
		bytes_sent_ = 0;
	}

	void Networkinfo::render(SDL_Renderer* renderer, const network::Connection connection, const TextHandler& text_handler)
	{
		{
			const std::string string = "latency: " + std::to_string((int)connection.latency().as_milliseconds()) + " ms";
			render_text(renderer, connection, text_handler, string, Color::white, 0, 0);
		}
		{
			const std::string string = "input corr: " + std::to_string((int)input_misprediction_);
			render_text(renderer, connection, text_handler, string, Color::white, 200, 0);
		}
		{
			const std::string string = "rtt: " + std::to_string((int)connection.round_trip_time_.as_milliseconds()) + " ms";
			render_text(renderer, connection, text_handler, string, Color::white, 0, 20);
		}
		{
			const std::string string = "max: " + std::to_string((int)rtt_max_.as_milliseconds()) + " ms";
			render_text(renderer, connection, text_handler, string, Color::white, 125, 20);
		}
		{
			const std::string string = "min: " + std::to_string((int)rtt_min_.as_milliseconds()) + " ms";
			render_text(renderer, connection, text_handler, string, Color::white, 250, 20);
		}
		{
			const std::string string = "avg: " + std::to_string((int)rtt_avg_) + " ms";
			render_text(renderer, connection, text_handler, string, Color::white, 375, 20);
		}
		{
			const std::string string = "in: " + std::to_string(kibibytes_recv_avg_) + " KiB";
			render_text(renderer, connection, text_handler, string, Color::white, 0, 40);
		}
		{
			const std::string string = "out: " + std::to_string(kibibytes_sent_avg_) + " KiB";
			render_text(renderer, connection, text_handler, string, Color::white, 200, 40);
		}
		{
			const std::string string = "p in: " + std::to_string(packets_recv_);
			render_text(renderer, connection, text_handler, string, Color::white, 0, 60);
		}
		{
			const std::string string = "p out: " + std::to_string(packets_sent_);
			render_text(renderer, connection, text_handler, string, Color::white, 200, 60);
		}
	}

	void Networkinfo::render_text(SDL_Renderer* renderer, const network::Connection connection, const TextHandler& text_handler, const std::string& text,
		SDL_Color color, int x, int y)
	{
		rect_ = { x, y, static_cast<int>(text.length()) * 10, 15 };
		SDLSprite* sprite = text_handler.CreateText(text, color, rect_.x, rect_.y, rect_.w, rect_.h);
		SDL_RenderCopy(renderer, sprite->get_texture(), nullptr, &rect_);
		sprite->destroy();
	}

	void Networkinfo::packet_received(int32 size)
	{
		packets_recv_++;
		bytes_recv_ += static_cast<float>(size);
	}

	void Networkinfo::packet_sent(int32 size)
	{
		packets_sent_++;
		bytes_sent_ += static_cast<float>(size);
	}
}
