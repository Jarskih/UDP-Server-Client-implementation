// client_app.cc

#include "client_app.hpp"
#include <charlie_messages.hpp>
#include <cstdio>

template <typename T, std::size_t N>
constexpr auto array_size(T(&)[N])
{
	return N;
}

ClientApp::ClientApp()
	: mouse_(window_.mouse_)
	, keyboard_(window_.keyboard_)
	, tickrate_(1.0 / 60.0)
	, input_bits_(0), tick_(0)
{
}

bool ClientApp::on_init()
{
	network_.set_send_rate(Time(1.0 / 10.0));
	if (!network_.initialize({})) {
		return false;
	}

	connection_.set_listener(this);

	connection_.connect(network::IPAddress(192, 168, 1, 230, 54345));

	return true;
}

void ClientApp::on_exit()
{
}

bool ClientApp::on_tick(const Time& dt)
{
	if (keyboard_.pressed(Keyboard::Key::Escape)) {
		return false;
	}

	accumulator_ += dt;
	while (accumulator_ >= tickrate_) {
		accumulator_ -= tickrate_;
		tick_++;

		Vector2 direction;
		input_bits_ = 0;
		if (keyboard_.down(Keyboard::Key::W)) {
			input_bits_ |= (1 << int32(gameplay::Action::Up));
			direction.y_ -= 1.0f;
		}
		if (keyboard_.down(Keyboard::Key::S)) {
			input_bits_ |= (1 << int32(gameplay::Action::Down));
			direction.y_ += 1.0f;
		}
		if (keyboard_.down(Keyboard::Key::A)) {
			input_bits_ |= (1 << int32(gameplay::Action::Left));
			direction.x_ -= 1.0f;
		}
		if (keyboard_.down(Keyboard::Key::D)) {
			input_bits_ |= (1 << int32(gameplay::Action::Right));
			direction.x_ += 1.0f;
		}

		const float speed = 100.0;
		if (direction.length() > 0.0f) {
			direction.normalize();
			player_.position_ += direction * speed * tickrate_.as_seconds();
		}

		gameplay::InputSnapshot snapshot;
		snapshot.input_bits_ = input_bits_;
		snapshot.tick_ = tick_;
		snapshot.position_ = player_.position_;

		interpolator_.add_snapshot(snapshot);
		interpolator_.acc_ += dt.as_seconds();
	}

	return true;
}

void ClientApp::on_draw()
{
	renderer_.clear({ 0.2f, 0.3f, 0.4f, 1.0f });
	renderer_.render_text({ 2, 2 }, Color::White, 1, "CLIENT");
	renderer_.render_rectangle_fill({ int32(entity_.position_.x_), int32(entity_.position_.y_), 20, 20 }, Color::Green);
	renderer_.render_rectangle_fill({ int32(player_.position_.x_), int32(player_.position_.y_), 20, 20 }, Color::Magenta);
}

void ClientApp::on_acknowledge(network::Connection* connection,
	const uint16 sequence)
{
}

void ClientApp::on_receive(network::Connection* connection,
	network::NetworkStreamReader& reader)
{
	while (reader.position() < reader.length()) {
		switch (reader.peek()) {
		case network::NETWORK_MESSAGE_SERVER_TICK:
		{
			network::NetworkMessageServerTick message;
			if (!message.read(reader)) {
				assert(!"could not read message!");
			}

			const Time current = Time(message.server_time_);
			message.server_tick_;
			playerPos_ = interpolator_.get_position(message.server_tick_, tickrate_);
			lastReceive_ = Time::now();
		} break;

		case network::NETWORK_MESSAGE_ENTITY_STATE:
		{
			network::NetworkMessageEntityState message;
			if (!message.read(reader)) {
				assert(!"could not read message!");
			}

			entity_.position_ = message.position_;
		} break;

		case network::NETWORK_MESSAGE_PLAYER_STATE:
		{
			network::NetworkMessagePlayerState message;
			if (!message.read(reader)) {
				assert(!"could not read message!");
			}

			interpolator_.acc_ = 0;
			if (playerPos_.length() > 1)
			{
				player_.position_ = message.position_;
				continue;
			}
			auto diff = message.position_ - playerPos_;
			if (diff.length() > 5.0f)
			{
				player_.position_ = playerPos_;
			}
		} break;

		default:
		{
			assert(!"unknown message type received from server!");
		} break;
		}
	}
}

void ClientApp::on_send(network::Connection* connection,
	const uint16 sequence,
	network::NetworkStreamWriter& writer)
{
	network::NetworkMessageInputCommand command(input_bits_);
	if (!command.write(writer)) {
		assert(!"could not write network command!");
	}
	lastSend_ = Time::now();
}
