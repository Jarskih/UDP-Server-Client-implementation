// client_app.cc

#include "client_app.hpp"
#include "config.h"

template <typename T, std::size_t N>
constexpr auto array_size(T(&)[N])
{
	return N;
}

ClientApp::ClientApp() : state_()
{
}

bool ClientApp::on_init()
{
	menu_.on_init(renderer_.get_renderer());
	state_ = SceneState::MENU;

	network_.set_send_rate(Time(1.0 / 60.0));
	if (!network_.initialize({})) {
		return false;
	}

	//master_server_client_.request_server();

	return true;
}

void ClientApp::on_exit()
{
	menu_.on_exit();
	game_.on_exit();
}

bool ClientApp::on_tick(const Time& dt)
{
	if (master_server_client_.receive_messages())
	{
		menu_.servers_available_ = true;
		game_.server_ = master_server_client_.server_;
	}

	menu_.servers_available_ = true;
	game_.server_ = network::IPAddress(config::IP_A, config::IP_B, config::IP_C, config::IP_D, config::PORT);

	switch (state_) {
	case(SceneState::MENU):
	{
		if (!menu_.on_tick(dt))
		{
			set_state(SceneState::GAME);
		}
	} break;
	case(SceneState::GAME):
	{
		if (!game_.on_tick(dt))
		{
			set_state(SceneState::END);
		}
	} break;
	case(SceneState::END):
	{
		if (!end_.on_tick(dt))
		{
			return false;
		}
	} break;
	}
	return true;
}

void ClientApp::on_draw()
{
	switch (state_) {
	case(SceneState::MENU):
	{
		menu_.on_draw();
	}break;
	case(SceneState::GAME):
	{
		game_.on_draw();
	}break;
	case(SceneState::END):
	{
		end_.on_draw();
	}break;
	}
}

void ClientApp::set_state(SceneState state)
{
	switch (state_) {
	case(SceneState::MENU):
	{
		menu_.on_exit();
	}break;
	case(SceneState::GAME):
	{
		game_.on_exit();
	}break;
	case(SceneState::END):
	{
		end_.on_exit();
	}break;
	}

	state_ = state;

	switch (state_) {
	case(SceneState::MENU):
	{
		menu_.on_init(renderer_.get_renderer());
	}break;
	case(SceneState::GAME):
	{
		game_.on_init(renderer_.get_renderer());
	}break;
	case(SceneState::END):
	{
		end_.on_init(renderer_.get_renderer());
	}break;
	}
}