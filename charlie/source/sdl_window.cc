#include "sdl_window.hpp"

namespace charlie
{
	SDLWindow::SDLWindow() : window_(nullptr), width_(0), height_(0)
	{
	}

	bool SDLWindow::open(const charlie::Settings& settings)
	{
		height_ = settings.height_;
		width_ = settings.width_;
		window_ = SDL_CreateWindow(settings.title_, SDL_WINDOWPOS_UNDEFINED,
			SDL_WINDOWPOS_UNDEFINED, width_, height_, SDL_WINDOW_SHOWN);
		if (window_ == nullptr) {
			SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Could not create SDL_Window: %s", SDL_GetError());
			return false;
		}
		return true;
	}

	void SDLWindow::close()
	{
		SDL_DestroyWindow(window_);
		window_ = nullptr;
	}

	bool SDLWindow::update()
	{
		return true;
	}

	void SDLWindow::present()
	{
	}

	SDL_Window* SDLWindow::get_window()
	{
		return window_;
	}
}
