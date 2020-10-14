#include <SDL_image.h>
#include "sprite_handler.hpp"
#include "sdl_sprite.hpp"

namespace charlie
{
	SpriteHandler::SpriteHandler() : renderer_(nullptr)
	{
	}

	void SpriteHandler::set_renderer(SDL_Renderer* p_renderer)
	{
		renderer_ = p_renderer;
	}


	SDLSprite* SpriteHandler::create_sprite(const char* p_filePath, int p_x, int p_y, int p_w = 0, int p_h = 0)
	{	
		const auto it = textures_.find(p_filePath);
		if (it == textures_.end())
		{
			SDL_Surface* surface = IMG_Load(p_filePath);
			if (surface == nullptr)
			{
				SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't load surface: %s", SDL_GetError());
				return nullptr;
			}
			SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer_, surface);

			if(p_w == 0)
			{
				p_w = p_h * surface->h/surface->w;
			} else if(p_h == 0)
			{
				p_h = p_w * surface->h/surface->w;
			}
			
			textures_[p_filePath] = texture;
			SDLSprite* const sprite = new SDLSprite(*texture, p_x, p_y, p_w, p_h);
			sprites_.push_back(sprite);
			SDL_FreeSurface(surface);
			return sprite;
		}
		else
		{
			auto* sprite = new SDLSprite(*textures_[p_filePath], p_x, p_y, p_w, p_h);
			sprites_.push_back(sprite);
			return sprite;
		}
		return nullptr;
	}
}
