#include "reliable_events.h"

#include "player.hpp"

namespace charlie
{
	Event::Event() : event_id_(), type_(EventType::INVALID), entity_id_(0), creator_(0), send_to_(0), rot_(0), tile_()
	{
	}

	PlayerSpawned::PlayerSpawned(int32 event_id, int32 entity_id, int32 send_to, Vector2 position
	)
	{
		event_id_ = event_id;
		entity_id_ = entity_id;
		type_ = EventType::SPAWN_PLAYER;
		send_to_ = send_to;
		pos_ = position;
	}

	EntitySpawned::EntitySpawned(int32 event_id, int32 entity_id, int32 send_to, Vector2 position)
	{
		event_id_ = event_id;
		entity_id_ = entity_id;
		type_ = EventType::SPAWN_ENTITY;
		send_to_ = send_to;
		pos_ = position;
	}

	ProjectileSpawned::ProjectileSpawned(
		int32 event_id,
		int32 entity_id,
		int32 creator,
		int32 send_to,
		Vector2 position,
		float rotation
	)
	{
		event_id_ = event_id;
		entity_id_ = entity_id;
		type_ = EventType::SPAWN_PROJECTILE;
		creator_ = creator;
		send_to_ = send_to;
		pos_ = position;
		rot_ = rotation;
	}

	ProjectileDestroyed::ProjectileDestroyed(int32 event_id, int32 entity_id, int32 send_to)
	{
		event_id_ = event_id;
		entity_id_ = entity_id;
		type_ = EventType::DESTROY_PROJECTILE;
		send_to_ = send_to;
	}

	PlayerDestroyed::PlayerDestroyed(int32 event_id, int32 entity_id, int32 send_to)
	{
		event_id_ = event_id;
		entity_id_ = entity_id;
		type_ = EventType::DESTROY_PLAYER;
		send_to_ = send_to;
	}

	EntityDestroyed::EntityDestroyed(int32 event_id, int32 entity_id, int32 send_to)
	{
		event_id_ = event_id;
		entity_id_ = entity_id;
		type_ = EventType::DESTROY_ENTITY;
		send_to_ = send_to;
	}

	ReliableEvents::ReliableEvents() : event_id_(0)
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

	void ReliableEvents::create_spawn_event(int32 entity_id, const Player& event_creator, int32 send_to, const EventType event, const DynamicArray<Player>& players)
	{
		switch (event)
		{
		case EventType::SPAWN_ENTITY:
		{
			const EntitySpawned e(event_id_, entity_id, send_to, event_creator.transform_.position_);
			events_.push_back(e);
		} break;
		case EventType::SPAWN_PLAYER:
		{
			const PlayerSpawned e(event_id_, entity_id, send_to, event_creator.transform_.position_);
			events_.push_back(e);
		} break;
		case EventType::SPAWN_PROJECTILE:
		{
			const ProjectileSpawned e(event_id_, entity_id, event_creator.id_, send_to, event_creator.get_shoot_pos(), event_creator.turret_transform_.rotation_);
			events_.push_back(e);
		} break;
		default:
			break;
		}

		event_id_ += 1;
	}

	void ReliableEvents::create_destroy_event(const int32 entity_id, const int32 send_to, const EventType event, const DynamicArray<Player>& players)
	{
		switch (event)
		{
		case EventType::DESTROY_PLAYER:
		{
			PlayerDestroyed e(event_id_, entity_id, send_to);
			events_.push_back(e);
			printf("RELIABLE MESSAGE: Created player destroy event for player: %i\n", entity_id);
		} break;
		case EventType::DESTROY_ENTITY:
		{
			EntityDestroyed e(event_id_, entity_id, send_to);
			events_.push_back(e);
			printf("RELIABLE MESSAGE: Created entity destroy event for player: %i\n", entity_id);
		} break;
		case EventType::DESTROY_PROJECTILE:
		{
			ProjectileDestroyed e(event_id_, entity_id, send_to);
			events_.push_back(e);
			printf("RELIABLE MESSAGE: Created projectile destroy event for projectile: %i \n", entity_id);
		} break;
		case EventType::PLAYER_DISCONNECTED:
		{
			PlayerDestroyed e(event_id_, entity_id, send_to);
			events_.push_back(e);
			printf("RELIABLE MESSAGE: Created player disconnected event for player: %i\n", entity_id);
		} break;
		default:
			break;
		}
		printf("RELIABLE MESSAGE: reliable events in queue %i \n", (int)events_.size());
		event_id_ += 1;
	}

	void ReliableEvents::clear()
	{
		events_.clear();
	}

	Event ReliableEvents::get_event(int32 id)
	{
		Event event;
		for (const auto e : events_)
		{
			if (e.event_id_ == id)
			{
				event = e;
			}
		}
		return event;
	}

	void ReliableEvents::send_level_info(uint8 level_id, int32 send_to)
	{
		Event e;
		e.event_id_ = event_id_;
		e.level_id_ = level_id;
		e.type_ = EventType::SEND_LEVEL_INFO;
		e.send_to_ = send_to;
		events_.push_back(e);
		event_id_ += 1;
	}

	void ReliableEvents::send_level_data(Tile tile, int32 send_to)
	{
		Event e;
		e.event_id_ = event_id_;
		e.tile_ = tile;
		e.type_ = EventType::SEND_LEVEL_DATA;
		e.send_to_ = send_to;
		events_.push_back(e);
		event_id_ += 1;
	}
}
