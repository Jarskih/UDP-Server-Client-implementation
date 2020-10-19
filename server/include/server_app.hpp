// server_app.hpp

#ifndef SERVER_APP_HPP_INCLUDED
#define SERVER_APP_HPP_INCLUDED

#include <sdl_application.hpp>

#include "level_manager.h"

using namespace charlie;

struct ClientList {
	ClientList()
		: next_(0)
	{
	}

	int32 add_client(const uint64 connection)
	{
		const int32 id = next_++;
		clients_.push_back({ id, connection });
		return id;
	}

	int32 find_client(const uint64 connection)
	{
		for (auto& client : clients_) {
			if (client.connection_ == connection) {
				return client.id_;
			}
		}
		return -1;
	}

	void remove_client(const uint64 connection)
	{
		auto it = clients_.begin();
		while (it != clients_.end()) {
			if ((*it).connection_ == connection) {
				clients_.erase(it);
				break;
			}
			++it;
		}
	}

	struct Client {
		int32  id_{ -1 };
		uint64 connection_{};
	};

	int32 next_;
	DynamicArray<Client> clients_;
};

struct Message
{
	uint16 seq_;
	uint32 id_;
};

struct Event
{
	uint32 id_;
	uint32 send_to_;
};


struct ServerApp final : SDLApplication, network::IServiceListener, network::IConnectionListener {
	ServerApp();

	// note: Application
	virtual bool on_init();
	virtual void on_exit();
	virtual bool on_tick(const Time& dt);
	virtual void on_draw();

	// note: IServiceListener
	virtual void on_timeout(network::Connection* connection);
	virtual void on_connect(network::Connection* connection);
	virtual void on_disconnect(network::Connection* connection);

	// note: IConnectionListener 
	virtual void on_acknowledge(network::Connection* connection, const uint16 sequence);
	virtual void on_receive(network::Connection* connection, network::NetworkStreamReader& reader);
	virtual void on_send(network::Connection* connection, const uint16 sequence, network::NetworkStreamWriter& writer);

	static void remove_from_array(DynamicArray<Event>& arr, uint32 id);
	static bool contains(const DynamicArray<uint32>& event, uint32 id);

	const Time tickrate_;
	Time accumulator_;
	uint32 tick_;
	ClientList clients_;
	DynamicArray<Player> players_;
	LevelManager level_manager_;

	Vector2 send_position_;
	Random random_;

	DynamicArray<Message> reliable_queue_;
	DynamicArray<Event> spawn_event_list;
	DynamicArray<Event> destroy_event__list_;
	uint32 index_;
};

#endif // !SERVER_APP_HPP_INCLUDED
