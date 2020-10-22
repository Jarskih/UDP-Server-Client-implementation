#include "player.hpp"


#include "camera.h"
#include "charlie_gameplay.hpp"
#include "input_handler.h"
#include "sdl_application.hpp"
#include "Singleton.hpp"
#include "sprite_handler.hpp"

namespace charlie
{
	Player::Player()
		: renderer_(nullptr)
		, body_sprite_(nullptr)
		, body_window_rect_()
		, turret_sprite_(nullptr)
		, turret_window_rect_()
		, point()
		, turret_rotation_(0)
		, input_bits_(0)
		, id_(0)
		, speed_(100.0f)
		, tank_turn_speed_(50.0f)
		, turret_turn_speed_(1)
	{
	}

	void Player::init(SDL_Renderer* renderer, Vector2& pos, uint32 id)
	{
		renderer_ = renderer;
		transform_.position_ = pos;
		id_ = id;
	}

	void Player::update(Time deltaTime, int levelHeight, int levelWidth)
	{
		float direction = 0;
		float rotation = 0;
		input_bits_ = 0;

		if (Singleton<InputHandler>::Get()->IsKeyDown(SDL_SCANCODE_W)) {
			input_bits_ |= (1 << int32(gameplay::Action::Up));
			direction -= 1.0f;
		}
		if (Singleton<InputHandler>::Get()->IsKeyDown(SDL_SCANCODE_S)) {
			input_bits_ |= (1 << int32(gameplay::Action::Down));
			direction += 1.0f;
		}
		if (Singleton<InputHandler>::Get()->IsKeyDown(SDL_SCANCODE_A)) {
			input_bits_ |= (1 << int32(gameplay::Action::Left));
			rotation -= 1.0f;
		}
		if (Singleton<InputHandler>::Get()->IsKeyDown(SDL_SCANCODE_D)) {
			input_bits_ |= (1 << int32(gameplay::Action::Right));
			rotation += 1.0f;
		}

		if (abs(rotation) > 0.0f)
		{
			const float rot = transform_.rotation_ + rotation * tank_turn_speed_ * deltaTime.as_seconds();
			transform_.set_rotation(rot);
		}

		if (abs(direction) > 0.0f) {
			transform_.position_ += transform_.forward() * direction * speed_ * deltaTime.as_seconds();
		}

		if ((transform_.position_.x_ < 0) || transform_.position_.x_ + (float)body_sprite_->get_area().w > levelHeight)
		{
			transform_.position_ -= transform_.forward() * direction * speed_ * deltaTime.as_seconds();
		}

		if ((transform_.position_.y_ < 0) || transform_.position_.y_ + (float)body_sprite_->get_area().h > levelWidth)
		{
			transform_.position_ -= transform_.forward() * direction * speed_ * deltaTime.as_seconds();
		}

		const auto mouse_x = Singleton<InputHandler>::Get()->GetMousePositionX();
		const auto mouse_y = Singleton<InputHandler>::Get()->GetMousePositionY();

		const auto delta_y = body_window_rect_.y + point.y - mouse_y;
		const auto delta_x = body_window_rect_.x + point.x - mouse_x;
		turret_rotation_ = static_cast<float>(-90 + atan2(delta_y, delta_x) * (180 / M_PI));
		turret_rotation_ >= 0 ? turret_rotation_ : 360 + turret_rotation_;
	}

	void Player::render(SDL_Rect cam)
	{
		body_window_rect_.x = static_cast<int>(transform_.position_.x_) - cam.x;
		body_window_rect_.y = static_cast<int>(transform_.position_.y_) - cam.y;
		turret_window_rect_.x = static_cast<int>(transform_.position_.x_) - cam.x;
		turret_window_rect_.y = static_cast<int>(transform_.position_.y_) - cam.y;

		point.x = static_cast<int>(transform_.origin_.x_);
		point.y = static_cast<int>(transform_.origin_.y_);

		SDL_RenderCopyEx(renderer_, body_sprite_->get_texture(), nullptr, &body_window_rect_, transform_.rotation_, &point, SDL_FLIP_NONE);
		SDL_RenderCopyEx(renderer_, turret_sprite_->get_texture(), nullptr, &turret_window_rect_, turret_rotation_, &point, SDL_FLIP_NONE);

		//SDL_SetRenderDrawColor(renderer_, 255, 0, 0, 255);
		//SDL_RenderDrawRect(renderer_, &body_window_rect_);
		//SDL_RenderDrawRect(renderer_, &turret_window_rect_);

		//int x = Singleton<InputHandler>::Get()->GetMousePositionX();
		//int y = Singleton<InputHandler>::Get()->GetMousePositionY();

		//SDL_SetRenderDrawColor(renderer_, 255, 0, 0, 255);
		//SDL_RenderDrawLine(renderer_, (int)body_window_rect_.x + point.x, (int)body_window_rect_.y + point.y, x, y);
	}

	void Player::load_body_sprite(const char* body, int srcX, int srcY, int srcW, int srcH)
	{
		body_sprite_ = Singleton<SpriteHandler>::Get()->create_sprite(body, srcX, srcY, srcW, srcH);
		body_window_rect_ = { srcX, srcY, body_sprite_->get_area().w, body_sprite_->get_area().h };
		transform_.set_origin(Vector2(body_window_rect_.w / 2, body_window_rect_.h / 2));
	}

	void Player::load_turret_sprite(const char* turret, int srcX, int srcY, int srcW, int srcH)
	{
		turret_sprite_ = Singleton<SpriteHandler>::Get()->create_sprite(turret, srcX, srcY, srcW, srcH);
		turret_window_rect_ = { srcX, srcY, turret_sprite_->get_area().w, turret_sprite_->get_area().h };
	}

	void Player::destroy()
	{
		renderer_ = nullptr;
		body_sprite_ = nullptr;
		turret_sprite_ = nullptr;
	}

	uint8 Player::get_input_bits() const
	{
		return input_bits_;
	}
}
