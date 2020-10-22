#include "charlie_networkinfo.hpp"

#include "charlie_network.hpp"
#include "sdl_application.hpp"

namespace charlie {
	Networkinfo::Networkinfo():
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

	void Networkinfo::render(SDL_Renderer* renderer, const network::Connection connection, const TextHandler &text_handler)
	{
		render_text(renderer, connection, text_handler, "latency: %ld ms", Color::white, 0, 0);
		render_text(renderer, connection, text_handler, "input corr: %i", Color::white, 150, 0);
		render_text(renderer, connection, text_handler, "rtt: %ld ms", Color::white, 300, 0);
		render_text(renderer, connection, text_handler, "max: %ld ms", Color::white, 450, 0);
		render_text(renderer, connection, text_handler, "min: %ld ms", Color::white, 0, 20);
		render_text(renderer, connection, text_handler, "avg: %ld ms", Color::white, 150, 20);
		render_text(renderer, connection, text_handler, "in: %f KiB", Color::white, 0, 40);
		render_text(renderer, connection, text_handler, "out: %f KiB", Color::white, 150, 40);
		render_text(renderer, connection, text_handler, "p in: %i", Color::white, 0, 60);
		render_text(renderer, connection, text_handler, "p out: %i", Color::white, 150, 60);
	}

	void Networkinfo::render_text(
		SDL_Renderer* renderer,
		const network::Connection connection,
		const TextHandler &text_handler,
		const char* text,
		SDL_Color color, int x, int y)
	{
		const auto length = strlen(text);
		char buffer[20] = "";
		rect_ = {x, y, (int)length*10, 15};
		sprintf_s(buffer, text, (long)connection.latency().as_milliseconds());
		SDLSprite* sprite = text_handler.CreateText(buffer, color, rect_.x, rect_.y, rect_.w, rect_.h);
		SDL_RenderCopy(renderer, sprite->get_texture(), nullptr, &rect_);
		sprite->destroy();
	}

	void Networkinfo::packet_received(int32 size)
	{
		packets_recv_++;
		bytes_recv_ += (float)size;
	}

	void Networkinfo::packet_sent(int32 size)
	{
		packets_sent_++;
		bytes_sent_ += (float)size;
	}
}
