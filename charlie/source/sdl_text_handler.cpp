#include "sdl_text_handler.h"

#include <memory>


#include "sdl_font.h"

namespace charlie
{
	TextHandler::TextHandler() : renderer_(nullptr)
	{
	}

	void TextHandler::LoadFont(const SDLFont& font)
	{
		font_ = font;
	}

	std::unique_ptr<SDLSprite> TextHandler::CreateText(const std::string p_text, const SDL_Color p_textColor, const int p_x, const int p_y, const int p_w, const int p_h) const
	{
		if (font_.font_ == nullptr)
		{
			SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "No font created while trying to create text %s", SDL_GetError());
			return nullptr;
		}

		if (renderer_ == nullptr)
		{
			SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "No renderer sent in the TextHandler %s", SDL_GetError());
			return nullptr;
		}

		SDL_Surface* textSurface = TTF_RenderText_Solid(font_.font_, p_text.c_str(), p_textColor);
		if (textSurface == nullptr)
		{
			SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Unable to render text surface! SDL_ttf Error: %s\n", TTF_GetError());
			return nullptr;
		}
		else
		{
			SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer_, textSurface);
			if (textTexture == nullptr)
			{
				SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Unable to create texture from rendered text! SDL Error: %s\n", SDL_GetError());
				return nullptr;
			}
			auto sprite = std::make_unique<SDLSprite>(*textTexture, p_x, p_y, p_w, p_h);
			SDL_FreeSurface(textSurface);
			return sprite;
		}
	}

	TextHandler::~TextHandler()
	{
		font_.destroy();
		renderer_ = nullptr;
	}
}
