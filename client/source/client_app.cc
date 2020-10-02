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
	, tick_(0), server_tick_(0)
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

		const bool player_move_up = player_.input_bits_ & (1 << int32(gameplay::Action::Up));
		const bool player_move_down = player_.input_bits_ & (1 << int32(gameplay::Action::Down));
		const bool player_move_left = player_.input_bits_ & (1 << int32(gameplay::Action::Left));
		const bool player_move_right = player_.input_bits_ & (1 << int32(gameplay::Action::Right));

		Vector2 direction;
		if (player_move_up) {
			direction.y_ -= 1.0f;
		}
		if (player_move_down) {
			direction.y_ += 1.0f;
		}
		if (player_move_left) {
			direction.x_ -= 1.0f;
		}
		if (player_move_right) {
			direction.x_ += 1.0f;
		}

		const float speed = 100.0;
		if (direction.length() > 0.0f) {
			direction.normalize();
			player_.position_ += direction * speed * tickrate_.as_seconds();
		}
	}

	gameplay::InputSnapshot snapshot;
	snapshot.input_bits_ = player_.input_bits_;
	snapshot.tick_ = tick_;
	snapshot.position_ = player_.position_;

	inputinator_.add_snapshot(snapshot);

	interpolator_.acc_ += dt.as_seconds();

	for (auto& entity : entities_)
	{
		// interpolator_.interpolate();
	}
}

return true;
}

void ClientApp::on_draw()
{
	renderer_.clear({ 0.2f, 0.3f, 0.4f, 1.0f });
	renderer_.render_text({ 2, 2 }, Color::White, 1, "CLIENT");
	char latency[10] = "";
	sprintf_s(latency, "%ld ms", (long)connection_.latency().as_milliseconds());
	renderer_.render_text({ 100, 2 }, Color::White, 1, latency);

	for (auto& entity : entities_)
	{
		renderer_.render_rectangle_fill({ int32(entity.position_.x_), int32(entity.position_.y_), 20, 20 }, Color::Green);
	}
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
			server_tick_ = message.server_tick_;
			lastReceive_ = Time::now();
		} break;

		case network::NETWORK_MESSAGE_ENTITY_STATE:
		{
			network::NetworkMessageEntityState message;
			if (!message.read(reader)) {
				assert(!"could not read message!");
			}

			auto id = message.id_;
			for (auto& entity : entities_)
			{
				if (entity.id_ == id)
				{
					break;
				}
				entities_.push_back(gameplay::Entity(message.position_, message.id_));
				printf("Remote player connected\n");
			}

			for (auto& entity : entities_)
			{
				if (entity.id_ == id)
				{
					entity.position_ = message.position_;
					break;
				}
			}
		} break;

		case network::NETWORK_MESSAGE_PLAYER_STATE:
		{
			network::NetworkMessagePlayerState message;
			if (!message.read(reader)) {
				assert(!"could not read message!");
			}

			auto playerPos_ = inputinator_.get_position(server_tick_, tickrate_);
			auto diff = message.position_ - playerPos_;
			if (diff.length() > 5.0f)
			{
				player_.position_ = playerPos_;
				printf("Player position corrected new simulated pos\n");
			}
			if (diff.length() > 5.0f)
			{
				player_.position_ = message.position_;
				printf("Player position corrected to server pos\n");
			}
		} break;

		case network::NETWORK_MESSAGE_SPAWN:
		{
			network::NetworkMessagePlayerSpawn message;
			if (!message.read(reader)) {
				assert(!"could not read message!");
			}

			gameplay::Entity entity(message.position_, message.id_);
			entities_.push_back(entity);
			printf("Spawn message received \n");
		}
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
	network::NetworkMessageInputCommand command(player_.input_bits_);
	if (!command.write(writer)) {
		assert(!"could not write network command!");
	}
	lastSend_ = Time::now();
}
