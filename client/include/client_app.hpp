// client_app.hpp

#ifndef CLIENT_APP_HPP_INCLUDED
#define CLIENT_APP_HPP_INCLUDED

#include <sdl_application.hpp>



#include "entity.h"
#include "level_manager.h"
#include "player.hpp"
#include "projectile.h"
#include "sdl_text_handler.h"

using namespace charlie;

struct ClientApp final : SDLApplication, network::IConnectionListener {
	ClientApp();

	// note: Application
	virtual bool on_init();
	virtual void on_exit();
	virtual bool on_tick(const Time& dt);
	virtual void on_draw();

	// note: IConnectionListener 
	virtual void on_acknowledge(network::Connection* connection, const uint16 sequence);
	virtual void on_receive(network::Connection* connection, network::NetworkStreamReader& reader);
	virtual void on_send(network::Connection* connection, const uint16 sequence, network::NetworkStreamWriter& writer);
	void spawn_entity(network::NetworkMessagePlayerSpawn message);
	void remove_entity(uint32 id);
	void remove_projectile(uint32 id);
	void spawn_projectile(network::NetworkMessageProjectileSpawn message);
	void destroy_projectile(const network::NetworkMessageProjectileSpawn& message);
	void spawn_local_projectile(Vector2 pos, float rotation);

	// Networking
	network::Connection connection_;
	const Time tickrate_;
	Time accumulator_;
	Time lastSend_;
	Time lastReceive_;
	Queue<network::NetworkMessageAck> spawn_message_queue_;
	gameplay::Inputinator inputinator_;
	Networkinfo networkinfo_;
	Vector2 oldPos_;
	Vector2 newPos_;
	uint32 tick_;
	uint32 server_tick_;
	Time server_time_;

	// Gameplay
	Player player_;
	DynamicArray <Entity> entities_;
	DynamicArray<uint32> entities_to_remove_;
	DynamicArray<Projectile> projectiles_;
	DynamicArray<Projectile> local_projectiles_;
	DynamicArray<uint32> projectiles_to_remove_;
	LevelManager level_manager_;
	TextHandler text_handler_;
	SDLFont text_font_;
	uint32 local_projectile_index_;
};

#endif // !CLIENT_APP_HPP_INCLUDED
