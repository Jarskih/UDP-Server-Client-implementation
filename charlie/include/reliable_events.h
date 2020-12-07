#pragma once
#include "charlie.hpp"
#include "player.hpp"
#include "leveldata.h"

namespace charlie
{
	enum class EventType
	{
		INVALID,
		SPAWN_PLAYER,
		SPAWN_PROJECTILE,
		DESTROY_PLAYER,
		DESTROY_ENTITY,
		DESTROY_PROJECTILE,
		PLAYER_DISCONNECTED,
		SPAWN_ENTITY,
		SEND_LEVEL_INFO,
		SEND_LEVEL_DATA,
		COUNT,
	};

	struct Event
	{
		Event();
		int32 event_id_;
		EventType type_;
		int32 entity_id_;
		int32 creator_;
		int32 send_to_;
		Vector2 pos_;
		float rot_;
		Tile tile_;
		uint8 level_id_;
	};

	struct PlayerSpawned : Event
	{
		PlayerSpawned(int32 id, int32 entity_id, int32 send_to, Vector2 pos);
	};

	struct EntitySpawned : Event
	{
		EntitySpawned(int32 id, int32 entity_id, int32 send_to, Vector2 pos);
	};


	struct ProjectileSpawned : Event
	{
		ProjectileSpawned(int32 event_id, int32 entity_id, int32 creator, int32 send_to, Vector2 position, float rotation);
	};

	struct ProjectileDestroyed : Event
	{
		ProjectileDestroyed(int32 event_id, int32 entity_id, int32 send_to);
	};

	struct PlayerDestroyed : Event
	{
		PlayerDestroyed(int32 event_id, int32 entity_id, int32 send_to);
	};

	struct EntityDestroyed : Event
	{
		EntityDestroyed(int32 event_id, int32 entity_id, int32 send_to);
	};

	struct ReliableEvents
	{
		ReliableEvents();
		void create_spawn_event(int32 entity_id, const Player& event_creator, int32 send_to, EventType event, const DynamicArray<Player>& players);
		void create_destroy_event(int32 entity_id, int32 send_to, EventType event, const DynamicArray<Player>& players);
		void clear();
		Event get_event(int32 id);
		void send_level_info(uint8 level, int32 send_to);
		void send_level_data(Tile tile, int32 send_to);
		DynamicArray<Event> events_;
		int32 event_id_;
		uint8 level_id_;
	};

}
