// charlie_messages.hpp

#ifndef CHARLIE_MESSAGES_HPP_INCLUDED
#define CHARLIE_MESSAGES_HPP_INCLUDED

#include <charlie.hpp>

namespace charlie {
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
			NETWORK_MESSAGE_PROJECTILE_SPAWN,
			NETWORK_MESSAGE_DISCONNECTED,
			NETWORK_MESSAGE_PLAYER_DESTROYED,
			NETWORK_MESSAGE_ENTITY_DESTROYED,
			NETWORK_MESSAGE_PROJECTILE_DESTROYED,
			NETWORK_MESSAGE_ACK,
			NETWORK_MESSAGE_COUNT,
		};

		static_assert(NETWORK_MESSAGE_COUNT <= 255, "network message type cannot exceed 255!");

		struct NetworkMessageServerTick {
			NetworkMessageServerTick();
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

			uint8 type_;
			int64 server_time_;
			uint32 server_tick_;
		};

		struct NetworkMessageEntityState {
			NetworkMessageEntityState();
			explicit NetworkMessageEntityState(const Transform& transform, float turret_rotation, uint32 entity_id);

			bool read(NetworkStreamReader& reader);
			bool write(NetworkStreamWriter& writer);

			template <typename Stream>
			bool serialize(Stream& stream)
			{
				bool result = true;
				result &= stream.serialize(type_);
				result &= stream.serialize(position_.x_);
				result &= stream.serialize(position_.y_);
				result &= stream.serialize(rotation_);
				result &= stream.serialize(entity_id_);
				result &= stream.serialize(turret_rotation_);
				return result;
			}

			uint8 type_;
			Vector2 position_;
			float rotation_;
			uint32 entity_id_;
			float turret_rotation_;
		};

		struct NetworkMessageInputCommand {
			NetworkMessageInputCommand();
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

			uint8 type_;
			uint8 bits_;
			float rot_;
			bool fire_;
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
			float rotation_;
			uint16 y_;
			uint16 x_;
			float turret_rotation_;
		};

		// Reliable messages

		struct NetworkMessageAck
		{
			NetworkMessageAck();
			explicit NetworkMessageAck(const uint32 id);
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
			uint32 event_id_;
		};

		struct NetworkMessagePlayerSpawn
		{
			NetworkMessagePlayerSpawn();
			explicit NetworkMessagePlayerSpawn(uint32 event_id, uint32 entity_id, const Vector2& position);
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
			uint32 entity_id_;
			uint32 event_id_;
		};

		struct NetworkMessageEntitySpawn
		{
			NetworkMessageEntitySpawn();
			explicit NetworkMessageEntitySpawn(uint32 event_id, uint32 entity_id, const Vector2& position);
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
			uint32 entity_id_;
			uint32 event_id_;
		};

		struct NetworkMessageProjectileSpawn
		{
			NetworkMessageProjectileSpawn();
			explicit NetworkMessageProjectileSpawn(uint32 event_id, uint32 entity_id, uint32 shot_by, const Vector2& position, float rotation);
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
			uint32 entity_id_;
			uint32 shot_by_;
			uint32 event_id_;
			Vector2 pos_;
			float rotation_;
		};

		struct NetworkMessagePlayerDisconnected
		{
			NetworkMessagePlayerDisconnected();
			explicit NetworkMessagePlayerDisconnected(uint32 entity_id, uint32 message_id);
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
			uint32 entity_id_;
			uint32 message_id_;
		};

		struct NetworkMessageProjectileDestroy
		{
			NetworkMessageProjectileDestroy();
			explicit NetworkMessageProjectileDestroy(uint32 entity_id, uint32 event_id);
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
			uint32 entity_id_;
			uint32 event_id_;
		};

		struct NetworkMessageEntityDestroy
		{
			NetworkMessageEntityDestroy();
			explicit NetworkMessageEntityDestroy(uint32 entity_id, uint32 event_id);
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
			uint32 entity_id_;
			uint32 event_id_;
		};

		struct NetworkMessagePlayerDestroy
		{
			NetworkMessagePlayerDestroy();
			explicit NetworkMessagePlayerDestroy(uint32 entity_id, uint32 event_id);
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
			uint32 entity_id_;
			uint32 event_id_;
		};
	} // !network
} // !charlie

#endif // !CHARLIE_MESSAGES_HPP_INCLUDED
