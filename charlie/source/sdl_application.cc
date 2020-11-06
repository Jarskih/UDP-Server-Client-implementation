// charlie_application.cc

#include "sdl_application.hpp"

#include "config.h"
#include "player.hpp"
#include "Singleton.hpp"

namespace charlie {
	Camera::Camera() : rect_(), level_width_(0), level_heigth_(0)
	{
	}

	void Camera::init(int level_width, int level_height, SDL_Rect rect) {
		level_width_ = level_width;
		level_heigth_ = level_height;
		rect_ = rect;
	}

	void Camera::lookAt(Player& player)
	{
		rect_.x = (int)player.transform_.position_.x_ + player.body_sprite_->get_area().w / 2 - config::SCREEN_WIDTH / 2;
		rect_.y = (int)player.transform_.position_.y_ + player.body_sprite_->get_area().h / 2 - config::SCREEN_HEIGHT / 2;

		if (rect_.x < 0)
		{
			rect_.x = 0;
		}
		if (rect_.y < 0)
		{
			rect_.y = 0;
		}
		if (rect_.x > level_width_ - rect_.w)
		{
			rect_.x = level_width_ - rect_.w;
		}
		if (rect_.y > level_heigth_ - rect_.h)
		{
			rect_.y = level_heigth_ - rect_.h;
		}
	}

	SDLApplication::SDLApplication() : level_width_(0), level_heigth_(0)
	{
	}

	bool SDLApplication::init()
	{
		if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
			SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Could not initialize SDL: %s", SDL_GetError());
		}
		if (Mix_Init(MIX_INIT_MP3) == 0) {
			SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Could not initialize SDL_mixer: %s", Mix_GetError());
		}
		if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 1024) == -1) {
			SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Could not initialize OpenAudio: %s", Mix_GetError());
		}
		if (IMG_Init(IMG_INIT_JPG | IMG_INIT_PNG) == 0)
		{
			SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Could not initialize IMG: %s", IMG_GetError());
		}
		if (TTF_Init() == -1)
		{
			SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "SDL_ttf could not initialize! SDL_ttf Error: %s\n", TTF_GetError());
		}

		if (!window_.open(Settings("tank game", 640, 360)))
		{
			SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Could not create Window: %s", SDL_GetError());
		}

		if (!renderer_.initialize(window_.get_window()))
		{
			SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Could not create renderer: %s", SDL_GetError());
		}

		sprite_handler_.set_renderer(renderer_.get_renderer());
		Singleton<SpriteHandler>::Set(&sprite_handler_);
		Singleton<InputHandler>::Set(&input_handler_);

		return on_init();
	}

	void SDLApplication::run()
	{
		bool running = true;

		while (running) {
			if (!window_.update()) {
				running = false;
			}

			network_.update();

			const auto dt = Time::deltatime();
			if (!on_tick(dt)) {
				running = false;
			}

			renderer_.clear(Color::Black);
			on_draw();
			renderer_.present();
		}
	}

	void SDLApplication::exit()
	{
		on_exit();

		network_.shutdown();
		renderer_.shutdown();
		window_.close();
	}

	bool SDLApplication::on_init()
	{
		return true;
	}

	void SDLApplication::on_exit()
	{
	}

	bool SDLApplication::on_tick(const charlie::Time& dt)
	{
		return true;
	}

	void SDLApplication::on_draw()
	{
	}

} // !charlie
