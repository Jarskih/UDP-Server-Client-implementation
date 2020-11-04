#include "sdl_text_handler.h"

#include "sdl_font.h"

namespace charlie
{
	TextHandler::TextHandler() : renderer_(nullptr)
	{
	}

	void TextHandler::LoadFont(SDLFont& font)
	{
		font_ = font;
	}

	SDLSprite* TextHandler::CreateText(const std::string p_text, SDL_Color p_textColor, int p_x, int p_y, int p_w, int p_h) const
	{
		if (font_.font_ == nullptr)
		{
			SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "No font created while trying to create text %s", SDL_GetError());
			return nullptr;
		}

		if (renderer_ == nullptr)
		{
			SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "No renderer sent in the TextHandler %s", SDL_GetError());
		}

		SDL_Surface* textSurface = TTF_RenderText_Solid(font_.font_, p_text.c_str(), p_textColor);
		if (textSurface == nullptr)
		{
			SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Unable to render text surface! SDL_ttf Error: %s\n", TTF_GetError());
		}
		else
		{
			SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer_, textSurface);
			if (textTexture == nullptr)
			{
				SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Unable to create texture from rendered text! SDL Error: %s\n", SDL_GetError());
			}
			auto sprite = new SDLSprite(*textTexture, p_x, p_y, p_w, p_h);
			SDL_FreeSurface(textSurface);
			return sprite;
		}
		return nullptr;
	}

	TextHandler::~TextHandler()
	{
		font_.destroy();
		renderer_ = nullptr;
	}
}
