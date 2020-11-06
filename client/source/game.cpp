#include "charlie.hpp"
#include "client_app.hpp"
#include "config.h"
#include "game.h"
#include "level_manager.h"
#include "leveldata.h"
#include "player.hpp"
#include "Singleton.hpp"

namespace charlie
{
	Game::Game()
		: renderer_(nullptr)
		, tickrate_(1.0 / 60.0)
		, tick_(0)
		, server_tick_(0)
		, local_projectile_index_(0)
		, level_width_(0)
		, level_heigth_(0)
	{
	}

	bool Game::on_init(SDL_Renderer* renderer)
	{
		renderer_ = renderer;
		connection_.set_listener(this);
		connection_.connect(server_);

		text_font_.create(config::FONT_PATH, 20, SDL_Color({ 255,255,255,255 }));
		text_handler_.renderer_ = renderer_;
		text_handler_.LoadFont(text_font_);

		disconnected_ = Singleton<SpriteHandler>::Get()->create_sprite(config::DISCONNECTED, 0, 0, config::SCREEN_WIDTH, config::SCREEN_HEIGHT);
		if (disconnected_ == nullptr)
		{
			return false;
		}

		return true;
	}

	void Game::on_exit()
	{
	}

	bool Game::on_tick(const Time& dt)
	{
		for (auto& e : entities_)
		{
			if (e.id_ == player_.id_)
			{
				assert(!"Entity has same id as player");
			}
		}

		Singleton<InputHandler>::Get()->HandleEvents();
		if (Singleton<InputHandler>::Get()->IsKeyDown(SDL_SCANCODE_ESCAPE))
		{
			player_.state_ = PlayerState::DEAD;
		}

		if (player_.is_waiting_to_spawn())
		{
			return true;
		}

		accumulator_ += dt;
		while (accumulator_ >= tickrate_) {
			accumulator_ -= tickrate_;

			networkinfo_.update(dt, connection_);

			player_.update(dt, level_heigth_, level_width_);

			{
				const uint32 delayInTicks = (uint32)(networkinfo_.rtt_avg_ / tickrate_.as_milliseconds());

				gameplay::InputSnapshot snapshot;
				snapshot.input_bits_ = player_.input_bits_;
				snapshot.tick_ = server_tick_ + delayInTicks;
				snapshot.position_ = player_.transform_.position_;
				snapshot.rotation_ = player_.transform_.rotation_;
				snapshot.turret_rotation = player_.turret_transform_.rotation_;

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

			cam_.lookAt(player_);

			for (auto& id : entities_to_remove_)
			{
				remove_entity(id);
			}
			entities_to_remove_.clear();

			for (auto& projectile : projectiles_)
			{
				if (projectile.is_dead())
				{
					projectiles_to_remove_.push_back(projectile.id_);
				}
			}

			for (auto& id : projectiles_to_remove_)
			{
				printf("Projectile to destroy %i \n", id);
				remove_projectile(id);
			}
			projectiles_to_remove_.clear();

			if (player_.is_dead())
			{
				return false;
			}
		}
		return true;
	}

	void Game::on_draw()
	{
		level_manager_.render(cam_.rect_, renderer_);

		for (auto& entity : entities_)
		{
			entity.render(cam_.rect_);
		}

		for (auto& projectile : projectiles_)
		{
			projectile.render(cam_.rect_);
		}

		player_.render(cam_.rect_);

		networkinfo_.render(renderer_, connection_, text_handler_);

		if(connection_.is_disconnected() || connection_.state_ == network::Connection::State::Invalid || connection_.state_ == network::Connection::State::Timedout)
		{
			SDL_RenderCopy(renderer_, disconnected_->get_texture(), nullptr, nullptr);
		}
	}

	void Game::on_acknowledge(network::Connection* connection, const uint16 sequence)
	{
	}

	void Game::on_receive(network::Connection* connection, network::NetworkStreamReader& reader)
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
				snapshot.position.x_ = message.x_;
				snapshot.position.y_ = message.y_;
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

				auto diff = Vector2(input.position_.x_ - (float)message.x_, input.position_.y_ - (float)message.y_);

				diff = player_.transform_.position_ - Vector2(message.x_, message.y_);

				// If 5px mistake correct calculate new predicted pos using server pos
				const float correct_dist = 5.0f;
				if (diff.length() > correct_dist)
				{
					player_.transform_.position_ = inputinator_.get_corrected_position(server_tick_, tickrate_, Vector2(message.x_, message.y_), player_.speed_);
					networkinfo_.input_misprediction_++;
				}

				if (abs(input.turret_rotation - (float)message.turret_rotation_) > correct_dist)
				{
					player_.turret_transform_.rotation_ = message.turret_rotation_;
				}


				if (abs(input.rotation_ - (float)message.rotation_) > correct_dist)
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

				spawn_player(message);

				create_ack_message(message.event_id_);
			} break;

			case network::NETWORK_MESSAGE_ENTITY_SPAWN:
			{
				network::NetworkMessageEntitySpawn message;
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
			case network::NETWORK_MESSAGE_PLAYER_DESTROYED:
			{
				network::NetworkMessagePlayerDestroy message;
				if (!message.read(reader)) {
					assert(!"could not read message!");
				}

				player_.state_ = PlayerState::DEAD;

				create_ack_message(message.event_id_);

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

			case network::NETWORK_MESSAGE_ENTITY_DESTROYED:
			{
				network::NetworkMessageEntityDestroy message;
				if (!message.read(reader)) {
					assert(!"could not read message!");
				}

				if (contains(entities_, message.entity_id_))
				{
					entities_to_remove_.push_back(message.entity_id_);
					printf("RELIABLE MESSAGE: Destroying projectile: %i \n", message.entity_id_);
				}

				create_ack_message(message.event_id_);
			} break;

			case network::NETWORK_MESSAGE_LEVEL_INFO:
			{
				network::NetworkMessageLevelInfo message;
				if (!message.read(reader)) {
					assert(!"could not read message!");
				}

				level_width_ = message.size_x_ * config::LEVEL_OBJECT_WIDTH;
				level_heigth_ = message.size_y_ * config::LEVEL_OBJECT_HEIGHT;

				cam_.init(level_width_, level_heigth_, { (int)player_.transform_.position_.x_, (int)player_.transform_.position_.y_, config::SCREEN_WIDTH, config::SCREEN_HEIGHT });

				auto data = Leveldata();
				//Attempt to load level if exits on client
				if (data.create_level(message.level_id_))
				{
					level_manager_ = LevelManager();
					level_manager_.load_assets(data);
				}
				else
				{

					// Request level data for current level
					level_manager_.request_map_data(message.level_id_, message.size_x_, message.size_y_);
					network::NetworkMessageLevelDataRequest msg(message.event_id_);
					level_message_queue_.push(msg);
				}

				create_ack_message(message.event_id_);
			} break;

			case network::NETWORK_MESSAGE_LEVEL_DATA:
			{
				network::NetworkMessageLevelData message;
				if (!message.read(reader)) {
					assert(!"could not read message!");
				}
				//printf("RELIABLE MESSAGE: x: %i y: %i id: %i \n", message.x_, message.y_, message.level_tile_);

				level_manager_.create_tile(message.level_tile_, message.x_, message.y_);
				create_ack_message(message.event_id_);

				if (!level_manager_.is_done_sending())
				{
					network::NetworkMessageLevelDataRequest msg(message.event_id_);
					level_message_queue_.push(msg);
				}

			} break;

			default:
			{
				assert(!"unknown message type received from server!");
			} break;
			}
		}
	}

	void Game::on_send(network::Connection* connection, const uint16 sequence, network::NetworkStreamWriter& writer)
	{
		network::NetworkMessageInputCommand command(player_.input_bits_, player_.turret_transform_.rotation_, player_.fire_);
		if (!command.write(writer)) {
			assert(!"could not write network command!");
		}

		if (level_manager_.waiting_for_data())
		{
			while (!level_message_queue_.empty() && writer.length() < 1024 - sizeof(network::NetworkMessageLevelDataRequest))
			{
				network::NetworkMessageLevelDataRequest msg = level_message_queue_.front();
				if (!msg.write(writer)) {
					assert(!"could not write network command!");
				}
				level_message_queue_.pop();
				//printf("RELIABLE MESSAGE: Requesting next tile from server \n");
			}
		}

		while (!message_queue_.empty() && writer.length() < 1024 - sizeof(network::NetworkMessageAck))
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

	void Game::spawn_entity(network::NetworkMessageEntitySpawn message)
	{
		Entity e{};
		e.init(renderer_, message.position_, message.entity_id_);
		e.load_body_sprite(config::TANK_BODY_SPRITE, 0, 0, config::PLAYER_WIDTH, config::PLAYER_HEIGHT);
		e.load_turret_sprite(config::TANK_TURRET_SPRITE, 0, 0, config::PLAYER_WIDTH, config::PLAYER_HEIGHT);
		entities_.push_back(e);
		printf("RELIABLE MESSAGE: Remote player (id %i) spawned with message id: %i \n", message.entity_id_, message.event_id_);
	}

	void Game::spawn_player(network::NetworkMessagePlayerSpawn message)
	{
		player_ = Player();
		player_.init(renderer_, message.position_, message.entity_id_);
		player_.load_body_sprite(config::TANK_BODY_SPRITE, 0, 0, config::PLAYER_WIDTH, config::PLAYER_HEIGHT);
		player_.load_turret_sprite(config::TANK_TURRET_SPRITE, 0, 0, config::PLAYER_WIDTH, config::PLAYER_HEIGHT);
		printf("RELIABLE MESSAGE: Player (id %i) spawned with message id: %i \n", message.entity_id_, message.event_id_);
	}

	void Game::remove_entity(uint32 id)
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

	void Game::remove_projectile(uint32 id)
	{
		auto it = projectiles_.begin();
		while (it != projectiles_.end())
		{
			if ((*it).id_ == id)
			{
				(*it).destroy();
				projectiles_.erase(it);
				printf("RELIABLE MESSAGE: Projectile destroyed with id %i \n", id);
				break;
			}
			++it;
		}
	}

	void Game::spawn_projectile(network::NetworkMessageProjectileSpawn message)
	{
		Projectile e(message.pos_, message.rotation_, message.entity_id_, message.shot_by_);
		e.renderer_ = renderer_;
		e.load_sprite(config::TANK_SHELL, 0, 0, config::PROJECTILE_WIDTH, config::PROJECTILE_HEIGHT);
		projectiles_.push_back(e);
		printf("RELIABLE MESSAGE: Remote projectile: %i spawned with owner: %i \n", message.entity_id_, message.shot_by_);
	}

	bool Game::contains(const DynamicArray<Entity>& vector, uint32 id)
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

	bool Game::contains(const DynamicArray<Projectile>& vector, uint32 id)
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

	void Game::create_ack_message(uint32 event_id_)
	{
		network::NetworkMessageAck msg;
		msg.event_id_ = event_id_;
		message_queue_.push(msg);
		printf("RELIABLE MESSAGE: Message confirmation sent with id %i \n", msg.event_id_);
	}
}
