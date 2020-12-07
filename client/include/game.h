#pragma once
#include <sdl_application.hpp>

#include "level_manager.h"
#include "master_server_client.h"
#include "player.hpp"
#include "projectile.h"
#include "Scene.h"
#include "entity.h"

namespace charlie
{
	struct Game : Scene, network::IConnectionListener
	{
		Game();

		bool on_init(SDL_Renderer* renderer) override;
		void on_exit() override;
		bool on_tick(const Time& dt) override;
		void on_draw() override;

		void on_acknowledge(network::Connection* connection, uint16 sequence) override;
		void on_receive(network::Connection* connection, network::NetworkStreamReader& reader) override;
		void on_send(network::Connection* connection, uint16 sequence, network::NetworkStreamWriter& writer) override;

		// Modify entities
		void spawn_entity(network::NetworkMessageEntitySpawn message);
		void spawn_player(network::NetworkMessagePlayerSpawn message);
		void remove_entity(int32 id);
		void remove_projectile(int32 id);
		void spawn_projectile(network::NetworkMessageProjectileSpawn message);

		// Helpers
		static bool contains(const DynamicArray<Entity>& vector, int32 id);
		static bool contains(const DynamicArray<Projectile>& vector, int32 id);

		// Messages
		void create_ack_message(int32 event_id_);

		SDL_Renderer* renderer_;

		// Networking
		network::Connection connection_;
		const Time tickrate_;
		Time accumulator_;
		Time lastSend_;
		Time lastReceive_;
		Queue<network::NetworkMessageAck> message_queue_;
		Queue<network::NetworkMessageLevelDataRequest> level_message_queue_;
		gameplay::Inputinator inputinator_;
		Networkinfo networkinfo_;
		Vector2 oldPos_;
		Vector2 newPos_;
		int32 tick_;
		int32 server_tick_;
		Time server_time_;
		network::IPAddress server_;

		// Gameplay
		Camera cam_;
		Player player_;
		DynamicArray <Entity> entities_;
		DynamicArray<int32> entities_to_remove_;
		DynamicArray<Projectile> projectiles_;
		DynamicArray<int32> projectiles_to_remove_;
		LevelManager level_manager_;
		TextHandler text_handler_;
		SDLFont text_font_;
		int32 local_projectile_index_;
		int level_width_;
		int level_heigth_;
		SDLSprite* disconnected_;
	};
}
