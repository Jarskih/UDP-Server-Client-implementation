#include "sdl_font.h"
#include <SDL_ttf.h>

namespace charlie
{
	SDLFont::SDLFont() : font_(nullptr), color_()
	{
	}

	SDLFont::~SDLFont()
	{
	}

	void SDLFont::destroy()
	{
		if (font_ != nullptr)
		{
			TTF_CloseFont(font_);
			font_ = nullptr;
		}
	}

	void SDLFont::create(const char* font_path, int font_size, SDL_Color color)
	{
		color_ = color;
		font_ = TTF_OpenFont(font_path, font_size);
		if (font_ == nullptr)
		{
			SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Failed to load lazy font! SDL_ttf Error: %s\n", TTF_GetError());
		}
	}
}
