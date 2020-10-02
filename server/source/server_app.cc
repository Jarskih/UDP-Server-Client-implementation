// server_app.cc

#include "server_app.hpp"
#include <charlie_messages.hpp>
#include <cstdio>
#include <cmath>

ServerApp::ServerApp()
	: tickrate_(1.0 / 60.0)
	, tick_(0)
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

	player_.position_ = { 300.0f, 180.0f };
	player_.id_ = 1000;
	playersToSpawn_.push_back(player_);

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

		player_.position_.x_ = 300.0f + std::cosf(Time::now().as_seconds()) * 150.0f;
		player_.position_.y_ = 180.0f + std::sinf(Time::now().as_seconds()) * 100.0f;
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
	renderer_.render_rectangle_fill({ static_cast<int32>(send_position_.x_), static_cast<int32>(send_position_.y_),  20, 20 }, Color::Yellow);
	renderer_.render_rectangle_fill({ static_cast<int32>(player_.position_.x_), static_cast<int32>(player_.position_.y_),  20, 20 }, Color::Red);


	for (auto& player : players_)
	{
		renderer_.render_rectangle_fill({ static_cast<int32>(player.position_.x_), static_cast<int32>(player.position_.y_),  20, 20 }, Color::Magenta);
	}
}

void ServerApp::on_timeout(network::Connection* connection)
{
	connection->set_listener(nullptr);
	auto id = clients_.find_client((uint64)connection);
	// ...
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
}

void ServerApp::on_disconnect(network::Connection* connection)
{
	connection->set_listener(nullptr);
	auto id = clients_.find_client((uint64)connection);
	// ...
	clients_.remove_client((uint64)connection);
}

void ServerApp::on_acknowledge(network::Connection* connection,
	const uint16 sequence)
{

}

void ServerApp::on_receive(network::Connection* connection,
	network::NetworkStreamReader& reader)
{
	auto id = clients_.find_client((uint64)connection);

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
	auto id = clients_.find_client((uint64)connection);

	{
		network::NetworkMessageServerTick message(Time::now().as_ticks(), tick_);
		if (!message.write(writer)) {
			assert(!"failed to write message!");
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

		network::NetworkMessageEntityState message(player_.position_, player_.id_);
		if (!message.write(writer)) {
			assert(!"failed to write message!");
		}
	}

	{
		for (auto& player : playersToSpawn_)
		{
			if (player.id_ != id)
			{
				network::NetworkMessagePlayerSpawn message(player.position_, player.id_);
				if (!message.write(writer)) {
					assert(!"failed to write message!");
				}
			}
		}
		playersToSpawn_.clear();
	}
}
