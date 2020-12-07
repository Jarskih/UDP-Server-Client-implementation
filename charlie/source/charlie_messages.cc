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
			const int32 server_tick)
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
			, x_(0)
			, y_(0)
			, rotation_(0)
			, entity_id_(0)
			, turret_rotation_(0)
		{
		}

		NetworkMessageEntityState::NetworkMessageEntityState(const Transform& transform, float turret_rotation, int32 entity_id)
			: type_(NETWORK_MESSAGE_ENTITY_STATE)
			, x_((int16)transform.position_.x_)
			, y_((int16)transform.position_.y_)
			, rotation_((int16)transform.rotation_)
			, entity_id_((int8)entity_id)
			, turret_rotation_((int16)turret_rotation)
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
			, rot_((int16)rotation)
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
			, rotation_(static_cast<int16>(transform.rotation_))
			, y_(static_cast<int16>(transform.position_.y_))
			, x_(static_cast<int16>(transform.position_.x_))
			, turret_rotation_(static_cast<int16>(turret_rotation))
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

		NetworkMessagePlayerSpawn::NetworkMessagePlayerSpawn(int32 event_id, int32 entity_id, const Vector2& position)
			: type_(NETWORK_MESSAGE_PLAYER_SPAWN)
			, position_(position)
			, entity_id_((uint8)entity_id)
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

		NetworkMessageEntitySpawn::NetworkMessageEntitySpawn() : type_(NETWORK_MESSAGE_ENTITY_SPAWN), entity_id_(0), event_id_(0)
		{
		}

		NetworkMessageEntitySpawn::NetworkMessageEntitySpawn(int32 event_id, int32 entity_id, const Vector2& position) :
			type_(NETWORK_MESSAGE_ENTITY_SPAWN),
			position_(position),
			entity_id_((uint8)entity_id),
			event_id_(event_id)
		{
		}

		bool NetworkMessageEntitySpawn::read(NetworkStreamReader& reader)
		{
			return serialize(reader);
		}

		bool NetworkMessageEntitySpawn::write(NetworkStreamWriter& writer)
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

		NetworkMessageProjectileSpawn::NetworkMessageProjectileSpawn(const int32 id, const int32 entity_id, const int32 shot_by, const Vector2& position, float rotation)
			: type_(NETWORK_MESSAGE_PROJECTILE_SPAWN)
			, entity_id_((uint8)entity_id)
			, shot_by_((uint8)shot_by)
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

		NetworkMessagePlayerDisconnected::NetworkMessagePlayerDisconnected(const int32 entity_id, const int32 message_id)
			: type_(NETWORK_MESSAGE_DISCONNECTED)
			, entity_id_((uint8)entity_id)
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

		NetworkMessageProjectileDestroy::NetworkMessageProjectileDestroy(const int32 entity_id, const int32 event_id)
			: type_(NETWORK_MESSAGE_PROJECTILE_DESTROYED)
			, entity_id_((uint8)entity_id)
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

		NetworkMessageEntityDestroy::NetworkMessageEntityDestroy()
			: type_(NETWORK_MESSAGE_ENTITY_DESTROYED)
			, entity_id_(0)
			, event_id_(0)
		{
		}

		NetworkMessageEntityDestroy::NetworkMessageEntityDestroy(int32 entity_id, int32 event_id) :
			type_(NETWORK_MESSAGE_ENTITY_DESTROYED)
			, entity_id_((uint8)entity_id)
			, event_id_(event_id)
		{
		}

		bool NetworkMessageEntityDestroy::read(NetworkStreamReader& reader)
		{
			return serialize(reader);
		}

		bool NetworkMessageEntityDestroy::write(NetworkStreamWriter& writer)
		{
			return serialize(writer);
		}

		NetworkMessagePlayerDestroy::NetworkMessagePlayerDestroy()
			: type_(NETWORK_MESSAGE_PLAYER_DESTROYED)
			, entity_id_(0)
			, event_id_(0)
		{
		}

		NetworkMessagePlayerDestroy::NetworkMessagePlayerDestroy(int32 entity_id, int32 event_id) :
			type_(NETWORK_MESSAGE_PLAYER_DESTROYED)
			, entity_id_((uint8)entity_id)
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

		NetworkMessageLevelInfo::NetworkMessageLevelInfo() : type_(NETWORK_MESSAGE_LEVEL_INFO), level_id_(0),
			size_x_(0),
			size_y_(0), event_id_(0)
		{
		}

		NetworkMessageLevelInfo::NetworkMessageLevelInfo(uint8 level_id, uint8 size_x_, uint8 size_y_, int32 event_id)
			: type_(NETWORK_MESSAGE_LEVEL_INFO)
			, level_id_(level_id)
			, size_x_(size_x_), size_y_(size_y_), event_id_(event_id)
		{
		}

		bool NetworkMessageLevelInfo::read(NetworkStreamReader& reader)
		{
			return serialize(reader);
		}

		bool NetworkMessageLevelInfo::write(NetworkStreamWriter& writer)
		{
			return serialize(writer);
		}

		NetworkMessageLevelDataRequest::NetworkMessageLevelDataRequest() : type_(NETWORK_MESSAGE_LEVEL_REQUEST),
			event_id_(0)
		{
		}

		NetworkMessageLevelDataRequest::NetworkMessageLevelDataRequest(int32 event_id) : type_(NETWORK_MESSAGE_LEVEL_REQUEST), event_id_(event_id)
		{
		}

		bool NetworkMessageLevelDataRequest::read(NetworkStreamReader& reader)
		{
			return serialize(reader);
		}

		bool NetworkMessageLevelDataRequest::write(NetworkStreamWriter& writer)
		{
			return serialize(writer);
		}

		NetworkMessageLevelData::NetworkMessageLevelData() : type_(NETWORK_MESSAGE_LEVEL_DATA), level_tile_(0), x_(0),
			y_(0),
			event_id_(0)
		{
		}

		NetworkMessageLevelData::NetworkMessageLevelData(Tile tile, int32 event_id)
			: type_(NETWORK_MESSAGE_LEVEL_DATA)
			, level_tile_(tile.tile_id_)
			, x_(tile.x_)
			, y_(tile.y_)
			, event_id_(event_id)
		{
		}

		bool NetworkMessageLevelData::read(NetworkStreamReader& reader)
		{
			return serialize(reader);
		}

		bool NetworkMessageLevelData::write(NetworkStreamWriter& writer)
		{
			return serialize(writer);
		}

		NetworkMessageMasterServer::NetworkMessageMasterServer() : type_(NETWORK_MESSAGE_MASTER_SERVER), a_(0), b_(0), c_(0), d_(0)
		{
		}

		NetworkMessageMasterServer::NetworkMessageMasterServer(uint8 a_, uint8 b_, uint8 c_, uint8 d_)
			: type_(NETWORK_MESSAGE_MASTER_SERVER)
			, a_(a_)
			, b_(b_)
			, c_(c_)
			, d_(d_)
		{
		}

		bool NetworkMessageMasterServer::read(NetworkStreamReader& reader)
		{
			return serialize(reader);
		}

		bool NetworkMessageMasterServer::write(NetworkStreamWriter& writer)
		{
			return serialize(writer);
		}
	} // !network
} // !messages
