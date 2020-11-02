// charlie_application.hpp

#ifndef CHARLIE_APPLICATION_HPP_INCLUDED
#define CHARLIE_APPLICATION_HPP_INCLUDED

#include <charlie.hpp>
#include <charlie_gameplay.hpp>
#include <charlie_network.hpp>
#include <charlie_networkinfo.hpp>
#include "sdl_renderer.hpp"
#include "sdl_window.hpp"
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <SDL_mixer.h>
#include <SDL.h>
#include "input_handler.h"
#include "sprite_handler.hpp"

namespace charlie {
	struct Player;

	namespace Color
	{
		const static SDL_Color white = { 255, 255, 255, 255 };
		const static SDL_Color Black = { 0, 0, 0, 255 };
	}

	struct Camera
	{
		Camera();
		void init(int level_width, int level_height, SDL_Rect rect);
		SDL_Rect rect_;
		int level_width_;
		int level_heigth_;
		void lookAt(Player& player);
	};

	struct SDLApplication {
		SDLApplication();
		virtual ~SDLApplication() = default;

		bool init();
		void run();
		void exit();

		virtual bool on_init();
		virtual void on_exit();
		virtual bool on_tick(const charlie::Time& dt);
		virtual void on_draw();

		SDLWindow window_;
		SDLRenderer renderer_;
		//SDL_Rect cam_;
		network::Service network_;
		InputHandler input_handler_;
		SpriteHandler sprite_handler_;
		int level_width_;
		int level_heigth_;
	};
} // !charlies

#endif // !CHARLIE_APPLICATION_HPP_INCLUDED