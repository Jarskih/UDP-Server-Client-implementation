#pragma once
#include <SDL_ttf.h>

namespace charlie
{
	struct SDLFont
	{
		TTF_Font* font_;
		SDL_Color color_;
		SDLFont();
		explicit SDLFont(const char* p_font_path, int p_font_size, SDL_Color p_color);
		~SDLFont();
	};
}
