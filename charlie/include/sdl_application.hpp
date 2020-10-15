// charlie_application.hpp

#ifndef CHARLIE_APPLICATION_HPP_INCLUDED
#define CHARLIE_APPLICATION_HPP_INCLUDED

#include <charlie.hpp>
#include <charlie_gameplay.hpp>
#include <charlie_network.hpp>
#include <charlie_networkinfo.hpp>

#include "player.hpp"
#include "sdl_renderer.hpp"
#include "sdl_window.hpp"
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <SDL_mixer.h>
#include <SDL.h>


#include "Camera.h"
#include "input_handler.h"
#include "sprite_handler.hpp"

namespace charlie {
	struct SDLApplication {
		SDLApplication() = default;

		bool init();
		void run();
		void exit();

		virtual bool on_init();
		virtual void on_exit();
		virtual bool on_tick(const charlie::Time& dt);
		virtual void on_draw();

		SDLWindow window_;
		SDLRenderer renderer_;
		Camera camera_;
		charlie::network::Service network_;
		InputHandler input_handler_;
		SpriteHandler sprite_handler_;
	};
} // !charlies

#endif // !CHARLIE_APPLICATION_HPP_INCLUDED