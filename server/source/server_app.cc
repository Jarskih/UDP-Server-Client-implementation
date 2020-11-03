// server_app.cc

#include "server_app.hpp"
#include <charlie_messages.hpp>
#include <cstdio>
#include <cmath>
#include "collision_handler.h"
#include "config.h"
#include "reliable_events.h"

ServerApp::ServerApp()
	: tickrate_(1.0 / 60.0)
	, tick_(0)
	, index_(0)
	, projectile_index_(0)
{
}

bool ServerApp::on_init()
{
	network_.set_send_rate(Time(1.0 / 20.0));
	network_.set_allow_connections(true);
	if (!network_.initialize(network::IPAddress(network::IPAddress::ANY_HOST, 54345))) {
		return false;
	}

	network_.add_service_listener(this);

	auto data = Leveldata();
	data.create_level(config::LEVEL1);
	level_manager_ = LevelManager();
	level_manager_.load_assets(data);

	level_width_ = level_manager_.width_;
	level_heigth_ = level_manager_.height_;

	return true;
}

void ServerApp::on_exit()
{
}

bool ServerApp::on_tick(const Time& dt)
{
	input_handler_.HandleEvents();
	if (input_handler_.IsKeyDown(SDL_SCANCODE_ESCAPE))
	{
		return false;
	}

	accumulator_ += dt;
	while (accumulator_ >= tickrate_) {
		accumulator_ -= tickrate_;
		tick_++;

		read_input_queue();
		update_players(dt);

		for (auto& projectile : projectiles_)
		{
			projectile.update(dt);
		}

		check_collisions();

		for (auto& id : players_to_remove_)
		{
			remove_player(id);
		}

		for (auto& projectile : projectiles_)
		{
			if (projectile.is_dead())
			{
				reliable_events_.create_destroy_event(projectile.id_, EventType::DESTROY_PROJECTILE, players_);
				projectiles_to_remove_.push_back(projectile.id_);
			}
		}

		for (auto& id : projectiles_to_remove_)
		{
			remove_projectile(id);
		}
	}

	return true;
}

void ServerApp::on_draw()
{
	char myString[10] = "";
	sprintf_s(myString, "%ld", long(tick_));

	for (auto& player : players_)
	{
		player.render(cam_.rect_);
	}

	for (auto& projectile : projectiles_)
	{
		projectile.render(cam_.rect_);
	}
}

void ServerApp::on_timeout(network::Connection* connection)
{
	connection->set_listener(nullptr);
	const uint32 id = clients_.find_client((uint64)connection);

	players_to_remove_.push_back(id);
	reliable_events_.create_destroy_event(id, EventType::DESTROY_PLAYER, players_);

	clients_.remove_client((uint64)connection);
	printf("NETWORK: Player %i disconnected. Players %i \n", id, (int)clients_.clients_.size());
}

void ServerApp::on_connect(network::Connection* connection)
{
	connection->set_listener(this);

	const auto id = clients_.add_client((uint64)connection);

	Player player;
	player.id_ = id;

	Vector2 pos = Vector2(20.0f + random_() % 200, 200.0f + random_() % 100);
	player.init(renderer_.get_renderer(), pos, index_);
	player.load_body_sprite(config::TANK_BODY_SPRITE, 0, 0, config::PLAYER_WIDTH, config::PLAYER_HEIGHT);
	player.load_turret_sprite(config::TANK_TURRET_SPRITE, 0, 0, config::PLAYER_WIDTH, config::PLAYER_HEIGHT);

	for (const Player& p : players_)
	{
		reliable_events_.create_spawn_event(player.id_, p, EventType::SPAWN_PLAYER, players_);
		reliable_events_.create_spawn_event(p.id_, player, EventType::SPAWN_PLAYER, players_);
	}

	players_.push_back(player);
	index_ += 1;

	printf("NETWORK: Player joined. players: %i\n", (int)clients_.clients_.size());
}

void ServerApp::on_disconnect(network::Connection* connection)
{
	connection->set_listener(nullptr);

	const uint32 id = clients_.find_client((uint64)connection);

	players_to_remove_.push_back(id);

	reliable_events_.create_destroy_event(id, EventType::DESTROY_PLAYER, players_);

	clients_.remove_client((uint64)connection);

	printf("NETWORK: Player disconnected. players: %i\n", (int)clients_.clients_.size());
}

void ServerApp::on_acknowledge(network::Connection* connection,
	const uint16 sequence)
{
}

void ServerApp::on_receive(network::Connection* connection,
	network::NetworkStreamReader& reader)
{
	const uint32 id = clients_.find_client((uint64)connection);

	while (reader.position() < reader.length()) {
		switch (reader.peek()) {
		case(network::NETWORK_MESSAGE_INPUT_COMMAND):
		{
			network::NetworkMessageInputCommand command;
			if (!command.read(reader)) {
				assert(!"could not read command!");
			}

			for (auto& player : players_) {
				if (player.id_ == id) {
					gameplay::InputCommand cmd{};
					cmd.id_ = id;
					cmd.input_bits_ = command.bits_;
					cmd.rot_ = command.rot_;
					cmd.fire_ = command.fire_;
					input_queue_.push(cmd);
					break;
				}
			}
		} break;

		case(network::NETWORK_MESSAGE_ACK):
		{
			network::NetworkMessageAck msg;
			if (!msg.read(reader)) {
				assert(!"could not read command!");
			}

			reliable_queue_.mark_received(msg.event_id_);
		} break;

		default:
		{
			printf("Unknown message %i", (int)reader.peek());
			assert(!"unknown message type received from client!");
		} break;
		}
	}
}

void ServerApp::on_send(network::Connection* connection,
	const uint16 sequence,
	network::NetworkStreamWriter& writer)
{
	const uint32 id = clients_.find_client((uint64)connection);

	{
		network::NetworkMessageServerTick message(Time::now().as_ticks(), tick_);
		if (!message.write(writer)) {
			assert(!"failed to write message!");
		}
	}

	// Send reliable messages
	{
		// Remove confirmed messages
		for (auto& msg : reliable_queue_.buffer_)
		{
			if (msg.received_)
			{
				remove_from_array(reliable_events_.events_, msg.id_);
			}
		}

		// Create message from event
		for (Event reliable_event : reliable_events_.events_)
		{
			if (id == reliable_event.send_to_)
			{
				gameplay::Message msg{};
				msg.id_ = reliable_event.event_id_;
				msg.seq_ = sequence;
				msg.received_ = false;

				// Keep track of sent reliable messages
				reliable_queue_.add_message(msg);
				write_message(reliable_event, writer);
				printf("RELIABLE MESSAGE: Sent message with id %i \n", (int)reliable_event.event_id_);
			}
		}
	}

	{
		// Send player update and entity updates to one player
		for (auto& player : players_)
		{
			if (player.id_ == id)
			{
				network::NetworkMessagePlayerState message(player.transform_, player.turret_rotation_);
				if (!message.write(writer)) {
					assert(!"failed to write message!");
				}
				continue;
			}

			network::NetworkMessageEntityState message(player.transform_, player.turret_rotation_, player.id_);
			if (!message.write(writer)) {
				assert(!"failed to write message!");
			}
		}
	}
}

void ServerApp::write_message(const Event& reliable_event, network::NetworkStreamWriter& writer)
{
	switch (reliable_event.type_)
	{
	case(EventType::SPAWN_PLAYER):
	{
		network::NetworkMessagePlayerSpawn message(reliable_event.event_id_, reliable_event.entity_id_, reliable_event.pos_);
		if (!message.write(writer))
		{
			assert(!"failed to write message!");
		}
	} break;

	case(EventType::SPAWN_PROJECTILE):
	{
		network::NetworkMessageProjectileSpawn message(reliable_event.event_id_, reliable_event.entity_id_, reliable_event.creator_, reliable_event.pos_, reliable_event.rot_);
		if (!message.write(writer))
		{
			assert(!"failed to write message!");
		}
	} break;

	case(EventType::DESTROY_PLAYER):
	{
		network::NetworkMessagePlayerDisconnected message(reliable_event.entity_id_, reliable_event.event_id_);
		if (!message.write(writer))
		{
			assert(!"failed to write message!");
		}
	} break;

	case(EventType::DESTROY_PROJECTILE):
	{
		network::NetworkMessageProjectileDestroy message(reliable_event.entity_id_, reliable_event.event_id_);
		if (!message.write(writer))
		{
			assert(!"failed to write message!");
		}
	} break;

	default:
		assert(!"Unknown event type");
		break;
	}
}


void ServerApp::read_input_queue()
{
	while (!input_queue_.empty())
	{
		const gameplay::InputCommand cmd = input_queue_.front();
		input_queue_.pop();

		for (auto& player : players_)
		{
			if (player.id_ == cmd.id_)
			{
				player.input_bits_ = cmd.input_bits_;
				player.turret_rotation_ = cmd.rot_;
				player.fire_ = cmd.fire_;
				break;
			}
		}
	}
}

void ServerApp::update_players(const Time& dt)
{
	for (auto& player : players_)
	{
		player.old_pos_ = player.transform_.position_;

		float direction = 0;
		float rotation = 0;

		const bool player_move_up = player.get_input_bits() & (1 << int32(gameplay::Action::Up));
		const bool player_move_down = player.get_input_bits() & (1 << int32(gameplay::Action::Down));
		const bool player_move_left = player.get_input_bits() & (1 << int32(gameplay::Action::Left));
		const bool player_move_right = player.get_input_bits() & (1 << int32(gameplay::Action::Right));

		if (player_move_up) {
			direction -= 1.0f;
		}
		if (player_move_down) {
			direction += 1.0f;
		}
		if (player_move_left) {
			rotation -= 1.0f;
		}
		if (player_move_right) {
			rotation += 1.0f;
		}

		if (abs(rotation) > 0.0f)
		{
			const float rot = player.transform_.rotation_ + rotation * player.tank_turn_speed_ * dt.as_seconds();
			player.transform_.set_rotation(rot);
		}

		if (abs(direction) > 0.0f) {
			player.transform_.position_ += player.transform_.forward() * direction * player.speed_ * dt.as_seconds();
		}

		if ((player.transform_.position_.x_ < 0) || (player.transform_.position_.x_ + (float)player.body_sprite_->get_area().w) > level_width_)
		{
			player.transform_.position_ -= player.transform_.forward() * direction * player.speed_ * dt.as_seconds();
		}

		if ((player.transform_.position_.y_ < 0) || (player.transform_.position_.y_ + (float)player.body_sprite_->get_area().h) > level_heigth_)
		{
			player.transform_.position_ -= player.transform_.forward() * direction * player.speed_ * dt.as_seconds();
		}

		player.collider_.SetPosition(player.get_collider_pos());

		cam_.lookAt(player);

		player.fire_acc_ += dt;
		if (player.fire_ && player.can_shoot())
		{
			spawn_projectile(player.get_shoot_pos(), player.turret_rotation_, player.id_);
			player.fire();
			reliable_events_.create_spawn_event(projectile_index_, player, EventType::SPAWN_PROJECTILE, players_);
			projectile_index_ += 1;
		}
	}
}

void ServerApp::check_collisions()
{
	for (auto p1 = 0; p1 < (int)players_.size(); p1++)
	{
		for (int projectile = 0; projectile < (int)projectiles_.size(); projectile++)
		{
			// Cant collide with own projectiles
			if (projectiles_[projectile].owner_ == players_[p1].id_)
			{
				continue;
			}

			if (CollisionHandler::IsColliding(players_[p1].collider_, projectiles_[projectile].collider_))
			{
				players_[p1].on_collision(projectiles_[projectile]);
				projectiles_[projectile].on_collision();
				reliable_events_.create_destroy_event(projectiles_[projectile].id_, EventType::DESTROY_PROJECTILE, players_);
				projectiles_to_remove_.push_back(projectiles_[projectile].id_);
				//printf("COLLISION: Projectile collided with player \n");
			}
		}

		for (int p2 = 0; p2 < (int)players_.size(); p2++)
		{
			if (p2 == p1)
			{
				continue;
			}
			if (CollisionHandler::IsColliding(players_[p2].collider_, players_[p1].collider_))
			{
				players_[p1].on_collision(players_[p2]);
				players_[p2].on_collision(players_[p1]);
				//printf("COLLISION: Player collided with player \n");
			}
		}

		for (int collider = 0; collider < (int)level_manager_.colliders_.size(); collider++)
		{
			if (CollisionHandler::IsColliding(players_[p1].collider_, level_manager_.colliders_[collider].collider_))
			{
				players_[p1].on_collision(level_manager_.colliders_[collider]);
				// printf("COLLISION: Player collided with terrain \n");
			}
		}
	}

	for (auto& p : projectiles_)
	{
		for (auto& obj : level_manager_.colliders_)
		{
			if (CollisionHandler::IsColliding(obj.collider_, p.collider_))
			{
				p.on_collision();
				printf("COLLISION: Projectile collided with terrain \n");
				reliable_events_.create_destroy_event(p.id_, EventType::DESTROY_PROJECTILE, players_);
				projectiles_to_remove_.push_back(p.id_);
			}
		}
	}
}

void ServerApp::remove_player(const uint32 id)
{
	auto it = players_.begin();
	while (it != players_.end())
	{
		if ((*it).id_ == id)
		{
			(*it).destroy();
			players_.erase(it);
			break;
		}
		++it;
	}
}

void ServerApp::spawn_projectile(const Vector2 pos, const float rotation, const uint32 id)
{
	Projectile e(pos, rotation, projectile_index_, id);
	e.renderer_ = renderer_.get_renderer();
	e.load_sprite(config::TANK_SHELL, 0, 0, config::PROJECTILE_WIDTH, config::PROJECTILE_HEIGHT);
	projectiles_.push_back(e);
}

void ServerApp::remove_projectile(uint32 id)
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

bool ServerApp::contains(const DynamicArray<uint32>& arr, const uint32 id)
{
	return std::find(arr.begin(), arr.end(), id) != arr.end();
}

void ServerApp::remove_from_array(DynamicArray<Event>& arr, uint32 id)
{
	auto it = arr.begin();
	while (it != arr.end())
	{
		if ((*it).event_id_ == id)
		{
			arr.erase(it);
			break;
		}
		++it;
	}
}
