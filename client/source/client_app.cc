// client_app.cc

#include "client_app.hpp"
#include <charlie_messages.hpp>
#include <cstdio>
#include "config.h"
#include "entity.h"
#include "level_manager.h"
#include "projectile.h"

template <typename T, std::size_t N>
constexpr auto array_size(T(&)[N])
{
	return N;
}

ClientApp::ClientApp()
	: tickrate_(1.0 / 60.0)
	, tick_(0)
	, server_tick_(0)
	, local_projectile_index_(0)
{
}

bool ClientApp::on_init()
{
	network_.set_send_rate(Time(1.0 / 60.0));
	if (!network_.initialize({})) {
		return false;
	}

	connection_.set_listener(this);
	connection_.connect(network::IPAddress(config::IP_A, config::IP_B, config::IP_C, config::IP_D, config::PORT));

	Vector2 pos = Vector2(200, 300);
	player_.init(renderer_.get_renderer(), pos, 0);
	player_.load_body_sprite(config::TANK_BODY_SPRITE, 0, 0, config::PLAYER_WIDTH, config::PLAYER_HEIGHT);
	player_.load_turret_sprite(config::TANK_TURRET_SPRITE, 0, 0, config::PLAYER_WIDTH, config::PLAYER_HEIGHT);

	auto data = Leveldata();
	data.create_level(config::LEVEL1);
	level_manager_ = LevelManager();
	level_manager_.load_assets(data);

	level_width_ = level_manager_.width_;
	level_heigth_ = level_manager_.height_;

	cam_.init(level_width_, level_heigth_, { 0, 0, 640, 480 });

	text_font_.create(config::FONT_PATH, 20, SDL_Color({ 255,255,255,255 }));
	text_handler_.renderer_ = renderer_.get_renderer();
	text_handler_.LoadFont(text_font_);

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

		networkinfo_.update(dt, connection_);

		player_.update(dt, level_heigth_, level_width_);

		if (player_.fire_ && player_.can_shoot())
		{
			// TODO decide if should use local projectiles
			//spawn_local_projectile(player_.get_shoot_pos(), player_.turret_rotation_);
			player_.fire();
		}

		{
			const uint32 delayInTicks = (uint32)(networkinfo_.rtt_avg_ / tickrate_.as_milliseconds());

			gameplay::InputSnapshot snapshot;
			snapshot.input_bits_ = player_.input_bits_;
			snapshot.tick_ = server_tick_ + delayInTicks;
			snapshot.position_ = player_.transform_.position_;
			snapshot.rotation_ = player_.transform_.rotation_;
			snapshot.turret_rotation = player_.turret_rotation_;
			snapshot.fire_ = player_.fire_;

			inputinator_.add_snapshot(snapshot);
		}

		for (auto& entity : entities_)
		{
			entity.interpolator_.acc_ += dt;
			entity.transform_.position_ = entity.interpolator_.interpolate_pos(networkinfo_.rtt_avg_);
			entity.transform_.rotation_ = entity.interpolator_.interpolate_rot();
			entity.turret_rotation_ = entity.interpolator_.interpolate_turret_rot();
		}

		for (auto& projectile : projectiles_)
		{
			projectile.update(dt);
		}

		for (auto& projectile : local_projectiles_)
		{
			projectile.update(dt);
		}

		cam_.lookAt(player_);

		for (auto& id : entities_to_remove_)
		{
			remove_entity(id);
		}
		entities_to_remove_.clear();

		for (auto& id : projectiles_to_remove_)
		{
			remove_projectile(id);
		}
		projectiles_to_remove_.clear();
	}
	return true;
}

void ClientApp::on_draw()
{
	level_manager_.render(cam_.rect_, renderer_.get_renderer());

	for (auto& entity : entities_)
	{
		entity.render(cam_.rect_);
	}

	for (auto& projectile : projectiles_)
	{
		projectile.render(cam_.rect_);
	}

	for (auto& projectile : local_projectiles_)
	{
		projectile.render(cam_.rect_);
	}

	player_.render(cam_.rect_);

	networkinfo_.render(renderer_.get_renderer(), connection_, text_handler_);
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

			server_tick_ = message.server_tick_;
			server_time_ = Time(message.server_time_);
			lastReceive_ = Time::now();
		} break;

		case network::NETWORK_MESSAGE_ENTITY_STATE:
		{
			network::NetworkMessageEntityState message;
			if (!message.read(reader)) {
				assert(!"could not read message!");
			}

			const uint32 id = message.entity_id_;

			gameplay::PosSnapshot snapshot;
			snapshot.tick_ = server_tick_;
			snapshot.servertime_ = server_time_;
			snapshot.position = message.position_;
			snapshot.rotation = message.rotation_;
			snapshot.turret_rotation = message.turret_rotation_;

			for (auto& e : entities_)
			{
				if (e.id_ == id)
				{
					e.interpolator_.acc_ = Time(0.0);
					e.interpolator_.add_position(snapshot);
					e.interpolator_.clear_old_snapshots();
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

			// Find old position with ack and compare to server pos
			gameplay::InputSnapshot input = inputinator_.get_snapshot(server_tick_);

			auto diff = Vector2(input.position_.x_ - message.x_, input.position_.y_ - message.y_);

			diff = player_.transform_.position_ - Vector2(message.x_, message.y_);

			// If 5px mistake correct calculate new predicted pos using server pos
			const float correct_dist = 5.0f;
			if (diff.length() > correct_dist)
			{
				player_.transform_.position_ = inputinator_.get_corrected_position(server_tick_, tickrate_, Vector2(message.x_, message.y_), player_.speed_);
				networkinfo_.input_misprediction_++;
			}

			if (abs(input.turret_rotation - message.turret_rotation_) > correct_dist)
			{
				player_.turret_rotation_ = message.turret_rotation_;
			}


			if (abs(input.rotation_ - message.rotation_) > correct_dist)
			{
				player_.transform_.set_rotation(message.rotation_);
			}
		} break;

		case network::NETWORK_MESSAGE_PLAYER_SPAWN:
		{
			network::NetworkMessagePlayerSpawn message;
			if (!message.read(reader)) {
				assert(!"could not read message!");
			}

			if (entities_.empty() || !contains(entities_, message.entity_id_))
			{
				spawn_entity(message);
			}

			create_ack_message(message.event_id_);
		} break;

		case network::NETWORK_MESSAGE_DISCONNECTED:
		{
			network::NetworkMessagePlayerDisconnected message;
			if (!message.read(reader)) {
				assert(!"could not read message!");
			}

			if (contains(entities_, message.entity_id_))
			{
				entities_to_remove_.push_back(message.entity_id_);
			}

			create_ack_message(message.message_id_);
		} break;
		case network::NETWORK_MESSAGE_PROJECTILE_SPAWN:
		{
			network::NetworkMessageProjectileSpawn message;
			if (!message.read(reader)) {
				assert(!"could not read message!");
			}

			if (projectiles_.empty() || !contains(projectiles_, message.entity_id_))
			{
				spawn_projectile(message);
			}

			create_ack_message(message.event_id_);
		} break;

		case network::NETWORK_MESSAGE_PROJECTILE_DESTROYED:
		{
			network::NetworkMessageProjectileDestroy message;
			if (!message.read(reader)) {
				assert(!"could not read message!");
			}

			if (contains(projectiles_, message.entity_id_))
			{
				projectiles_to_remove_.push_back(message.entity_id_);
				printf("RELIABLE MESSAGE: Destroying projectile: %i \n", message.entity_id_);
			}

			create_ack_message(message.event_id_);
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
	network::NetworkMessageInputCommand command(player_.input_bits_, player_.turret_rotation_, player_.fire_);
	if (!command.write(writer)) {
		assert(!"could not write network command!");
	}

	while (!message_queue_.empty())
	{
		network::NetworkMessageAck msg = message_queue_.front();
		if (!msg.write(writer)) {
			assert(!"could not write network command!");
		}
		message_queue_.pop();
	}

	lastSend_ = Time::now();

	networkinfo_.packet_sent(writer.length());
}

void ClientApp::spawn_entity(network::NetworkMessagePlayerSpawn message)
{
	Entity e{};
	e.init(renderer_.get_renderer(), message.position_, message.entity_id_);
	e.load_body_sprite(config::TANK_BODY_SPRITE, 0, 0, config::PLAYER_WIDTH, config::PLAYER_HEIGHT);
	e.load_turret_sprite(config::TANK_TURRET_SPRITE, 0, 0, config::PLAYER_WIDTH, config::PLAYER_HEIGHT);
	entities_.push_back(e);
	printf("RELIABLE MESSAGE: Remote player spawned with message id: %i \n", message.event_id_);
}

void ClientApp::remove_entity(uint32 id)
{
	auto it = entities_.begin();
	while (it != entities_.end())
	{
		if ((*it).id_ == id)
		{
			(*it).destroy();
			entities_.erase(it);
			break;
		}
		++it;
	}
}

void ClientApp::remove_projectile(uint32 id)
{
	auto it = projectiles_.begin();
	while (it != projectiles_.end())
	{
		if ((*it).id_ == id)
		{
			(*it).destroy();
			projectiles_.erase(it);
			break;
		}
		++it;
	}
}

void ClientApp::spawn_projectile(network::NetworkMessageProjectileSpawn message)
{
	Projectile e(message.pos_, message.rotation_, message.entity_id_, message.shot_by_);
	e.renderer_ = renderer_.get_renderer();
	e.load_sprite(config::TANK_SHELL, 0, 0, config::PROJECTILE_WIDTH, config::PROJECTILE_HEIGHT);
	projectiles_.push_back(e);
	printf("RELIABLE MESSAGE: Remote projectile: %i spawned with owner: %i \n", message.entity_id_, message.shot_by_);
}

bool ClientApp::contains(const DynamicArray<Entity>& vector, uint32 id)
{
	for (const auto& element : vector)
	{
		if (element.id_ == id)
		{
			return true;
		}
	}
	return false;
}

bool ClientApp::contains(const DynamicArray<Projectile>& vector, uint32 id)
{
	for (const auto& element : vector)
	{
		if (element.id_ == id)
		{
			return true;
		}
	}
	return false;
}

void ClientApp::create_ack_message(uint32 event_id_)
{
	network::NetworkMessageAck msg;
	msg.event_id_ = event_id_;
	message_queue_.push(msg);
	printf("RELIABLE MESSAGE: Message confirmation sent with id %i \n", msg.event_id_);
}
