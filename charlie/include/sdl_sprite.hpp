#pragma once

#include <memory>
#include <SDL.h>

namespace charlie
{
	class SDLSprite
	{
		SDL_Texture* texture_{};
		SDL_Rect area_{};
	public:
		SDLSprite(SDL_Texture& p_texture, int p_x, int p_y, int p_w, int p_h);
		void destroy();
		~SDLSprite();
		SDL_Texture* get_texture() const;
		SDL_Rect get_area() const;
	};
}
