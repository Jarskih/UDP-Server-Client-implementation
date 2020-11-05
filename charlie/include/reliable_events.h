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
		uint32 event_id_;
		EventType type_;
		uint32 entity_id_;
		uint32 creator_;
		uint32 send_to_;
		Vector2 pos_;
		float rot_;
		Tile tile_;
		uint8 level_id_;
	};

	struct PlayerSpawned : Event
	{
		PlayerSpawned(uint32 id, uint32 entity_id, uint32 send_to, Vector2 pos);
	};

	struct EntitySpawned : Event
	{
		EntitySpawned(uint32 id, uint32 entity_id, uint32 send_to, Vector2 pos);
	};


	struct ProjectileSpawned : Event
	{
		ProjectileSpawned(uint32 event_id, uint32 entity_id, uint32 creator, uint32 send_to, Vector2 position, float rotation);
	};

	struct ProjectileDestroyed : Event
	{
		ProjectileDestroyed(uint32 event_id, uint32 entity_id, uint32 send_to);
	};

	struct PlayerDestroyed : Event
	{
		PlayerDestroyed(uint32 event_id, uint32 entity_id, uint32 send_to);
	};

	struct EntityDestroyed : Event
	{
		EntityDestroyed(uint32 event_id, uint32 entity_id, uint32 send_to);
	};

	struct ReliableEvents
	{
		ReliableEvents();
		void create_spawn_event(uint32 entity_id, const Player& event_creator, uint32 send_to, EventType event, const DynamicArray<Player>& players);
		void create_destroy_event(uint32 entity_id, const uint32 send_to, EventType event, const DynamicArray<Player>& players);
		void clear();
		Event get_event(uint32 id);
		void send_level_info(uint8 level, uint32 send_to);
		void send_level_data(Tile tile, uint32 send_to);
		DynamicArray<Event> events_;
		uint32 event_id_;
		uint8 level_id_;
	};

}
