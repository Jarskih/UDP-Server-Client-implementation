#pragma once
#include "charlie.hpp"

namespace charlie
{

	enum class EventType
{
	SPAWN_PLAYER,
	SPAWN_PROJECTILE,
	DESTROY_PLAYER,
	DESTROY_PROJECTILE,
	COUNT
};
	
struct Event
{
	Event(uint32 id, EventType type, uint32 send_to);
	Event(uint32 id, EventType type, uint32 send_to, Vector2 position);
	Event(uint32 id, EventType type, uint32 send_to, uint32 owner_, Vector2 position, float rotation);
	uint32 id_{};
	EventType type_;
	uint32 send_to_{};
	Vector2 pos_;
	float rot_;
	uint32 owner_;
};

struct PlayerSpawned : Event
{
	PlayerSpawned(uint32 id, uint32 send_to, Vector2 pos);
};

struct ProjectileSpawned : Event
{
	ProjectileSpawned(uint32 id, uint32 send_to, uint32 owner, Vector2 position, float rotation);
};

struct ProjectileDestroyed : Event
{
	ProjectileDestroyed(uint32 id, uint32 send_to);
};

struct PlayerDestroyed : Event
{
	PlayerDestroyed(uint32 id, uint32 send_to);
};
	
}
