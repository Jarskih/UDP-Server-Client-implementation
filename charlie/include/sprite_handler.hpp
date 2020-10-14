#pragma once
#include "charlie.hpp"
#include "sdl_sprite.hpp"

namespace charlie
{
	class SpriteHandler
	{
		SDL_Renderer* renderer_;
		Map<const char*, SDL_Texture*> textures_;
		DynamicArray<SDLSprite*> sprites_;

	public:
		SpriteHandler();
		void set_renderer(SDL_Renderer* p_renderer);
		SDLSprite* create_sprite(const char* p_filePath, int p_x, int p_y, int p_w, int p_h);
	};
}
