#include "sdl_text_handler.h"

namespace charlie
{
	TextHandler::TextHandler(SDL_Renderer* p_renderer) : m_renderer()
	{
		if (p_renderer == nullptr)
		{
			SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "No renderer sent in the TextHandler::Constructor %s", SDL_GetError());
		}
		m_renderer = p_renderer;
	}

	SDLSprite* TextHandler::CreateText(const std::string& p_text, SDL_Color p_textColor, TTF_Font* p_font, int p_x, int p_y, int p_w, int p_h) const
	{
		SDL_Surface* textSurface = TTF_RenderText_Solid(p_font, p_text.c_str(), p_textColor);
		if (textSurface == nullptr)
		{
			SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Unable to render text surface! SDL_ttf Error: %s\n", TTF_GetError());
		}
		else
		{
			SDL_Texture* textTexture = SDL_CreateTextureFromSurface(m_renderer, textSurface);
			if (textTexture == nullptr)
			{
				SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Unable to create texture from rendered text! SDL Error: %s\n", SDL_GetError());
			}
			SDL_FreeSurface(textSurface);
			const auto sprite = new SDLSprite(*textTexture, p_x, p_y, p_w, p_h);
			return sprite;
		}
		return nullptr;
	}

	TextHandler::~TextHandler()
	{
		m_renderer = nullptr;
	}

}
