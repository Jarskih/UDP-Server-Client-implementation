#include "sdl_sprite.hpp"

namespace charlie
{
	SDLSprite::SDLSprite(SDL_Texture& p_texture, const int p_x, const int p_y, const int p_w, const int p_h)
	{
		texture_ = &p_texture;
		area_.x = p_x;
		area_.y = p_y;
		area_.w = p_w;
		area_.h = p_h;
	}

	SDLSprite::~SDLSprite()
	{
		if (texture_ != nullptr) {
			SDL_DestroyTexture(texture_);
		}
		texture_ = nullptr;
	}

	SDL_Texture* SDLSprite::get_texture() const
	{
		return texture_;
	}

	SDL_Rect SDLSprite::get_area() const
	{
		return area_;
	}
}
