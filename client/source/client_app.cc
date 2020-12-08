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

	return true;
}

void ClientApp::on_exit()
{
	menu_.on_exit();
	game_.on_exit();
}

bool ClientApp::on_tick(const Time& dt)
{
	master_server_client_.update(dt);
	if (master_server_client_.is_looking_for_server_)
	{
		return true;
	}

	menu_.servers_available_ = true;
	game_.server_ = master_server_client_.game_server_;
	if (state_ == SceneState::MENU)
	{
		set_state(SceneState::GAME);
	}


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