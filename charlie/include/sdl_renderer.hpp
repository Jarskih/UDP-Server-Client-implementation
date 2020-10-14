#pragma once
#include <SDL.h>

#include "charlie.hpp"

namespace charlie
{
	struct SDLRenderer
	{
		SDLRenderer();
		bool initialize(SDL_Window* window);
		void shutdown();
		SDL_Renderer* get_renderer() const;
		void clear(const Color& color) const;
		void present() const;
		SDL_Renderer* renderer_;
	};
}
