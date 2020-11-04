#include "end.h"

#include "config.h"
#include "Singleton.hpp"

charlie::End::End() : sprite_(nullptr), renderer_(nullptr)
{
}

bool charlie::End::on_init(SDL_Renderer* renderer)
{
	renderer_ = renderer;
	sprite_ = Singleton<SpriteHandler>::Get()->create_sprite(config::DEFEAT_SCREEN, 0, 0, config::SCREEN_WIDTH, config::SCREEN_HEIGHT);
	if (sprite_ == nullptr)
	{
		return false;
	}
	return true;
}

void charlie::End::on_exit()
{
	renderer_ = nullptr;
	sprite_ = nullptr;
}

bool charlie::End::on_tick(const Time& dt)
{
	Singleton<InputHandler>::Get()->HandleEvents();
	if (Singleton<InputHandler>::Get()->IsKeyPressed(SDL_SCANCODE_SPACE) || Singleton<InputHandler>::Get()->IsKeyPressed(SDL_SCANCODE_KP_ENTER))
	{
		return false;
	}
	return true;
}

void charlie::End::on_draw()
{
	SDL_RenderCopy(renderer_, sprite_->get_texture(), nullptr, nullptr);
}
