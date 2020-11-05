// server_app.hpp

#ifndef SERVER_APP_HPP_INCLUDED
#define SERVER_APP_HPP_INCLUDED

#include <sdl_application.hpp>
#include "ClientList.h"
#include "level_manager.h"
#include "projectile.h"
#include "reliable_events.h"
#include "server_register.h"

using namespace charlie;

struct ServerApp final : SDLApplication, network::IServiceListener, network::IConnectionListener {
	ServerApp();

	static int register_server(network::IPAddress address, std::string message);

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

	void write_message(const Event& reliable_event, network::NetworkStreamWriter& writer) const;

	// note: gameplay
	void read_input_queue();
	void update_players(const Time& dt);
	void destroy_projectile(uint32 id);
	void destroy_player(uint32 id);
	void check_collisions();
	void remove_player(uint32 id);
	void spawn_projectile(Vector2 pos, float rotation, uint32 id);
	void remove_projectile(uint32 id);

	static void remove_from_array(DynamicArray<Event>& arr, uint32 id);
	static bool contains(const DynamicArray<uint32>& arr, uint32 id);

	// note: Network
	ServerRegister server_register_;
	network::Connection connection_;
	const Time tickrate_;
	Time accumulator_;
	uint32 tick_;
	ClientList clients_;
	gameplay::ReliableMessageQueue reliable_queue_;
	DynamicArray<Event> spawn_event_list;
	DynamicArray<Event> destroy_event_list_;
	ReliableEvents reliable_events_;
	Queue<gameplay::InputCommand> input_queue_;

	// note: gameplay
	Camera cam_;
	uint32 index_; // index keeping track of joined players
	uint32 projectile_index_;
	DynamicArray<Player> players_;
	DynamicArray<uint32> players_to_remove_;
	DynamicArray<Projectile> projectiles_;
	DynamicArray<uint32> projectiles_to_remove_;
	LevelManager level_manager_;
	Random random_;
	uint8 current_map_;
};

#endif // !SERVER_APP_HPP_INCLUDED
