#pragma once
#include <SDL_ttf.h>
#include <string>

namespace charlie
{
	struct SDLFont
	{
		TTF_Font* font_;
		SDL_Color color_;
		SDLFont();
		void create(std::string font_path, int font_size, SDL_Color color);
		~SDLFont();
		void destroy();
	};
}
