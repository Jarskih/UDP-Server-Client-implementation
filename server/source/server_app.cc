// server_app.cc

#include "server_app.hpp"
#include <charlie_messages.hpp>
#include <cstdio>
#include <cmath>

ServerApp::ServerApp()
	: tickrate_(1.0 / 60.0)
	, tick_(0), reliable_queue_(), index_(0)
{
}

bool ServerApp::on_init()
{
	network_.set_send_rate(Time(1.0 / 10.0));
	network_.set_allow_connections(true);
	if (!network_.initialize(network::IPAddress(network::IPAddress::ANY_HOST, 54345))) {
		return false;
	}

	network_.add_service_listener(this);

	auto data = Leveldata();
	data.create_level("../assets/map.txt");
	level_manager_ = LevelManager();
	level_manager_.load_assets(data);

	LEVEL_WIDTH = level_manager_.width_;
	LEVEL_HEIGHT = level_manager_.height_;

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


			if ((player.transform_.position_.x_ < 0) || (player.transform_.position_.x_ + (float)player.body_sprite_->get_area().w > LEVEL_WIDTH))
			{
				player.transform_.position_ -= player.transform_.forward() * direction * player.speed_ * dt.as_seconds();
			}

			if ((player.transform_.position_.y_ < 0) || (player.transform_.position_.y_ + (float)player.body_sprite_->get_area().h > LEVEL_HEIGHT))
			{
				player.transform_.position_ -= player.transform_.forward() * direction * player.speed_ * dt.as_seconds();
			}
			

				//Center the camera over the dot
                cam_.x = (int)player.transform_.position_.x_ + player.body_sprite_->get_area().w / 2 - SCREEN_WIDTH / 2;
                cam_.y = (int)player.transform_.position_.y_ + player.body_sprite_->get_area().h / 2 - SCREEN_HEIGHT / 2;

                //Keep the camera in bounds
                if( cam_.x < 0 )
                { 
                    cam_.x = 0;
                }
                if( cam_.y < 0 )
                {
                    cam_.y = 0;
                }
                if( cam_.x > LEVEL_WIDTH - cam_.w )
                {
                    cam_.x = LEVEL_WIDTH - cam_.w;
                }
                if( cam_.y > LEVEL_HEIGHT - cam_.h )
                {
                    cam_.y = LEVEL_HEIGHT - cam_.h;
                }
		}
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
}

void ServerApp::on_timeout(network::Connection* connection)
{
	connection->set_listener(nullptr);
	const uint32 id = clients_.find_client((uint64)connection);

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
	clients_.remove_client((uint64)connection);
}

void ServerApp::on_connect(network::Connection* connection)
{
	connection->set_listener(this);

	auto id = clients_.add_client((uint64)connection);
	// event : "player_connected"
	Player player;
	player.id_ = id;

	Vector2 pos = Vector2(20.0f + random_() % 200, 200.0f + random_() % 100);
	player.init(renderer_.get_renderer(), pos, index_);
	player.load_body_sprite("../assets/tank_body.png", 0, 0, 50, 76);
	player.load_turret_sprite("../assets/tank_turret.png", 0, 0, 30, 45);

	players_.push_back(player);
	playersToSpawn_.push_back(player);

	event_list_.id_ = index_;
	event_list_.event_.push_back(player.id_);

	index_++;
}

void ServerApp::on_disconnect(network::Connection* connection)
{
	connection->set_listener(nullptr);

	const uint32 id = clients_.find_client((uint64)connection);
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

	clients_.remove_client((uint64)connection);
}

void ServerApp::on_acknowledge(network::Connection* connection,
	const uint16 sequence)
{
	// find if queue contains sequence
	if (sequence == reliable_queue_.seq_)
	{
		// remove all events with id
	}
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
				player.input_bits_ = command.bits_;
				break;
			}
		}
	}
}

void ServerApp::on_send(network::Connection* connection,
	const uint16 sequence,
	network::NetworkStreamWriter& writer)
{
	reliable_queue_.seq_ = sequence;
	reliable_queue_.id_ = event_list_.id_;

	const uint32 id = clients_.find_client((uint64)connection);

	{
		network::NetworkMessageServerTick message(Time::now().as_ticks(), tick_);
		if (!message.write(writer)) {
			assert(!"failed to write message!");
		}
	}

	{
		// for each item in the queue send a packet
		for (auto& player : playersToSpawn_)
		{
			if (player.id_ != id)
			{
				network::NetworkMessagePlayerSpawn message(player.transform_.position_, player.id_);
				if (!message.write(writer)) {
					assert(!"failed to write message!");
				}
				// add player id and sequence to queue
			}
		}
	}

	{
		for (auto& player : players_)
		{
			if (player.id_ == id)
			{
				network::NetworkMessagePlayerState message(player.transform_);
				if (!message.write(writer)) {
					assert(!"failed to write message!");
				}
				continue;
			}

			network::NetworkMessageEntityState message(player.transform_, player.id_);
			if (!message.write(writer)) {
				assert(!"failed to write message!");
			}
		}
	}
}
