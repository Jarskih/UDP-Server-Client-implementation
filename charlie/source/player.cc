#include "player.hpp"



#include "charlie_gameplay.hpp"
#include "input_handler.h"
#include "SDLKeyboard.h"
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

	void Player::update(Time deltaTime)
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

		if(abs(rotation) > 0.0f)
		{
			const float rot = transform_.rotation_ + rotation * tank_turn_speed_ * deltaTime.as_seconds();
			transform_.set_rotation(rot);
		}
		
		if (abs(direction) > 0.0f) {
			transform_.position_ += transform_.forward() * direction * speed_ * deltaTime.as_seconds();
		}
	}

	void Player::render()
	{
		body_window_rect_.x = int(transform_.position_.x_ + transform_.origin_.x_);
		body_window_rect_.y = int(transform_.position_.y_ + transform_.origin_.y_);
		turret_window_rect_.x = body_window_rect_.x;
		turret_window_rect_.y = body_window_rect_.y;

		// https://stackoverflow.com/questions/21174767/how-do-you-rotate-a-sprite-based-on-mouse-position
		// mouse rotate
		// angle_deg = (atan2(delta_y, delta_x)*180.0000)/3.1416
		// delta_y = origin_y - mouse_y
		// delta_x = origin_x - mouse_x
		
		SDL_Point* point = new SDL_Point();
		point->x = (int)transform_.origin_.x_;
		point->y = (int)transform_.origin_.y_;
		int flip = SDL_FLIP_HORIZONTAL | SDL_FLIP_VERTICAL;

		SDL_RenderCopyEx(renderer_, body_sprite_->get_texture(), nullptr, &body_window_rect_, transform_.rotation_, point, (SDL_RendererFlip)flip);
		SDL_RenderCopyEx(renderer_, turret_sprite_->get_texture(), nullptr, &turret_window_rect_, transform_.rotation_, point, (SDL_RendererFlip)flip);
		SDL_SetRenderDrawColor(renderer_, 255  ,0,  0, 255);
		SDL_RenderDrawRect(renderer_, &body_window_rect_);
		SDL_RenderDrawRect(renderer_, &turret_window_rect_);
	}

	void Player::load_body_sprite(const char* body, int srcX, int srcY, int srcW, int srcH)
	{
		body_sprite_ = Singleton<SpriteHandler>::Get()->create_sprite(body, srcX, srcY, srcW, srcH);
		body_window_rect_ = { 0, 0, body_sprite_->get_area().w, body_sprite_->get_area().h };
		transform_.set_origin(Vector2(body_window_rect_.w/2, body_window_rect_.h/2));
	}

	void Player::load_turret_sprite(const char* turret, int srcX, int srcY, int srcW, int srcH)
	{
		turret_sprite_ = Singleton<SpriteHandler>::Get()->create_sprite(turret, srcX, srcY, srcW, srcH);
		turret_window_rect_ = { 0, 0, turret_sprite_->get_area().w, turret_sprite_->get_area().h };
	}

	uint8 Player::get_input_bits() const
	{
		return input_bits_;
	}
}
