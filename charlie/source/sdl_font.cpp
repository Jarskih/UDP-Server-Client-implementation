#include "sdl_font.h"
#include <SDL_ttf.h>

namespace charlie
{
	SDLFont::SDLFont() : font_(nullptr), color_()
	{
	}

	SDLFont::SDLFont(const char* p_font_path, int p_font_size, SDL_Color p_color) : font_(), color_()
	{
		color_ = p_color;
		font_ = TTF_OpenFont(p_font_path, p_font_size);
		if (font_ == nullptr)
		{
			SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Failed to load lazy font! SDL_ttf Error: %s\n", TTF_GetError());
		}
	}

	SDLFont::~SDLFont()
	{
		SDL_Log("Font::~Font");
		if (font_ != nullptr)
		{
			TTF_CloseFont(font_);
			font_ = nullptr;
		}
	}
}
