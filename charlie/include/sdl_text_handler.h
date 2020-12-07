#pragma once
#include <memory>
#include <SDL.h>
#include <SDL_ttf.h>
#include <string>

#include "sdl_font.h"
#include "sdl_sprite.hpp"

namespace charlie
{
	struct TextHandler
	{

		TextHandler();
		void LoadFont(const SDLFont& font);
		std::unique_ptr<SDLSprite> CreateText(std::string p_text, SDL_Color p_textColor, int p_x, int p_y, int p_w,
			int p_h) const;
		~TextHandler();

		SDL_Renderer* renderer_;
		SDLFont font_;
	};
}
