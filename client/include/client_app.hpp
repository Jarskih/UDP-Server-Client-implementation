// client_app.hpp

#ifndef CLIENT_APP_HPP_INCLUDED
#define CLIENT_APP_HPP_INCLUDED

#include <sdl_application.hpp>



#include "entity.h"
#include "level_manager.h"
#include "player.hpp"
#include "sprite_handler.hpp"

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

	network::Connection connection_;
	const Time tickrate_;
	Time accumulator_;
	Time lastSend_;
	Time lastReceive_;

	Player player_;
	DynamicArray <Entity> entities_;
	LevelManager level_manager_;
	Queue<network::NetworkMessagePlayerSpawnAck> spawn_message_queue_;

	gameplay::Inputinator inputinator_;
	Networkinfo networkinfo_;
	Vector2 oldPos_;
	Vector2 newPos_;
	uint32 tick_;
	uint32 server_tick_;
	Time server_time_;
};

#endif // !CLIENT_APP_HPP_INCLUDED
