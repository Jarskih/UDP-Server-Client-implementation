// client_app.hpp

#ifndef CLIENT_APP_HPP_INCLUDED
#define CLIENT_APP_HPP_INCLUDED

#include <charlie_application.hpp>

using namespace charlie;

struct ClientApp final : Application, network::IConnectionListener {
	ClientApp();

	// note: Application
	virtual bool on_init();
	virtual void on_exit();
	virtual bool on_tick(const Time& dt);
	virtual void on_draw();

	// note: IConnectionListener 
	virtual void on_acknowledge(network::Connection* connection, const uint16 sequence);
	virtual void on_receive(network::Connection* connection, network::NetworkStreamReader& reader);
	virtual void on_send(network::Connection* connection, const uint16 sequence, network::NetworkStreamWriter& writer);

	Mouse& mouse_;
	Keyboard& keyboard_;
	network::Connection connection_;
	const Time tickrate_;
	Time accumulator_;
	Time rtt_;
	Time lastSend_;
	Time lastReceive_;

	uint8 input_bits_;
	gameplay::Player player_;
	gameplay::Entity entity_;

	gameplay::Interpolator interpolator_;
	Vector2 oldPos_;
	Vector2 newPos_;
	uint32 tick_;
	Vector2 playerPos_;
};

#endif // !CLIENT_APP_HPP_INCLUDED
