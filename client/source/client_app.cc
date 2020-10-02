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

		Vector2 direction;
		player_.input_bits_ = 0;
		if (keyboard_.down(Keyboard::Key::W)) {
			player_.input_bits_ |= (1 << int32(gameplay::Action::Up));
			direction.y_ -= 1.0f;
		}
		if (keyboard_.down(Keyboard::Key::S)) {
			player_.input_bits_ |= (1 << int32(gameplay::Action::Down));
			direction.y_ += 1.0f;
		}
		if (keyboard_.down(Keyboard::Key::A)) {
			player_.input_bits_ |= (1 << int32(gameplay::Action::Left));
			direction.x_ -= 1.0f;
		}
		if (keyboard_.down(Keyboard::Key::D)) {
			player_.input_bits_ |= (1 << int32(gameplay::Action::Right));
			direction.x_ += 1.0f;
		}

		const float speed = 100.0;
		if (direction.length() > 0.0f) {
			direction.normalize();
			player_.position_ += direction * speed * tickrate_.as_seconds();
		}

		gameplay::InputSnapshot snapshot;
		snapshot.input_bits_ = player_.input_bits_;
		snapshot.tick_ = tick_;
		snapshot.position_ = player_.position_;

		inputinator_.add_snapshot(snapshot);

		for (auto& entity : entities_)
		{
			entity.interpolator_.acc_ += dt;
			entity.position_ = entity.interpolator_.interpolate(rtt_);
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
			server_time_ = current;
			lastReceive_ = Time::now();
		} break;

		case network::NETWORK_MESSAGE_ENTITY_STATE:
		{
			network::NetworkMessageEntityState message;
			if (!message.read(reader)) {
				assert(!"could not read message!");
			}

			const uint32 id = message.id_;

			if (entities_.empty())
			{
				entities_.push_back(gameplay::Entity(message.position_, message.id_));
				printf("Remote player connected: %i \n", (int)entities_.size());
			}

			auto it = entities_.begin();
			while (it != entities_.end())
			{
				if ((*it).id_ == id) {
					break;
				}

				++it;
				if (it == entities_.end())
				{
					entities_.push_back(gameplay::Entity(message.position_, message.id_));
					printf("Remote player connected: %i \n", (int)entities_.size());
					break;
				}
			}

			for (auto& entity : entities_)
			{
				if (entity.id_ == id)
				{
					gameplay::PosSnapshot snapshot;
					snapshot.servertime_ = server_time_;
					snapshot.position = message.position_;

					entity.interpolator_.add_position(snapshot);
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

			auto recalculated = inputinator_.get_position(server_tick_, tickrate_);
			auto diff = message.position_ - recalculated;
			if (diff.length() > 5.0f)
			{
				player_.position_ = recalculated;
			}
			auto newdiff = message.position_ - player_.position_;
			if (newdiff.length() > 5.0f)
			{
				player_.position_ = message.position_;
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
