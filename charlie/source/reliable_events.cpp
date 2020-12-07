#include "reliable_events.h"

#include "player.hpp"

namespace charlie
{
	PlayerSpawned::PlayerSpawned(uint32 entity_id, uint32 send_to, Vector2 position
	)
	{
		entity_id_ = entity_id;
		type_ = EventType::SPAWN_PLAYER;
		send_to_ = send_to;
		pos_ = position;
	}

	EntitySpawned::EntitySpawned(uint32 entity_id, uint32 send_to, Vector2 position)
	{
		entity_id_ = entity_id;
		type_ = EventType::SPAWN_ENTITY;
		send_to_ = send_to;
		pos_ = position;
	}

	ProjectileSpawned::ProjectileSpawned(
		uint32 entity_id,
		uint32 creator,
		uint32 send_to,
		Vector2 position,
		float rotation
	)
	{
		entity_id_ = entity_id;
		type_ = EventType::SPAWN_PROJECTILE;
		creator_ = creator;
		send_to_ = send_to;
		pos_ = position;
		rot_ = rotation;
	}

	ProjectileDestroyed::ProjectileDestroyed(uint32 entity_id, uint32 send_to)
	{
		entity_id_ = entity_id;
		type_ = EventType::DESTROY_PROJECTILE;
		send_to_ = send_to;
	}

	PlayerDestroyed::PlayerDestroyed(uint32 entity_id, uint32 send_to)
	{
		entity_id_ = entity_id;
		type_ = EventType::DESTROY_PLAYER;
		send_to_ = send_to;
	}

	EntityDestroyed::EntityDestroyed(uint32 entity_id, uint32 send_to)
	{
		entity_id_ = entity_id;
		type_ = EventType::DESTROY_ENTITY;
		send_to_ = send_to;
	}

	ReliableEvents::ReliableEvents()
	{
	}

	/// <summary>
	/// Create spawn event to be sent to players
	/// </summary>
	/// <param name="entity_id">Entity id which is spawned</param>
	/// <param name="event_creator">Player who is spawned or spawned projectile</param>
	/// <param name="send_to">Send to player with this id</param>
	/// <param name="event">Event type SPAWN_PLAYER or SPAWN_PROJECTILE</param>
	/// <param name="players">List of connected players</param>

	void ReliableEvents::create_spawn_event(const uint32 entity_id, const Player& event_creator, uint32 send_to, const EventType event, const DynamicArray<Player>& players)
	{
		switch (event)
		{
		case EventType::SPAWN_ENTITY:
		{
			const EntitySpawned e(entity_id, send_to, event_creator.transform_.position_);
			events_.push_back(e);
		} break;
		case EventType::SPAWN_PLAYER:
		{
			const PlayerSpawned e(entity_id, send_to, event_creator.transform_.position_);
			events_.push_back(e);
		} break;
		case EventType::SPAWN_PROJECTILE:
		{
			const ProjectileSpawned e(entity_id, event_creator.id_, send_to, event_creator.get_shoot_pos(), event_creator.turret_transform_.rotation_);
			events_.push_back(e);
		} break;
		default:
			break;
		}
	}

	void ReliableEvents::create_destroy_event(const uint32 entity_id, const uint32 send_to, const EventType event, const DynamicArray<Player>& players)
	{
		switch (event)
		{
		case EventType::DESTROY_PLAYER:
		{
			PlayerDestroyed reliableEvent(entity_id, send_to);
			events_.push_back(reliableEvent);
			printf("RELIABLE MESSAGE: Created player destroy event for player: %i\n", entity_id);
		} break;
		case EventType::DESTROY_ENTITY:
		{
			EntityDestroyed reliableEvent(entity_id, send_to);
			events_.push_back(reliableEvent);
			printf("RELIABLE MESSAGE: Created entity destroy event for player: %i\n", entity_id);
		} break;
		case EventType::DESTROY_PROJECTILE:
		{
			ProjectileDestroyed reliableEvent(entity_id, send_to);
			events_.push_back(reliableEvent);
			printf("RELIABLE MESSAGE: Created projectile destroy event for projectile: %i \n", entity_id);
		} break;
		case EventType::PLAYER_DISCONNECTED:
		{
			PlayerDestroyed reliableEvent(entity_id, send_to);
			events_.push_back(reliableEvent);
			printf("RELIABLE MESSAGE: Created player disconnected event for player: %i\n", entity_id);
		} break;
		default:
			break;
		}
		printf("RELIABLE MESSAGE: reliable events in queue %i \n", (int)events_.size());
	}

	void ReliableEvents::clear()
	{
		events_.clear();
	}

	Event ReliableEvents::get_event(uint16 sequence)
	{
		Event event;
		/*
		for (const auto& reliableEvent : events_)
		{
			if (reliableEvent.seq_ == sequence)
			{
				event = reliableEvent;
			}
		}
		*/
		return event;
	}

	void ReliableEvents::send_level_info(uint8 level_id, uint32 send_to)
	{
		Event e;
		e.level_id_ = level_id;
		e.type_ = EventType::SEND_LEVEL_INFO;
		e.send_to_ = send_to;
		events_.push_back(e);
	}

	void ReliableEvents::send_level_data(Tile tile, uint32 send_to)
	{
		Event e;
		e.tile_ = tile;
		e.type_ = EventType::SEND_LEVEL_DATA;
		e.send_to_ = send_to;
		events_.push_back(e);
	}
}
