// server_app.cc

#include "server_app.hpp"
#include <charlie_messages.hpp>
#include <cstdio>
#include <cmath>

ServerApp::ServerApp()
	: tickrate_(1.0 / 60.0)
	  , tick_(0), reliable_queue_(), index_(0), event_list_()
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

	return true;
}

void ServerApp::on_exit()
{
}

bool ServerApp::on_tick(const Time& dt)
{
	accumulator_ += dt;
	while (accumulator_ >= tickrate_) {
		accumulator_ -= tickrate_;
		tick_++;

		for (auto& player : players_)
		{
			// note: update player
			const bool player_move_up = player.get_input_bits() & (1 << int32(gameplay::Action::Up));
			const bool player_move_down = player.get_input_bits() & (1 << int32(gameplay::Action::Down));
			const bool player_move_left = player.get_input_bits() & (1 << int32(gameplay::Action::Left));
			const bool player_move_right = player.get_input_bits() & (1 << int32(gameplay::Action::Right));

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
				player.position_ += direction * player.speed_ * tickrate_.as_seconds();
			}
		}
	}

	return true;
}

void ServerApp::on_draw()
{
	renderer_.clear({ 0.4f, 0.3f, 0.2f, 1.0f });
	renderer_.render_text({ 2, 2 }, Color::White, 2, "SERVER");
	char myString[10] = "";
	sprintf_s(myString, "%ld", long(tick_));
	renderer_.render_text({ 150, 2 }, Color::White, 1, myString);


	for (auto& player : players_)
	{
		renderer_.render_rectangle_fill({ static_cast<int32>(player.position_.x_), static_cast<int32>(player.position_.y_),  20, 20 }, Color::Magenta);
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
	gameplay::Player player;
	player.id_ = id;
	player.position_.x_ = 20.0f + random_() % 200;
	player.position_.y_ = 200.0f + random_() % 100;
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
	if(sequence == reliable_queue_.seq_)
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
		for (auto& players : players_)
		{
			for (auto& player : playersToSpawn_)
			{
				if (player.id_ != id)
				{
					network::NetworkMessagePlayerSpawn message(player.position_, player.id_);
					if (!message.write(writer)) {
						assert(!"failed to write message!");
					}
					// add player id and sequence to queue
				}
			}
		}
	}

	{
		for (auto& player : players_)
		{
			if (player.id_ == id)
			{
				network::NetworkMessagePlayerState message(player.position_);
				if (!message.write(writer)) {
					assert(!"failed to write message!");
				}
				continue;
			}

			network::NetworkMessageEntityState message(player.position_, player.id_);
			if (!message.write(writer)) {
				assert(!"failed to write message!");
			}
		}
	}
}
