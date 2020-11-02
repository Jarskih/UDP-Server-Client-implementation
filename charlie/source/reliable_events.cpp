#include "reliable_events.h"

#include "player.hpp"

namespace charlie
{
	Event::Event(uint32 id, EventType type, uint32 send_to)
		: id_(id)
		, type_(type)
		, send_to_(send_to)
		, rot_(0)
		, owner_(0)
	{
	}

	Event::Event(uint32 id, EventType type, uint32 send_to, Vector2 position)
		: id_(id)
		, type_(type)
		, send_to_(send_to)
		, pos_(position)
		, rot_(0)
		, owner_(0)
	{
	}


	Event::Event(uint32 id, EventType type, uint32 send_to, uint32 owner, Vector2 position, float rotation)
		: id_(id)
		, type_(type)
		, send_to_(send_to)
		, pos_(position)
		, rot_(rotation)
		, owner_(owner)
	{
	}

	PlayerSpawned::PlayerSpawned(uint32 id, uint32 send_to, Vector2 position)
		: Event(id, EventType::SPAWN_PLAYER, send_to, position)
	{
	}

	ProjectileSpawned::ProjectileSpawned(uint32 id, uint32 send_to, uint32 owner, Vector2 position, float rotation)
		: Event(id, EventType::SPAWN_PROJECTILE, send_to, owner, position, rotation)
	{
	}

	ProjectileDestroyed::ProjectileDestroyed(uint32 id, uint32 send_to) : Event(id, EventType::DESTROY_PROJECTILE, send_to)
	{
	}

	PlayerDestroyed::PlayerDestroyed(uint32 id, uint32 send_to) : Event(id, EventType::DESTROY_PLAYER, send_to)
	{
	}

	void ReliableEvents::create_spawn_event(const uint32 owner, const Player& p, const EventType event, const DynamicArray<Player>& players, uint32& projectile_index)
	{
		for (auto& player : players)
		{
			if (player.id_ == owner)
			{
				continue;
			}

			switch (event)
			{
			case EventType::SPAWN_PLAYER:
			{
				PlayerSpawned e(owner, player.id_, p.transform_.position_);
				events_.push_back(e);
				printf("created player spawned event \n");
			} break;
			case EventType::SPAWN_PROJECTILE:
			{
				ProjectileSpawned e(projectile_index, player.id_, owner, player.get_shoot_pos(), p.turret_rotation_);
				events_.push_back(e);
				printf("created projectile spawned event \n");
			} break;
			default:
				break;
			}
		}
		printf("reliable events in queue %i \n", (int)events_.size());
	}

	void ReliableEvents::create_destroy_event(const uint32 id, const EventType event, const DynamicArray<Player>& players)
	{
		for (auto& player : players)
		{
			switch (event)
			{
			case EventType::DESTROY_PLAYER:
			{
				PlayerDestroyed e(id, player.id_);
				events_.push_back(e);
				printf("created player destroy event \n");
			} break;
			case EventType::DESTROY_PROJECTILE:
			{
				ProjectileDestroyed e(id, player.id_);
				events_.push_back(e);
				printf("created projectile destroy event \n");
			} break;
			default:
				break;
			}
		}
		printf("reliable events in queue %i \n", (int)events_.size());
	}
}