// charlie_messages.hpp

#ifndef CHARLIE_MESSAGES_HPP_INCLUDED
#define CHARLIE_MESSAGES_HPP_INCLUDED

#include <charlie.hpp>
#include <leveldata.h>

namespace charlie {
	struct Tile;

	namespace network {
		struct NetworkStreamReader;
		struct NetworkStreamWriter;

		enum NetworkMessageType {
			NETWORK_MESSAGE_SERVER_TICK,
			NETWORK_MESSAGE_ENTITY_STATE,
			NETWORK_MESSAGE_INPUT_COMMAND,
			NETWORK_MESSAGE_PLAYER_STATE,
			NETWORK_MESSAGE_PLAYER_SPAWN,
			NETWORK_MESSAGE_ENTITY_SPAWN,
			NETWORK_MESSAGE_MASTER_SERVER,
			NETWORK_MESSAGE_PROJECTILE_SPAWN,
			NETWORK_MESSAGE_DISCONNECTED,
			NETWORK_MESSAGE_PLAYER_DESTROYED,
			NETWORK_MESSAGE_ENTITY_DESTROYED,
			NETWORK_MESSAGE_PROJECTILE_DESTROYED,
			NETWORK_MESSAGE_LEVEL_INFO,
			NETWORK_MESSAGE_LEVEL_REQUEST,
			NETWORK_MESSAGE_LEVEL_DATA,
			NETWORK_MESSAGE_ACK,
			NETWORK_MESSAGE_COUNT,
		};

		static_assert(NETWORK_MESSAGE_COUNT <= 255, "network message type cannot exceed 255!");

		struct NetworkMessageServerTick {
			NetworkMessageServerTick() = default;
			explicit NetworkMessageServerTick(const int64  server_time,
				const uint32 server_tick);

			bool read(NetworkStreamReader& reader);
			bool write(NetworkStreamWriter& writer);

			template <typename Stream>
			bool serialize(Stream& stream)
			{
				bool result = true;
				result &= stream.serialize(type_);
				result &= stream.serialize(server_time_);
				result &= stream.serialize(server_tick_);
				return result;
			}

			uint8 type_ = NETWORK_MESSAGE_SERVER_TICK;
			int64 server_time_ = 0;
			uint32 server_tick_ = 0;
		};

		struct NetworkMessageEntityState {
			NetworkMessageEntityState() = default;
			explicit NetworkMessageEntityState(const Transform& transform, float turret_rotation, uint32 entity_id);

			bool read(NetworkStreamReader& reader);
			bool write(NetworkStreamWriter& writer);

			template <typename Stream>
			bool serialize(Stream& stream)
			{
				bool result = true;
				result &= stream.serialize(type_);
				result &= stream.serialize(x_);
				result &= stream.serialize(y_);
				result &= stream.serialize(rotation_);
				result &= stream.serialize(entity_id_);
				result &= stream.serialize(turret_rotation_);
				return result;
			}

			uint8 type_ = NETWORK_MESSAGE_ENTITY_STATE;
			int16 x_ = 0;
			int16 y_ = 0;
			int16 rotation_ = 0;
			uint8 entity_id_ = 0;
			int16 turret_rotation_ = 0;
		};

		struct NetworkMessageInputCommand {
			NetworkMessageInputCommand() = default;
			explicit NetworkMessageInputCommand(uint8 bits, float rotation, bool fire);

			bool read(NetworkStreamReader& reader);
			bool write(NetworkStreamWriter& writer);

			template <typename Stream>
			bool serialize(Stream& stream)
			{
				bool result = true;
				result &= stream.serialize(type_);
				result &= stream.serialize(bits_);
				result &= stream.serialize(rot_);
				result &= stream.serialize(fire_);
				return result;
			}

			uint8 type_ = NETWORK_MESSAGE_INPUT_COMMAND;
			uint8 bits_ = 0;
			int16 rot_ = 0;
			bool fire_ = false;
		};

		struct NetworkMessagePlayerState {
			NetworkMessagePlayerState() = default;
			explicit NetworkMessagePlayerState(const Transform& transform, float turret_rotation);

			bool read(NetworkStreamReader& reader);
			bool write(NetworkStreamWriter& writer);

			template <typename Stream>
			bool serialize(Stream& stream)
			{
				bool result = true;
				result &= stream.serialize(type_);
				result &= stream.serialize(y_);
				result &= stream.serialize(x_);
				result &= stream.serialize(rotation_);
				result &= stream.serialize(turret_rotation_);
				return result;
			}

			uint8 type_ = NETWORK_MESSAGE_PLAYER_STATE;
			int16 rotation_ = 0;
			int16 y_ = 0;
			int16 x_ = 0;
			int16 turret_rotation_ = 0;
		};

		// Reliable messages

		struct NetworkMessageAck
		{
			NetworkMessageAck() = default;
			bool read(NetworkStreamReader& reader);
			bool write(NetworkStreamWriter& writer);

			template <typename Stream>
			bool serialize(Stream& stream)
			{
				bool result = true;
				result &= stream.serialize(type_);
				return result;
			}

			uint8 type_ = NETWORK_MESSAGE_ACK;
		};

		struct NetworkMessagePlayerSpawn
		{
			NetworkMessagePlayerSpawn() = default;
			NetworkMessagePlayerSpawn(uint32 entity_id, const Vector2& position);
			bool read(NetworkStreamReader& reader);
			bool write(NetworkStreamWriter& writer);

			template <typename Stream>
			bool serialize(Stream& stream)
			{
				bool result = true;
				result &= stream.serialize(type_);
				result &= stream.serialize(position_.x_);
				result &= stream.serialize(position_.y_);
				result &= stream.serialize(entity_id_);
				return result;
			}


			uint8 type_ = NETWORK_MESSAGE_PLAYER_SPAWN;
			Vector2 position_;
			uint8 entity_id_ = 0;
		};

		struct NetworkMessageEntitySpawn
		{
			NetworkMessageEntitySpawn() = default;
			NetworkMessageEntitySpawn(uint32 entity_id, const Vector2& position);
			bool read(NetworkStreamReader& reader);
			bool write(NetworkStreamWriter& writer);

			template <typename Stream>
			bool serialize(Stream& stream)
			{
				bool result = true;
				result &= stream.serialize(type_);
				result &= stream.serialize(position_.x_);
				result &= stream.serialize(position_.y_);
				result &= stream.serialize(entity_id_);
				return result;
			}

			uint8 type_ = NETWORK_MESSAGE_ENTITY_SPAWN;
			Vector2 position_;
			uint8 entity_id_ = 0;
		};

		struct NetworkMessageProjectileSpawn
		{
			NetworkMessageProjectileSpawn() = default;
			NetworkMessageProjectileSpawn(uint32 entity_id, uint32 shot_by, const Vector2& position, float rotation);
			bool read(NetworkStreamReader& reader);
			bool write(NetworkStreamWriter& writer);

			template <typename Stream>
			bool serialize(Stream& stream)
			{
				bool result = true;
				result &= stream.serialize(type_);
				result &= stream.serialize(entity_id_);
				result &= stream.serialize(shot_by_);
				result &= stream.serialize(pos_.x_);
				result &= stream.serialize(pos_.y_);
				result &= stream.serialize(rotation_);
				return result;
			}

			uint8 type_ = NETWORK_MESSAGE_PROJECTILE_SPAWN;
			uint8 entity_id_ = 0;
			uint8 shot_by_ = 0;
			Vector2 pos_;
			float rotation_ = 0;
		};

		struct NetworkMessagePlayerDisconnected
		{
			NetworkMessagePlayerDisconnected() = default;
			explicit NetworkMessagePlayerDisconnected(uint32 entity_id);
			bool read(NetworkStreamReader& reader);
			bool write(NetworkStreamWriter& writer);

			template <typename Stream>
			bool serialize(Stream& stream)
			{
				bool result = true;
				result &= stream.serialize(type_);
				result &= stream.serialize(entity_id_);
				return result;
			}

			uint8 type_ = NETWORK_MESSAGE_DISCONNECTED;
			uint8 entity_id_ = 0;
		};

		struct NetworkMessageProjectileDestroy
		{
			NetworkMessageProjectileDestroy() = default;
			explicit NetworkMessageProjectileDestroy(uint32 entity_id);
			bool read(NetworkStreamReader& reader);
			bool write(NetworkStreamWriter& writer);

			template <typename Stream>
			bool serialize(Stream& stream)
			{
				bool result = true;
				result &= stream.serialize(type_);
				result &= stream.serialize(entity_id_);
				return result;
			}

			uint8 type_ = NETWORK_MESSAGE_PROJECTILE_DESTROYED;
			uint8 entity_id_ = 0;
		};

		struct NetworkMessageEntityDestroy
		{
			NetworkMessageEntityDestroy() = default;
			explicit NetworkMessageEntityDestroy(uint32 entity_id);
			bool read(NetworkStreamReader& reader);
			bool write(NetworkStreamWriter& writer);

			template <typename Stream>
			bool serialize(Stream& stream)
			{
				bool result = true;
				result &= stream.serialize(type_);
				result &= stream.serialize(entity_id_);
				return result;
			}

			uint8 type_ = NETWORK_MESSAGE_ENTITY_DESTROYED;
			uint8 entity_id_ = 0;
		};

		struct NetworkMessagePlayerDestroy
		{
			NetworkMessagePlayerDestroy() = default;
			explicit NetworkMessagePlayerDestroy(uint32 entity_id);
			bool read(NetworkStreamReader& reader);
			bool write(NetworkStreamWriter& writer);

			template <typename Stream>
			bool serialize(Stream& stream)
			{
				bool result = true;
				result &= stream.serialize(type_);
				result &= stream.serialize(entity_id_);
				return result;
			}

			uint8 type_ = NETWORK_MESSAGE_PLAYER_DESTROYED;
			uint8 entity_id_ = 0;
		};

		struct NetworkMessageLevelInfo
		{
			NetworkMessageLevelInfo() = default;
			NetworkMessageLevelInfo(uint8 level_id, uint8 size_x_, uint8 size_y_);
			bool read(NetworkStreamReader& reader);
			bool write(NetworkStreamWriter& writer);

			template <typename Stream>
			bool serialize(Stream& stream)
			{
				bool result = true;
				result &= stream.serialize(type_);
				result &= stream.serialize(level_id_);
				result &= stream.serialize(size_x_);
				result &= stream.serialize(size_y_);
				return result;
			}

			uint8 type_ = NETWORK_MESSAGE_LEVEL_INFO;
			uint8 level_id_ = 0;
			uint8 size_x_ = 0;
			uint8 size_y_ = 0;
		};

		struct NetworkMessageLevelDataRequest
		{
			NetworkMessageLevelDataRequest() = default;
			bool read(NetworkStreamReader& reader);
			bool write(NetworkStreamWriter& writer);

			template <typename Stream>
			bool serialize(Stream& stream)
			{
				bool result = true;
				result &= stream.serialize(type_);
				return result;
			}

			uint8 type_ = NETWORK_MESSAGE_LEVEL_REQUEST;
		};

		struct NetworkMessageLevelData
		{
			NetworkMessageLevelData() = default;
			explicit NetworkMessageLevelData(Tile level_tile);
			bool read(NetworkStreamReader& reader);
			bool write(NetworkStreamWriter& writer);

			template <typename Stream>
			bool serialize(Stream& stream)
			{
				bool result = true;
				result &= stream.serialize(type_);
				result &= stream.serialize(level_tile_);
				result &= stream.serialize(x_);
				result &= stream.serialize(y_);
				return result;
			}

			uint8 type_ = NETWORK_MESSAGE_LEVEL_DATA;
			uint8 level_tile_ = 0;
			uint8 x_ = 0;
			uint8 y_ = 0;
		};

		struct NetworkMessageMasterServer
		{
			NetworkMessageMasterServer() = default;
			explicit NetworkMessageMasterServer(uint8 a_, uint8 b_, uint8 c_, uint8 d_);
			bool read(NetworkStreamReader& reader);
			bool write(NetworkStreamWriter& writer);

			template <typename Stream>
			bool serialize(Stream& stream)
			{
				bool result = true;
				result &= stream.serialize(type_);
				result &= stream.serialize(a_);
				result &= stream.serialize(b_);
				result &= stream.serialize(c_);
				result &= stream.serialize(d_);
				return result;
			}

			uint8 type_ = NETWORK_MESSAGE_MASTER_SERVER;
			uint8 a_ = 0;
			uint8 b_ = 0;
			uint8 c_ = 0;
			uint8 d_ = 0;
		};
	} // !network
} // !charlie

#endif // !CHARLIE_MESSAGES_HPP_INCLUDED
