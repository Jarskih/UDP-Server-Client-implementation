// client_app.hpp

#ifndef CLIENT_APP_HPP_INCLUDED
#define CLIENT_APP_HPP_INCLUDED

#include <sdl_application.hpp>

#include "end.h"
#include "game.h"
#include "menu.h"
#include "master_server_client.h"

using namespace charlie;

struct ClientApp final : SDLApplication {
	ClientApp();

	enum class SceneState
	{
		MENU,
		GAME,
		END
	};

	// note: Application
	bool on_init() override;
	void on_exit() override;
	bool on_tick(const Time& dt) override;
	void on_draw() override;

	void set_state(SceneState state);

	MasterServerClient master_server_client_;
	SceneState state_;

	// Scenes
	Menu menu_;
	Game game_;
	End end_;
};

#endif // !CLIENT_APP_HPP_INCLUDED
