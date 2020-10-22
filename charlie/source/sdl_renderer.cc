#include "sdl_renderer.hpp"

namespace charlie
{
	charlie::SDLRenderer::SDLRenderer() : renderer_(nullptr)
	{
	}

	bool SDLRenderer::initialize(SDL_Window* window)
	{
		renderer_ = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
		if (renderer_ == nullptr) {
			SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Could not create SDL_renderer: %s", SDL_GetError());
			return false;
		}
		return true;
	}

	void SDLRenderer::shutdown()
	{
		SDL_DestroyRenderer(renderer_);
		renderer_ = nullptr;
	}

	SDL_Renderer* SDLRenderer::get_renderer() const
	{
		return renderer_;
	}

	void SDLRenderer::clear(const SDL_Color color) const
	{
		SDL_SetRenderDrawColor(renderer_, color.r, color.g, color.b, color.a);
		SDL_RenderClear(renderer_);
	}

	void SDLRenderer::present() const
	{
		SDL_RenderPresent(renderer_);
	}
}
