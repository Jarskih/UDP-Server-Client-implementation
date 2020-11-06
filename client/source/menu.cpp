#include "menu.h"
#include "config.h"
#include "Singleton.hpp"

namespace charlie
{
	Menu::Menu() : sprite_(nullptr), renderer_(nullptr), servers_available_(false)
	{
	}

	bool Menu::on_init(SDL_Renderer* renderer)
	{
		renderer_ = renderer;
		sprite_ = Singleton<SpriteHandler>::Get()->create_sprite(config::MENU_SCREEN, 0, 0, config::SCREEN_WIDTH, config::SCREEN_HEIGHT);
		if (sprite_ == nullptr)
		{
			return false;
		}
		return true;
	}

	void Menu::on_exit()
	{
		renderer_ = nullptr;
		sprite_ = nullptr;
	}

	bool Menu::on_tick(const Time& dt)
	{
		Singleton<InputHandler>::Get()->HandleEvents();
		if (Singleton<InputHandler>::Get()->IsKeyPressed(SDL_SCANCODE_SPACE) || Singleton<InputHandler>::Get()->IsKeyPressed(SDL_SCANCODE_KP_ENTER))
		{
			if (servers_available_)
			{
				return false;
			}
		}

		return true;
	}

	void Menu::on_draw()
	{
		SDL_RenderCopy(renderer_, sprite_->get_texture(), nullptr, nullptr);
	}

}
