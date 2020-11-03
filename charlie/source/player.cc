#include "player.hpp"

#include "charlie_gameplay.hpp"
#include "config.h"
#include "input_handler.h"
#include "level_manager.h"
#include "projectile.h"
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
		, speed_(config::PLAYER_SPEED)
		, tank_turn_speed_(config::PLAYER_TURN_SPEED)
		, turret_turn_speed_(1)
		, fire_(false)
		, collider_offset_x_(0)
		, collider_offset_y_(0)
		, is_dead_(false)
	{
		fire_delay_ = Time(config::FIRE_DELAY);
		fire_acc_ = fire_delay_;
	}

	void Player::init(SDL_Renderer* renderer, Vector2& pos, uint32 id)
	{
		is_dead_ = false;
		renderer_ = renderer;
		transform_.position_ = pos;
		old_pos_ = pos;
		id_ = id;
		collider_offset_x_ = config::PLAYER_WIDTH / 3;
		collider_offset_y_ = config::PLAYER_HEIGHT / 3;
		collider_ = RectangleCollider((int)pos.x_, (int)pos.y_, collider_offset_x_, collider_offset_y_);
	}

	void Player::update(Time deltaTime, int levelHeight, int levelWidth)
	{
		float direction = 0;
		float rotation = 0;
		input_bits_ = 0;
		fire_ = false;

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

		old_pos_ = transform_.position_;
		if (abs(direction) > 0.0f) {
			transform_.position_ += transform_.forward() * direction * speed_ * deltaTime.as_seconds();
		}

		if ((transform_.position_.x_ < 0) || (int)(transform_.position_.x_ + (float)body_sprite_->get_area().w) > (float)levelHeight)
		{
			transform_.position_ -= transform_.forward() * direction * speed_ * deltaTime.as_seconds();
		}

		if ((transform_.position_.y_ < 0) || (int)(transform_.position_.y_ + (float)body_sprite_->get_area().h) > levelWidth)
		{
			transform_.position_ -= transform_.forward() * direction * speed_ * deltaTime.as_seconds();
		}

		const auto mouse_x = Singleton<InputHandler>::Get()->GetMousePositionX();
		const auto mouse_y = Singleton<InputHandler>::Get()->GetMousePositionY();

		const auto delta_y = body_window_rect_.y + point.y - mouse_y;
		const auto delta_x = body_window_rect_.x + point.x - mouse_x;
		turret_rotation_ = static_cast<float>(-90 + atan2(delta_y, delta_x) * (180 / M_PI));
		turret_rotation_ >= 0 ? turret_rotation_ : 360 + turret_rotation_;

		fire_acc_ += deltaTime;
		if (Singleton<InputHandler>::Get()->IsKeyDown(SDL_SCANCODE_SPACE) || Singleton<InputHandler>::Get()->IsMouseButtonDown(1))
		{
			fire_ = true;
		}

		collider_.SetPosition(get_collider_pos());
	}

	void Player::render(SDL_Rect cam)
	{
		body_window_rect_.x = static_cast<int>(transform_.position_.x_) - cam.x;
		body_window_rect_.y = static_cast<int>(transform_.position_.y_) - cam.y;
		turret_window_rect_.x = static_cast<int>(transform_.position_.x_) - cam.x;
		turret_window_rect_.y = static_cast<int>(transform_.position_.y_) - cam.y;

		point.x = static_cast<int>(transform_.origin_.x_);
		point.y = static_cast<int>(transform_.origin_.y_);

		SDL_RenderCopyEx(renderer_, body_sprite_->get_texture(), nullptr, &body_window_rect_, (double)transform_.rotation_, &point, SDL_FLIP_NONE);
		SDL_RenderCopyEx(renderer_, turret_sprite_->get_texture(), nullptr, &turret_window_rect_, (double)turret_rotation_, &point, SDL_FLIP_NONE);

		SDL_Rect collider_rect_ = {
			collider_.GetBounds().x - cam.x,
			collider_.GetBounds().y - cam.y,
			collider_.GetBounds().w,
			collider_.GetBounds().h
		};

		SDL_SetRenderDrawColor(renderer_, 0, 255, 0, 255);
		SDL_RenderDrawRect(renderer_, &collider_rect_);
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

	Vector2 Player::get_shoot_pos() const
	{
		Vector2 pos;
		pos.x_ = transform_.position_.x_ + transform_.origin_.x_ - config::PROJECTILE_WIDTH / 2.0f;
		pos.y_ = transform_.position_.y_ + transform_.origin_.y_ - config::PROJECTILE_WIDTH / 2.0f;
		return pos;
	}

	void Player::destroy()
	{
		renderer_ = nullptr;
		body_sprite_ = nullptr;
		renderer_ = nullptr;
		turret_sprite_ = nullptr;
	}

	uint8 Player::get_input_bits() const
	{
		return input_bits_;
	}

	bool Player::can_shoot() const
	{
		return fire_acc_ > fire_delay_;
	}

	void Player::fire()
	{
		fire_acc_ = Time(0.0);
		// TODO effects sounds
	}

	void Player::on_collision(const LevelObject lvl_object)
	{
		reset_old_pos();
	}

	void Player::on_collision(const Player& other)
	{
		reset_old_pos();
	}

	void Player::on_collision(const Projectile& other)
	{
		is_dead_ = true;
	}

	void Player::reset_old_pos()
	{
		transform_.position_ = old_pos_;
		collider_.SetPosition(get_collider_pos());
		body_window_rect_.x = (int)transform_.position_.x_;
		body_window_rect_.y = (int)transform_.position_.y_;
	}

	Vector2 Player::get_collider_pos() const
	{
		return { transform_.position_.x_ + (float)collider_offset_x_, transform_.position_.y_ + (float)collider_offset_y_ };
	}

	bool Player::is_dead() const
	{
		return is_dead_;
	}
}
