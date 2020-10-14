#include "player.hpp"



#include "charlie_gameplay.hpp"
#include "input_handler.h"
#include "SDLKeyboard.h"
#include "Singleton.hpp"
#include "sprite_handler.hpp"

namespace charlie
{
	Player::Player() : window_rect_(), input_bits_(0), id_(0), speed_(100), sprite_(nullptr), renderer_(nullptr)
	{
	}

	void Player::init(SDL_Renderer* renderer, Vector2& pos, uint32 id)
	{
		renderer_ = renderer;
		position_ = pos;
		id_ = id;
	}

	void Player::update(Time deltaTime)
	{
		Vector2 direction;
		input_bits_ = 0;

		if (Singleton<InputHandler>::Get()->IsKeyDown(SDL_SCANCODE_W)) {
			input_bits_ |= (1 << int32(gameplay::Action::Up));
			direction.y_ -= 1.0f;
		}
		if (Singleton<InputHandler>::Get()->IsKeyDown(SDL_SCANCODE_S)) {
			input_bits_ |= (1 << int32(gameplay::Action::Down));
			direction.y_ += 1.0f;
		}
		if (Singleton<InputHandler>::Get()->IsKeyDown(SDL_SCANCODE_A)) {
			input_bits_ |= (1 << int32(gameplay::Action::Left));
			direction.x_ -= 1.0f;
		}
		if (Singleton<InputHandler>::Get()->IsKeyDown(SDL_SCANCODE_D)) {
			input_bits_ |= (1 << int32(gameplay::Action::Right));
			direction.x_ += 1.0f;
		}

		if (direction.length() > 0.0f) {
			direction.normalize();
			position_ += direction * speed_ * deltaTime.as_seconds();
		}
	}

	void Player::render()
	{
		window_rect_.x = (int)position_.x_;
		window_rect_.y = (int)position_.y_;
		SDL_RenderCopy(renderer_, sprite_->get_texture(), nullptr, &window_rect_);
	}

	void Player::load_sprite(const char* path, int srcX, int srcY, int srcW, int srcH)
	{
		sprite_ = Singleton<SpriteHandler>::Get()->create_sprite(path, srcX, srcY, srcW, srcH);
		window_rect_ = { 0, 0, 50, 50 };
	}

	uint8 Player::get_input_bits() const
	{
		return input_bits_;
	}
}
