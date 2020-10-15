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
			: type_(NETWORK_MESSAGE_ENTITY_STATE), id_(0)
		{
		}

		NetworkMessageEntityState::NetworkMessageEntityState(const Transform& transform, uint32 id)
			: type_(NETWORK_MESSAGE_ENTITY_STATE)
			, position_(transform.position_)
			, rotation_(transform.rotation_)
			, id_(id)
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
		{
		}

		NetworkMessageInputCommand::NetworkMessageInputCommand(uint8 bits)
			: type_(NETWORK_MESSAGE_INPUT_COMMAND)
			, bits_(bits)
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
		{
		}

		NetworkMessagePlayerState::NetworkMessagePlayerState(const Transform& transform)
			: type_(NETWORK_MESSAGE_PLAYER_STATE)
			, position_(transform.position_)
			, rotation_(transform.rotation_)
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

		NetworkMessagePlayerSpawn::NetworkMessagePlayerSpawn() : type_(NETWORK_MESSAGE_PLAYER_SPAWN), id_(0)
		{
		}

		NetworkMessagePlayerSpawn::NetworkMessagePlayerSpawn(const Vector2& position, const uint32 id) :
			type_(NETWORK_MESSAGE_PLAYER_SPAWN), position_(position), id_(id)
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
	} // !network
} // !messages
