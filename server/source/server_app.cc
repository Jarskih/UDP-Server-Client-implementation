// server_app.cc

#include "server_app.hpp"
#include <charlie_messages.hpp>
#include <cstdio>
#include <cmath>

#include "config.h"

ServerApp::ServerApp()
	: tickrate_(1.0 / 60.0)
	, tick_(0)
	, reliable_queue_()
	, index_(0)
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
	data.create_level("../assets/map.txt");
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


		while (!input_queue_.empty())
		{
			const InputCommand cmd = input_queue_.front();
			input_queue_.pop();

			for (auto& player : players_)
			{
				if(player.id_ == cmd.id_)
				{
					player.input_bits_ = cmd.input_bits_;
					player.turret_rotation_ = cmd.rot_;
					player.fire_ = cmd.fire_;
					break;
				}	
			}
		}

		for (auto& player : players_)
		{
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


			if ((player.transform_.position_.x_ < 0) || (player.transform_.position_.x_ + (float)player.body_sprite_->get_area().w > level_width_))
			{
				player.transform_.position_ -= player.transform_.forward() * direction * player.speed_ * dt.as_seconds();
			}

			if ((player.transform_.position_.y_ < 0) || (player.transform_.position_.y_ + (float)player.body_sprite_->get_area().h > level_heigth_))
			{
				player.transform_.position_ -= player.transform_.forward() * direction * player.speed_ * dt.as_seconds();
			}

			{
				cam_.x = (int)player.transform_.position_.x_ + player.body_sprite_->get_area().w / 2 - config::SCREEN_WIDTH / 2;
				cam_.y = (int)player.transform_.position_.y_ + player.body_sprite_->get_area().h / 2 - config::SCREEN_HEIGHT / 2;

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

			player.fire_acc_ += dt;
			if(player.fire_ && player.can_shoot())
			{
				spawn_projectile(player.get_shoot_pos(), player.turret_rotation_, player.id_);
				player.fire();
			}
		}
	}

	for (auto& projectile : projectiles_)
	{
		projectile.update(dt);
	}

	for (auto& id : players_to_remove_)
	{
		remove_player(id);
	}

	return true;
}

void ServerApp::on_draw()
{
	//renderer_.clear({ 0.4f, 0.3f, 0.2f, 1.0f });
	//renderer_.render_text({ 2, 2 }, Color::White, 2, "SERVER");

	char myString[10] = "";
	sprintf_s(myString, "%ld", long(tick_));
	//renderer_.render_text({ 150, 2 }, Color::White, 1, myString);

	for (auto& player : players_)
	{
		player.render(cam_);
	}

	for (auto& projectile : projectiles_)
	{
		projectile.render(cam_);
	}
}

void ServerApp::on_timeout(network::Connection* connection)
{
	connection->set_listener(nullptr);
	const uint32 id = clients_.find_client((uint64)connection);

	players_to_remove_.push_back(id);
		
	clients_.remove_client((uint64)connection);
	printf("Player %i disconnected. Players %i \n", id, (int)clients_.clients_.size());
}

void ServerApp::on_connect(network::Connection* connection)
{
	connection->set_listener(this);

	const auto id = clients_.add_client((uint64)connection);
	// event : "player_connected"
	Player player;
	player.id_ = id;

	Vector2 pos = Vector2(20.0f + random_() % 200, 200.0f + random_() % 100);
	player.init(renderer_.get_renderer(), pos, index_);
	player.load_body_sprite("../assets/tank_body.png", 0, 0, config::PLAYER_WIDTH, config::PLAYER_HEIGHT);
	player.load_turret_sprite("../assets/tank_turret.png", 0, 0, config::PLAYER_WIDTH, config::PLAYER_HEIGHT);

	for (const Player& p : players_)
	{
		{
		// inform all existing players of new player
		Event spawn_event;
		spawn_event.id_ = player.id_; // player to spawn
		spawn_event.send_to_ = p.id_; // player to receive the event
		spawn_event_list.push_back(spawn_event);
		}
		{
		// inform joining player of others
		Event spawn_event;
		spawn_event.id_ = p.id_;
		spawn_event.send_to_ = player.id_;
		spawn_event_list.push_back(spawn_event);
		}
	}
	
	players_.push_back(player);
	index_ += 1;

	printf("player joined. players: %i\n", (int)clients_.clients_.size());
}

void ServerApp::on_disconnect(network::Connection* connection)
{
	connection->set_listener(nullptr);

	const uint32 id = clients_.find_client((uint64)connection);
	
	players_to_remove_.push_back(id);

	// inform of others that player disconnected
	for (Player p : players_)
	{
		Event destroy_event_;
		destroy_event_.id_ = id;
		destroy_event_.send_to_ = p.id_;
		destroy_event_list_.push_back(destroy_event_);
	}

	clients_.remove_client((uint64)connection);

	printf("player disconnected. players: %i\n", (int)clients_.clients_.size());
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
		if (reader.peek() != network::NETWORK_MESSAGE_INPUT_COMMAND) {
			break;
		}

		network::NetworkMessageInputCommand command;
		if (!command.read(reader)) {
			assert(!"could not read command!");
		}

		for (auto& player : players_) {
			if (player.id_ == id) {
				InputCommand cmd{};
				cmd.id_ = id;
				cmd.input_bits_ = command.bits_;
				cmd.rot_ = command.rot_;
				cmd.fire_ = command.fire_;
				if(cmd.fire_)
				{
					printf("player fire command received \n");
				}
				input_queue_.push(cmd);
				break;
			}
		}
	}

	while (reader.position() < reader.length()) {
		if (reader.peek() != network::NETWORK_MESSAGE_PLAYER_SPAWN_ACK) {
			break;
		}

		network::NetworkMessagePlayerSpawnAck msg;
		if (!msg.read(reader)) {
			assert(!"could not read command!");
		}

		auto message = reliable_queue_.get_message(connection->acknowledge_);
		if(message.seq_ != 0)
		{
			printf("Spawn command %i acknowledged on sequence %i \n", msg.id_, message.seq_);
			remove_from_array(spawn_event_list, message.id_);
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

	{
		for (auto& event : spawn_event_list)
		{
			for (auto p : players_)
			{
				if (p.id_ == id)
				{
					continue;
				}
				if (p.id_ == event.id_)
				{
					gameplay::Message spawnMessage{};
					spawnMessage.id_ = event.id_; // player to spawn
					spawnMessage.seq_ = sequence;
					reliable_queue_.add_message(tick_, spawnMessage);

					network::NetworkMessagePlayerSpawn message(p.transform_.position_, p.id_);
					if (!message.write(writer)) {
						assert(!"failed to write message!");
					}

					//printf("Server sent request to: %i to spawn %i on sequence %i\n", id, event.id_, sequence);
					break;
				}
			}
		}
	}

	{
		for (auto destroy_event : destroy_event_list_)
		{
			// TODO Create destroy message
			
		}
	}

	{
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

void ServerApp::remove_player(uint32 id)
{
	auto it = players_.begin();
	while (it != players_.end())
	{
		if ((*it).id_ == id)
		{
			players_.erase(it);
			break;
		}
		++it;
	}
}

void ServerApp::spawn_projectile(Vector2 pos, float rotation, uint32 id)
{
	Projectile e(pos, rotation, id);
	e.renderer_ = renderer_.get_renderer();
	e.load_sprite("../assets/Light_Shell.png", 0, 0, config::PROJECTILE_WIDTH, config::PROJECTILE_HEIGHT);
	projectiles_.push_back(e);
	printf("Spawned projectile \n");
}

bool ServerApp::contains(const DynamicArray<uint32>& arr, uint32 id)
{
	return std::find(arr.begin(), arr.end(), id) != arr.end();
}

void ServerApp::remove_from_array(DynamicArray<Event>& arr, uint32 id)
{
	auto it = arr.begin();
	while (it != arr.end())
	{
		if ((*it).id_ == id)
		{
			arr.erase(it);
			break;
		}
		++it;
	}
}
