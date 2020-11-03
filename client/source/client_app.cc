// client_app.cc

#include "client_app.hpp"

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
	network_.set_send_rate(Time(1.0 / 60.0));
	if (!network_.initialize({})) {
		return false;
	}

	menu_.on_init(renderer_.get_renderer());
	state_ = SceneState::MENU;

	return true;
}

void ClientApp::on_exit()
{
	menu_.on_exit();
	game_.on_exit();
}

bool ClientApp::on_tick(const Time& dt)
{
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