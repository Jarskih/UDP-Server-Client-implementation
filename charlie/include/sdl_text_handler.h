#pragma once
#include <SDL.h>
#include <SDL_ttf.h>
#include <string>
#include "sdl_sprite.hpp"

namespace charlie
{
	struct TextHandler
	{
		SDL_Renderer* m_renderer;
		TextHandler(SDL_Renderer* p_renderer);
		SDLSprite* CreateText(const std::string& p_text, SDL_Color p_textColor, TTF_Font* p_font, int p_x, int p_y, int p_w,
			int p_h) const;
		~TextHandler();
	};
}
