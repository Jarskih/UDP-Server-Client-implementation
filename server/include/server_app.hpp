// server_app.hpp

#ifndef SERVER_APP_HPP_INCLUDED
#define SERVER_APP_HPP_INCLUDED

#include <sdl_application.hpp>



#include "collision_handler.h"
#include "config.h"
#include "level_manager.h"
#include "projectile.h"
#include "reliable_events.h"

using namespace charlie;

struct ClientList {
	ClientList()
		: next_(0)
	{
	}

	int32 add_client(const uint64 connection)
	{
		const int32 id = next_++;
		clients_.push_back({ id, connection });
		return id;
	}

	int32 find_client(const uint64 connection)
	{
		for (auto& client : clients_) {
			if (client.connection_ == connection) {
				return client.id_;
			}
		}
		return -1;
	}

	void remove_client(const uint64 connection)
	{
		auto it = clients_.begin();
		while (it != clients_.end()) {
			if ((*it).connection_ == connection) {
				clients_.erase(it);
				break;
			}
			++it;
		}
	}

	struct Client {
		int32  id_{ -1 };
		uint64 connection_{};
	};

	int32 next_;
	DynamicArray<Client> clients_;
};

struct ServerApp final : SDLApplication, network::IServiceListener, network::IConnectionListener {
	ServerApp();

	// note: Application
	virtual bool on_init();
	virtual void on_exit();
	virtual bool on_tick(const Time& dt);
	virtual void on_draw();

	// note: IServiceListener
	virtual void on_timeout(network::Connection* connection);
	virtual void on_connect(network::Connection* connection);
	virtual void on_disconnect(network::Connection* connection);

	// note: IConnectionListener 
	virtual void on_acknowledge(network::Connection* connection, const uint16 sequence);
	virtual void on_receive(network::Connection* connection, network::NetworkStreamReader& reader);
	virtual void on_send(network::Connection* connection, const uint16 sequence, network::NetworkStreamWriter& writer);

	// note: gameplay
	void read_input_queue();
	void update_players(const Time& dt);
	void check_collisions();
	void remove_player(uint32 id);
	void spawn_projectile(Vector2 pos, float rotation, uint32 id);
	void remove_projectile(uint32 id);
	void create_spawn_event(const uint32 owner, const Player& p, EventType event);
	void create_destroy_event(uint32 id, EventType event);
	static void write_message(const Event& reliable_event, network::NetworkStreamWriter& writer);

	static void remove_from_array(DynamicArray<Event>& arr, uint32 id);
	static bool contains(const DynamicArray<uint32>& event, uint32 id);

	// note: Network
	const Time tickrate_;
	Time accumulator_;
	uint32 tick_;
	ClientList clients_;
	gameplay::ReliableMessageQueue reliable_queue_;
	DynamicArray<Event> spawn_event_list;
	DynamicArray<Event> destroy_event_list_;
	DynamicArray<Event> reliable_events_;
	Queue<gameplay::InputCommand> input_queue_;

	// note: gameplay
	uint32 index_; // index keeping track of joined players
	uint32 projectile_index_;
	DynamicArray<Player> players_;
	DynamicArray<uint32> players_to_remove_;
	DynamicArray<Projectile> projectiles_;
	DynamicArray<uint32> projectiles_to_remove_;
	LevelManager level_manager_;
	Random random_;
};

#endif // !SERVER_APP_HPP_INCLUDED
