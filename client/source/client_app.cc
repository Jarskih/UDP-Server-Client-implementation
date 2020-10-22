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
{
}

bool ClientApp::on_init()
{
	network_.set_send_rate(Time(1.0 / 60.0));
	if (!network_.initialize({})) {
		return false;
	}

	connection_.set_listener(this);

	connection_.connect(network::IPAddress::get_broadcast(54345));

	Vector2 pos = Vector2(200, 300);
	player_.init(renderer_.get_renderer(), pos, 0);
	player_.load_body_sprite("../assets/tank_body.png", 0, 0, config::PLAYER_WIDTH, config::PLAYER_HEIGHT);
	player_.load_turret_sprite("../assets/tank_turret.png", 0, 0, config::PLAYER_WIDTH, config::PLAYER_HEIGHT);

	auto data = Leveldata();
	data.create_level("../assets/map.txt");
	level_manager_ = LevelManager();
	level_manager_.load_assets(data);

	level_width_ = level_manager_.width_;
	level_heigth_ = level_manager_.height_;

	text_font_.create("../assets/font/font.ttf", 20, SDL_Color({255,255,255,255}));
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

		if(player_.fire_ && player_.can_shoot())
		{
			spawn_local_projectile(player_.get_shoot_pos(), player_.turret_rotation_);
			player_.fire();
		}
		
		{
			const int delayInTicks = (int)(networkinfo_.rtt_avg_ / tickrate_.as_milliseconds());

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

		cam_.x = (int)player_.transform_.position_.x_ + player_.body_sprite_->get_area().w / 2 - config::SCREEN_WIDTH / 2;
		cam_.y = (int)player_.transform_.position_.y_ + player_.body_sprite_->get_area().h / 2 - config::SCREEN_HEIGHT / 2;

		if (cam_.x < 0)
		{
			cam_.x = 0;
		}
		if (cam_.y < 0)
		{
			cam_.y = 0;
		}
		if (cam_.x > level_width_ - cam_.w)
		{
			cam_.x = level_width_ - cam_.w;
		}
		if (cam_.y > level_heigth_ - cam_.h)
		{
			cam_.y = level_heigth_ - cam_.h;
		}
	}
	return true;
}

void ClientApp::on_draw()
{
	level_manager_.render(cam_, renderer_.get_renderer());

	for (auto& entity : entities_)
	{
		entity.render(cam_);
	}

	for(auto& projectile : projectiles_) 
	{
		projectile.render(cam_);
	}

	player_.render(cam_);

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

			gameplay::PosSnapshot snapshot;
			snapshot.servertime_ = server_time_;
			snapshot.position = message.position_;
			snapshot.rotation = message.rotation_;
			snapshot.turret_rotation = message.turret_rotation_;

			for (auto& e : entities_)
			{
				if(e.id_ == id)
				{
					e.interpolator_.acc_ = Time(0.0);
					e.interpolator_.add_position(snapshot);
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

			// Remove inputs before ack from server
			inputinator_.clear_old_inputs(server_tick_);

			// Find old position with ack and compare to server pos
			gameplay::InputSnapshot input = inputinator_.get_snapshot(server_tick_);
			const auto diff = input.position_ - message.position_;
				
			// If 5px mistake correct calculate new predicted pos using server pos
			const float correct_dist = 5.0f;
			if (abs(diff.x_) > correct_dist || abs(diff.y_) > correct_dist)
			{
				player_.transform_.position_ = inputinator_.get_corrected_position(server_tick_, tickrate_, message.position_, player_.speed_);
				networkinfo_.input_misprediction_++;
			}

			if(abs(input.turret_rotation - message.turret_rotation_) > correct_dist)
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

			for (auto& entity : entities_)
			{
				if (entity.id_ == message.id_)
				{
					break;
				}
				spawn_entity(message);
			}

			if (entities_.empty())
			{
				spawn_entity(message);
			}

			network::NetworkMessagePlayerSpawnAck msg;
			msg.id_ = player_.id_;
			spawn_message_queue_.push(msg);
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

	while (!spawn_message_queue_.empty())
	{
		network::NetworkMessagePlayerSpawnAck msg = spawn_message_queue_.front();
		if (!msg.write(writer)) {
			assert(!"could not write network command!");
		}
		spawn_message_queue_.pop();
		//printf("Sent spawn ack to server \n");
	}

	lastSend_ = Time::now();

	networkinfo_.packet_sent(writer.length());
}

void ClientApp::spawn_entity(network::NetworkMessagePlayerSpawn message)
{
	Entity e{};
	e.init(renderer_.get_renderer(), message.position_, message.id_);
	e.load_body_sprite("../assets/tank_body.png", 0, 0, config::PLAYER_WIDTH, config::PLAYER_HEIGHT);
	e.load_turret_sprite("../assets/tank_turret.png", 0, 0, config::PLAYER_WIDTH, config::PLAYER_HEIGHT);
	entities_.push_back(e);
	printf("Remote player spawned with id: %i \n", message.id_);
}

void ClientApp::spawn_projectile(network::NetworkMessageProjectileSpawn message)
{
	if(message.id_ == player_.id_) // is owned by local player?
	{
		printf("Got projectile spawn for owning player");
		return;
	}
	Projectile e(message.pos_, message.rotation_, message.id_);
	e.renderer_ = renderer_.get_renderer();
	e.load_sprite("../assets/Light_Shell.png", 0, 0, config::PROJECTILE_WIDTH, config::PROJECTILE_HEIGHT);
	projectiles_.push_back(e);
	printf("Remote projectile spawned with owner: %i \n", message.id_);
}

void ClientApp::spawn_local_projectile(Vector2 pos, float rotation)
{
	Projectile e(pos, rotation, player_.id_);
	e.renderer_ = renderer_.get_renderer();
	e.load_sprite("../assets/Light_Shell.png", 0, 0, config::PROJECTILE_WIDTH, config::PROJECTILE_HEIGHT);
	projectiles_.push_back(e);
	printf("Spawned projectile \n");
}
