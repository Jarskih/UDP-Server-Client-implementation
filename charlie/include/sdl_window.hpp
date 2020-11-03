#pragma once
#include "SDL.h"
#include "charlie.hpp"

namespace charlie {
	struct Settings;
}

namespace charlie
{
	struct SDLWindow {
		SDLWindow();

		bool open(const charlie::Settings& settings);
		void close();

		bool update();
		void present();
		SDL_Window* get_window();

		SDL_Window* window_;
		int32 width_;
		int32 height_;
	};
}
