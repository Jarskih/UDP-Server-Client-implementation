#pragma once
#include <SDL.h>

namespace charlie
{
	struct SDLRenderer
	{
		SDLRenderer();
		bool initialize(SDL_Window* window);
		void shutdown();
		SDL_Renderer* get_renderer() const;
		void clear(const SDL_Color color) const;
		void present() const;
		SDL_Renderer* renderer_;
	};
}
