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
			NetworkMessageServerTick();
			explicit NetworkMessageServerTick(int64  server_time,
				const int32 server_tick);

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

			uint8 type_;
			int64 server_time_;
			int32 server_tick_;
		};

		struct NetworkMessageEntityState {
			NetworkMessageEntityState();
			explicit NetworkMessageEntityState(const Transform& transform, float turret_rotation, int32 entity_id);

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

			uint8 type_;
			int16 x_;
			int16 y_;
			int16 rotation_;
			int8 entity_id_;
			int16 turret_rotation_;
		};

		struct NetworkMessageInputCommand {
			NetworkMessageInputCommand();
			explicit NetworkMessageInputCommand(uint8 bits, float rotation, bool fire, int32 tick);

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
				result &= stream.serialize(tick_);
				return result;
			}

			uint8 type_;
			uint8 bits_;
			int16 rot_;
			bool fire_;
			uint32 tick_;
		};

		struct NetworkMessagePlayerState {
			NetworkMessagePlayerState();
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

			uint8 type_;
			int16 rotation_;
			int16 y_;
			int16 x_;
			int16 turret_rotation_;
		};

		// Reliable messages

		struct NetworkMessageAck
		{
			NetworkMessageAck();
			explicit NetworkMessageAck(const int32 id);
			bool read(NetworkStreamReader& reader);
			bool write(NetworkStreamWriter& writer);

			template <typename Stream>
			bool serialize(Stream& stream)
			{
				bool result = true;
				result &= stream.serialize(type_);
				result &= stream.serialize(event_id_);
				return result;
			}

			uint8 type_;
			int32 event_id_;
		};

		struct NetworkMessagePlayerSpawn
		{
			NetworkMessagePlayerSpawn();
			explicit NetworkMessagePlayerSpawn(int32 event_id, int32 entity_id, const Vector2& position);
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
				result &= stream.serialize(event_id_);
				return result;
			}

			uint8 type_;
			Vector2 position_;
			uint8 entity_id_;
			int32 event_id_;
		};

		struct NetworkMessageEntitySpawn
		{
			NetworkMessageEntitySpawn();
			explicit NetworkMessageEntitySpawn(int32 event_id, int32 entity_id, const Vector2& position);
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
				result &= stream.serialize(event_id_);
				return result;
			}

			uint8 type_;
			Vector2 position_;
			uint8 entity_id_;
			int32 event_id_;
		};

		struct NetworkMessageProjectileSpawn
		{
			NetworkMessageProjectileSpawn();
			explicit NetworkMessageProjectileSpawn(int32 event_id, int32 entity_id, int32 shot_by, const Vector2& position, float rotation);
			bool read(NetworkStreamReader& reader);
			bool write(NetworkStreamWriter& writer);

			template <typename Stream>
			bool serialize(Stream& stream)
			{
				bool result = true;
				result &= stream.serialize(type_);
				result &= stream.serialize(entity_id_);
				result &= stream.serialize(shot_by_);
				result &= stream.serialize(event_id_);
				result &= stream.serialize(pos_.x_);
				result &= stream.serialize(pos_.y_);
				result &= stream.serialize(rotation_);
				return result;
			}

			uint8 type_;
			uint8 entity_id_;
			uint8 shot_by_;
			uint32 event_id_;
			Vector2 pos_;
			float rotation_;
		};

		struct NetworkMessagePlayerDisconnected
		{
			NetworkMessagePlayerDisconnected();
			explicit NetworkMessagePlayerDisconnected(int32 entity_id, int32 message_id);
			bool read(NetworkStreamReader& reader);
			bool write(NetworkStreamWriter& writer);

			template <typename Stream>
			bool serialize(Stream& stream)
			{
				bool result = true;
				result &= stream.serialize(type_);
				result &= stream.serialize(entity_id_);
				result &= stream.serialize(message_id_);
				return result;
			}

			uint8 type_;
			uint8 entity_id_;
			uint32 message_id_;
		};

		struct NetworkMessageProjectileDestroy
		{
			NetworkMessageProjectileDestroy();
			explicit NetworkMessageProjectileDestroy(int32 entity_id, int32 event_id);
			bool read(NetworkStreamReader& reader);
			bool write(NetworkStreamWriter& writer);

			template <typename Stream>
			bool serialize(Stream& stream)
			{
				bool result = true;
				result &= stream.serialize(type_);
				result &= stream.serialize(entity_id_);
				result &= stream.serialize(event_id_);
				return result;
			}

			uint8 type_;
			uint8 entity_id_;
			int32 event_id_;
		};

		struct NetworkMessageEntityDestroy
		{
			NetworkMessageEntityDestroy();
			explicit NetworkMessageEntityDestroy(int32 entity_id, int32 event_id);
			bool read(NetworkStreamReader& reader);
			bool write(NetworkStreamWriter& writer);

			template <typename Stream>
			bool serialize(Stream& stream)
			{
				bool result = true;
				result &= stream.serialize(type_);
				result &= stream.serialize(entity_id_);
				result &= stream.serialize(event_id_);
				return result;
			}

			uint8 type_;
			uint8 entity_id_;
			int32 event_id_;
		};

		struct NetworkMessagePlayerDestroy
		{
			NetworkMessagePlayerDestroy();
			explicit NetworkMessagePlayerDestroy(int32 entity_id, int32 event_id);
			bool read(NetworkStreamReader& reader);
			bool write(NetworkStreamWriter& writer);

			template <typename Stream>
			bool serialize(Stream& stream)
			{
				bool result = true;
				result &= stream.serialize(type_);
				result &= stream.serialize(entity_id_);
				result &= stream.serialize(event_id_);
				return result;
			}

			uint8 type_;
			uint8 entity_id_;
			int32 event_id_;
		};

		struct NetworkMessageLevelInfo
		{
			NetworkMessageLevelInfo();
			explicit NetworkMessageLevelInfo(uint8 level_id, uint8 size_x_, uint8 size_y_, int32 event_id);
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
				result &= stream.serialize(event_id_);
				return result;
			}

			uint8 type_;
			uint8 level_id_;
			uint8 size_x_;
			uint8 size_y_;
			int32 event_id_;
		};

		struct NetworkMessageLevelDataRequest
		{
			NetworkMessageLevelDataRequest();
			explicit NetworkMessageLevelDataRequest(int32 event_id_);
			bool read(NetworkStreamReader& reader);
			bool write(NetworkStreamWriter& writer);

			template <typename Stream>
			bool serialize(Stream& stream)
			{
				bool result = true;
				result &= stream.serialize(type_);
				result &= stream.serialize(event_id_);
				return result;
			}

			uint8 type_;
			int32 event_id_;
		};

		struct NetworkMessageLevelData
		{
			NetworkMessageLevelData();
			explicit NetworkMessageLevelData(Tile level_tile, int32 event_id);
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
				result &= stream.serialize(event_id_);
				return result;
			}

			uint8 type_;
			uint8 level_tile_;
			uint8 x_;
			uint8 y_;
			int32 event_id_;
		};

		struct NetworkMessageMasterServer
		{
			NetworkMessageMasterServer();
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

			uint8 type_;
			uint8 a_;
			uint8 b_;
			uint8 c_;
			uint8 d_;
		};
	} // !network
} // !charlie

#endif // !CHARLIE_MESSAGES_HPP_INCLUDED
