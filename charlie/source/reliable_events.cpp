#include "reliable_events.h"

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
}

