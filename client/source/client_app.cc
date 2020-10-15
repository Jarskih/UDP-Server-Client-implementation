// client_app.cc

#include "client_app.hpp"
#include <charlie_messages.hpp>
#include <cstdio>

#include "level_manager.h"

template <typename T, std::size_t N>
constexpr auto array_size(T(&)[N])
{
	return N;
}

ClientApp::ClientApp()
	: tickrate_(1.0 / 60.0)
	, level_manager_()
	, tick_(0)
	, server_tick_(0)
{
}

bool ClientApp::on_init()
{
	network_.set_send_rate(Time(1.0 / 10.0));
	if (!network_.initialize({})) {
		return false;
	}

	connection_.set_listener(this);

	connection_.connect(network::IPAddress::get_broadcast(54345));

	Vector2 pos = Vector2(200, 300);
	player_.init(renderer_.get_renderer(), pos, 0);
	player_.load_body_sprite("../assets/tank_body.png", 0, 0, 50, 0);
	player_.load_turret_sprite("../assets/tank_turret.png", 0, 0, 30, 0);

	auto data = Leveldata();
	data.create_level("../assets/map.txt");
	level_manager_ = LevelManager(data);

	return true;
}

void ClientApp::on_exit()
{
}

bool ClientApp::on_tick(const Time& dt)
{
	input_handler_.HandleEvents();
	if (input_handler_.IsKeyDown(SDL_SCANCODE_ESCAPE))
	{
		return false;
	}

	accumulator_ += dt;
	while (accumulator_ >= tickrate_) {
		accumulator_ -= tickrate_;
		server_tick_++;

		networkinfo_.update(dt, connection_);

		player_.update(dt);

		{
			gameplay::InputSnapshot snapshot;
			snapshot.input_bits_ = player_.input_bits_;
			snapshot.tick_ = server_tick_;
			snapshot.position_ = player_.transform_.position_;

			inputinator_.add_snapshot(snapshot);
		}

		for (auto& entity : entities_)
		{
			entity.interpolator_.acc_ += dt;
			entity.position_ = entity.interpolator_.interpolate();
		}
	}
	return true;
}

void ClientApp::on_draw()
{
	// CLEARING SCREEN


	// UPDATING FSM
	//stateMachine.Update();
	level_manager_.render(renderer_.get_renderer());
	player_.render();

	// PRESENTING TO THE SCREEN


	//renderer_.clear({ 0.2f, 0.3f, 0.4f, 1.0f });
	//renderer_.render_text({ 2, 2 }, Color::White, 1, "CLIENT");

	//networkinfo_.render(renderer_, connection_);

	//for (auto& entity : entities_)
	//{
	//	renderer_.render_rectangle_fill({ int32(entity.position_.x_), int32(entity.position_.y_), 20, 20 }, Color::Green);
	//}
	//renderer_.render_rectangle_fill({ int32(player_.position_.x_), int32(player_.position_.y_), 20, 20 }, Color::Magenta);
}

void ClientApp::on_acknowledge(network::Connection* connection,
	const uint16 sequence)
{
}

void ClientApp::on_receive(network::Connection* connection,
	network::NetworkStreamReader& reader)
{
	networkinfo_.packet_received(reader.length());

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
				//entities_.push_back(gameplay::Entity(message.position_, message.id_));
				//printf("Remote player connected: %i \n", (int)entities_.size());
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
					//entities_.push_back(gameplay::Entity(message.position_, message.id_));
					//printf("Remote player connected: %i \n", (int)entities_.size());
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

			Vector2 recalculated = inputinator_.old_pos(server_tick_);
			auto diff = message.position_ - recalculated;
			if (abs(diff.x_) > 5.0f || abs(diff.y_) > 5.0f)
			{
				player_.transform_.position_ = inputinator_.get_position(server_tick_, tickrate_, message.position_, player_.speed_);
				networkinfo_.input_misprediction_++;
			}
			player_.transform_.rotation_ = message.rotation_;
		} break;

		case network::NETWORK_MESSAGE_PLAYER_SPAWN:
		{
			printf("Spawn message received");
			network::NetworkMessagePlayerSpawn message;
			if (!message.read(reader)) {
				assert(!"could not read message!");
			}

			for (auto& entity : entities_)
			{
				if (entity.id_ == message.id_)
				{
					break;
				}

				gameplay::Entity e(message.position_, message.id_);
				entities_.push_back(e);
				printf("Player spawned %i \n", message.id_);
			}

			if (entities_.empty())
			{
				gameplay::Entity e(message.position_, message.id_);
				entities_.push_back(e);
				printf("Player spawned %i \n", message.id_);
			}
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

	networkinfo_.packet_sent(writer.length());
}
