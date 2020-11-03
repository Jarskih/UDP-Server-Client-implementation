// charlie_messages.cc

#include "charlie_messages.hpp"
#include "charlie_network.hpp"

namespace charlie {
	namespace network {
		NetworkMessageServerTick::NetworkMessageServerTick()
			: type_(NETWORK_MESSAGE_SERVER_TICK)
			, server_time_(0)
			, server_tick_(0)
		{
		}

		NetworkMessageServerTick::NetworkMessageServerTick(const int64  server_time,
			const uint32 server_tick)
			: type_(NETWORK_MESSAGE_SERVER_TICK)
			, server_time_(server_time)
			, server_tick_(server_tick)
		{
		}

		bool NetworkMessageServerTick::read(NetworkStreamReader& reader)
		{
			return serialize(reader);
		}

		bool NetworkMessageServerTick::write(NetworkStreamWriter& writer)
		{
			return serialize(writer);
		}

		NetworkMessageEntityState::NetworkMessageEntityState()
			: type_(NETWORK_MESSAGE_ENTITY_STATE)
			, rotation_(0)
			, entity_id_(0)
			, turret_rotation_(0)
		{
		}

		NetworkMessageEntityState::NetworkMessageEntityState(const Transform& transform, float turret_rotation, uint32 id)
			: type_(NETWORK_MESSAGE_ENTITY_STATE)
			, position_(transform.position_)
			, rotation_(transform.rotation_)
			, entity_id_(id)
			, turret_rotation_(turret_rotation)
		{
		}

		bool NetworkMessageEntityState::read(NetworkStreamReader& reader)
		{
			return serialize(reader);
		}

		bool NetworkMessageEntityState::write(NetworkStreamWriter& writer)
		{
			return serialize(writer);
		}

		NetworkMessageInputCommand::NetworkMessageInputCommand()
			: type_(NETWORK_MESSAGE_INPUT_COMMAND)
			, bits_(0)
			, rot_(0)
			, fire_(false)
		{
		}

		NetworkMessageInputCommand::NetworkMessageInputCommand(uint8 bits, float rotation, bool fire)
			: type_(NETWORK_MESSAGE_INPUT_COMMAND)
			, bits_(bits)
			, rot_(rotation)
			, fire_(fire)
		{
		}

		bool NetworkMessageInputCommand::read(NetworkStreamReader& reader)
		{
			return serialize(reader);
		}

		bool NetworkMessageInputCommand::write(NetworkStreamWriter& writer)
		{
			return serialize(writer);
		}

		NetworkMessagePlayerState::NetworkMessagePlayerState()
			: type_(NETWORK_MESSAGE_PLAYER_STATE)
			, rotation_(0)
			, y_(0)
			, x_(0)
			, turret_rotation_(0)
		{
		}

		NetworkMessagePlayerState::NetworkMessagePlayerState(const Transform& transform, float turret_rotation)
			: type_(NETWORK_MESSAGE_PLAYER_STATE)
			, rotation_(transform.rotation_)
			, y_((uint16)transform.position_.y_)
			, x_((uint16)transform.position_.x_)
			, turret_rotation_(turret_rotation)
		{
		}

		bool NetworkMessagePlayerState::read(NetworkStreamReader& reader)
		{
			return serialize(reader);
		}

		bool NetworkMessagePlayerState::write(NetworkStreamWriter& writer)
		{
			return serialize(writer);
		}

		NetworkMessagePlayerSpawn::NetworkMessagePlayerSpawn()
			: type_(NETWORK_MESSAGE_PLAYER_SPAWN)
			, entity_id_(0)
			, event_id_(0)
		{
		}

		NetworkMessagePlayerSpawn::NetworkMessagePlayerSpawn(uint32 event_id, uint32 entity_id, const Vector2& position)
			: type_(NETWORK_MESSAGE_PLAYER_SPAWN)
			, position_(position)
			, entity_id_(entity_id)
			, event_id_(event_id)
		{
		}

		bool NetworkMessagePlayerSpawn::read(NetworkStreamReader& reader)
		{
			return serialize(reader);
		}

		bool NetworkMessagePlayerSpawn::write(NetworkStreamWriter& writer)
		{
			return serialize(writer);
		}

		NetworkMessageAck::NetworkMessageAck()
			: type_(NETWORK_MESSAGE_ACK)
			, event_id_(0)
		{
		}

		NetworkMessageAck::NetworkMessageAck(const uint32 id)
			: type_(NETWORK_MESSAGE_ACK)
			, event_id_(id)
		{
		}

		bool NetworkMessageAck::read(NetworkStreamReader& reader)
		{
			return serialize(reader);
		}

		bool NetworkMessageAck::write(NetworkStreamWriter& writer)
		{
			return serialize(writer);
		}

		NetworkMessageProjectileSpawn::NetworkMessageProjectileSpawn()
			: type_(NETWORK_MESSAGE_PROJECTILE_SPAWN)
			, entity_id_(0)
			, shot_by_(0)
			, event_id_(0)
			, rotation_(0)
		{
		}

		NetworkMessageProjectileSpawn::NetworkMessageProjectileSpawn(const uint32 id, const uint32 entity_id, const uint32 shot_by, const Vector2& position, float rotation)
			: type_(NETWORK_MESSAGE_PROJECTILE_SPAWN)
			, entity_id_(entity_id)
			, shot_by_(shot_by)
			, event_id_(id)
			, pos_(position)
			, rotation_(rotation)
		{
		}

		bool NetworkMessageProjectileSpawn::read(NetworkStreamReader& reader)
		{
			return serialize(reader);
		}

		bool NetworkMessageProjectileSpawn::write(NetworkStreamWriter& writer)
		{
			return serialize(writer);
		}

		NetworkMessagePlayerDisconnected::NetworkMessagePlayerDisconnected()
			: type_(NETWORK_MESSAGE_DISCONNECTED)
			, entity_id_(0)
			, message_id_(0)
		{
		}

		NetworkMessagePlayerDisconnected::NetworkMessagePlayerDisconnected(const uint32 entity_id, const uint32 message_id)
			: type_(NETWORK_MESSAGE_DISCONNECTED)
			, entity_id_(entity_id)
			, message_id_(message_id)
		{
		}

		bool NetworkMessagePlayerDisconnected::read(NetworkStreamReader& reader)
		{
			return serialize(reader);
		}

		bool NetworkMessagePlayerDisconnected::write(NetworkStreamWriter& writer)
		{
			return serialize(writer);
		}

		NetworkMessageProjectileDestroy::NetworkMessageProjectileDestroy()
			: type_(NETWORK_MESSAGE_PROJECTILE_DESTROYED)
			, entity_id_(0)
			, event_id_(0)
		{
		}

		NetworkMessageProjectileDestroy::NetworkMessageProjectileDestroy(const uint32 entity_id, const uint32 event_id)
			: type_(NETWORK_MESSAGE_PROJECTILE_DESTROYED)
			, entity_id_(entity_id)
			, event_id_(event_id)
		{
		}

		bool NetworkMessageProjectileDestroy::read(NetworkStreamReader& reader)
		{
			return serialize(reader);
		}

		bool NetworkMessageProjectileDestroy::write(NetworkStreamWriter& writer)
		{
			return serialize(writer);
		}

		NetworkMessagePlayerDestroy::NetworkMessagePlayerDestroy()
			: type_(NETWORK_MESSAGE_PLAYER_DESTROYED)
			, entity_id_(0)
			, event_id_(0)
		{
		}

		NetworkMessagePlayerDestroy::NetworkMessagePlayerDestroy(uint32 entity_id, uint32 event_id) :
			type_(NETWORK_MESSAGE_PLAYER_DESTROYED)
			, entity_id_(entity_id)
			, event_id_(event_id)
		{
		}

		bool NetworkMessagePlayerDestroy::read(NetworkStreamReader& reader)
		{
			return serialize(reader);
		}

		bool NetworkMessagePlayerDestroy::write(NetworkStreamWriter& writer)
		{
			return serialize(writer);
		}
	} // !network
} // !messages
