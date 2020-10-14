#pragma once
#include "sdl_sprite.hpp"

namespace charlie
{
	SDLSprite::SDLSprite(SDL_Texture& p_texture, int p_x, int p_y, int p_w, int p_h)
	{
		texture_ = &p_texture;
		area_.x = p_x;
		area_.y = p_y;
		area_.w = p_w;
		area_.h = p_h;
	}

	SDL_Texture* SDLSprite::get_texture()
	{
		return texture_;
	}

	SDL_Rect SDLSprite::get_area()
	{
		return area_;
	}
}
