// charlie_messages.cc

#include "charlie_messages.hpp"

#include "charlie_network.hpp"

namespace charlie {
	namespace network {
		NetworkMessageServerTick::NetworkMessageServerTick(const int64  server_time,
			const uint32 server_tick)
			: server_time_(server_time)
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

		NetworkMessageEntityState::NetworkMessageEntityState(const Transform& transform, float turret_rotation, uint32 entity_id)
			: x_((int16)transform.position_.x_)
			, y_((int16)transform.position_.y_)
			, rotation_((int16)transform.rotation_)
			, entity_id_((uint8)entity_id)
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

		NetworkMessageInputCommand::NetworkMessageInputCommand(uint8 bits, float rotation, bool fire)
			: bits_(bits)
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

		NetworkMessagePlayerState::NetworkMessagePlayerState(const Transform& transform, float turret_rotation)
			: rotation_((int16)transform.rotation_)
			, y_((int16)transform.position_.y_)
			, x_((int16)transform.position_.x_)
			, turret_rotation_((int16)turret_rotation)
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

		NetworkMessagePlayerSpawn::NetworkMessagePlayerSpawn(uint32 entity_id, const Vector2& position)
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

		NetworkMessageEntitySpawn::NetworkMessageEntitySpawn(uint32 entity_id, const Vector2& position) :
			position_(position),
			entity_id_((uint8)entity_id)
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

		bool NetworkMessageAck::read(NetworkStreamReader& reader)
		{
			return serialize(reader);
		}

		bool NetworkMessageAck::write(NetworkStreamWriter& writer)
		{
			return serialize(writer);
		}

		NetworkMessageProjectileSpawn::NetworkMessageProjectileSpawn(const uint32 entity_id, const uint32 shot_by, const Vector2& position, float rotation)
			: entity_id_((uint8)entity_id)
			, shot_by_((uint8)shot_by)
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

		NetworkMessagePlayerDisconnected::NetworkMessagePlayerDisconnected(uint32 entity_id) : entity_id_((uint8)entity_id)
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

		NetworkMessageProjectileDestroy::NetworkMessageProjectileDestroy(const uint32 entity_id)
			: entity_id_((uint8)entity_id)
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

		NetworkMessageEntityDestroy::NetworkMessageEntityDestroy(uint32 entity_id)
			: entity_id_((uint8)entity_id)
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

		NetworkMessagePlayerDestroy::NetworkMessagePlayerDestroy(uint32 entity_id)
			: entity_id_((uint8)entity_id)
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

		NetworkMessageLevelInfo::NetworkMessageLevelInfo(uint8 level_id, uint8 size_x_, uint8 size_y_)
			: level_id_(level_id)
			, size_x_(size_x_)
			, size_y_(size_y_)
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

		bool NetworkMessageLevelDataRequest::read(NetworkStreamReader& reader)
		{
			return serialize(reader);
		}

		bool NetworkMessageLevelDataRequest::write(NetworkStreamWriter& writer)
		{
			return serialize(writer);
		}

		NetworkMessageLevelData::NetworkMessageLevelData(Tile tile)
			: level_tile_(tile.tile_id_)
			, x_(tile.x_)
			, y_(tile.y_)
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


		NetworkMessageMasterServer::NetworkMessageMasterServer(uint8 a_, uint8 b_, uint8 c_, uint8 d_)
			: a_(a_)
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
