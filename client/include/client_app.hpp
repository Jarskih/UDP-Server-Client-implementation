// client_app.hpp

#ifndef CLIENT_APP_HPP_INCLUDED
#define CLIENT_APP_HPP_INCLUDED

#include <sdl_application.hpp>
#include "entity.h"
#include "game.h"
#include "menu.h"

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
	virtual bool on_init();
	virtual void on_exit();
	virtual bool on_tick(const Time& dt);
	virtual void on_draw();

	void set_state(SceneState state);

	Menu menu_;
	Game game_;
	Menu end_;

	SceneState state_;
};

#endif // !CLIENT_APP_HPP_INCLUDED
