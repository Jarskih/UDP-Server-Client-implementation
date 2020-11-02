#include "reliable_events.h"

#include "player.hpp"

namespace charlie
{
	Event::Event(
		uint32 event_id,
		uint32 entity_id,
		EventType type,
		uint32 send_to
	)
		: event_id_(event_id)
		, type_(type)
		, entity_id_(entity_id)
		, creator_(0)
		, send_to_(send_to)
		, rot_(0)
	{
	}

	Event::Event(
		uint32 event_id,
		uint32 entity_id,
		EventType type,
		uint32 send_to,
		Vector2 position
	)
		: event_id_(event_id)
		, type_(type)
		, entity_id_(entity_id)
		, creator_(0)
		, send_to_(send_to)
		, pos_(position)
		, rot_(0)
	{
	}

	Event::Event(
		uint32 event_id,
		uint32 entity_id,
		EventType type,
		uint32 creator,
		uint32 send_to,
		Vector2 position,
		float rotation
	)
		: event_id_(event_id)
		, type_(type)
		, entity_id_(entity_id)
		, creator_(creator)
		, send_to_(send_to)
		, pos_(position)
		, rot_(rotation)
	{
	}

	PlayerSpawned::PlayerSpawned(
		uint32 event_id,
		uint32 entity_id,
		uint32 send_to,
		Vector2 position
	)
		: Event(event_id, entity_id, EventType::SPAWN_PLAYER, send_to, position)
	{
	}

	ProjectileSpawned::ProjectileSpawned(
		uint32 event_id,
		uint32 entity_id,
		uint32 creator,
		uint32 send_to,
		Vector2 position,
		float rotation
	)
		: Event(event_id, entity_id, EventType::SPAWN_PROJECTILE, creator, send_to, position, rotation)
	{
	}

	ProjectileDestroyed::ProjectileDestroyed(uint32 id, uint32 entity_id, uint32 send_to) : Event(id, entity_id, EventType::DESTROY_PROJECTILE, send_to)
	{
	}

	PlayerDestroyed::PlayerDestroyed(uint32 id, uint32 entity_id, uint32 send_to) : Event(id, entity_id, EventType::DESTROY_PLAYER, send_to)
	{
	}

	ReliableEvents::ReliableEvents() : event_id_(0)
	{
	}

	/// <summary>
	/// Create spawn event to be sent to players
	/// </summary>
	/// <param name="entity_id">Entity id which is spawned</param>
	/// <param name="p">Player who is spawned or spawned projectile</param>
	/// <param name="event">Event type SPAWN_PLAYER or SPAWN_PROJECTILE</param>
	/// <param name="players">List of connected players</param>

	void ReliableEvents::create_spawn_event(const uint32 entity_id, const Player& p, const EventType event, const DynamicArray<Player>& players)
	{
		for (auto& player : players)
		{
			switch (event)
			{
			case EventType::SPAWN_PLAYER:
			{
				if (player.id_ == p.id_)
				{
					continue;
				}
				PlayerSpawned e(event_id_, entity_id, player.id_, p.transform_.position_);
				events_.push_back(e);
			} break;
			case EventType::SPAWN_PROJECTILE:
			{
				ProjectileSpawned e(event_id_, entity_id, p.id_, player.id_, player.get_shoot_pos(), p.turret_rotation_);
				events_.push_back(e);
			} break;
			default:
				break;
			}
		}
		event_id_ += 1;
	}

	void ReliableEvents::create_destroy_event(const uint32 entity_id, const EventType event, const DynamicArray<Player>& players)
	{
		for (auto& player : players)
		{
			switch (event)
			{
			case EventType::DESTROY_PLAYER:
			{
				PlayerDestroyed e(event_id_, entity_id, player.id_);
				events_.push_back(e);
				printf("RELIABLE MESSAGE: Created player destroy event for player: %i\n", entity_id);
			} break;
			case EventType::DESTROY_PROJECTILE:
			{
				ProjectileDestroyed e(event_id_, entity_id, player.id_);
				events_.push_back(e);
				printf("RELIABLE MESSAGE: Created projectile destroy event for projectile: %i \n", entity_id);
			} break;
			default:
				break;
			}
		}
		printf("RELIABLE MESSAGE: reliable events in queue %i \n", (int)events_.size());
		event_id_ += 1;
	}

	void ReliableEvents::clear()
	{
		events_.clear();
	}

	Event ReliableEvents::get_event(uint32 id)
	{
		Event event(0, 0, EventType::INVALID, 0);
		for (auto e : events_)
		{
			if (e.event_id_ == id)
			{
				event = e;
			}
		}
		return event;
	}
}
