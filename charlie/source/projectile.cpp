#include "projectile.h"


#include "config.h"
#include "Singleton.hpp"
#include "sprite_handler.hpp"

namespace charlie
{
	Projectile::Projectile()
		: renderer_(nullptr)
		, window_rect_()
		, sprite_(nullptr)
		, point()
		, id_(0)
		, owner_(0)
		, speed_(config::PROJECTILE_SPEED)
		, lifetime_(config::PROJECTILE_LIFETIME)
	{
		collider_.SetSize(config::PROJECTILE_WIDTH, config::PROJECTILE_HEIGHT);
	}

	Projectile::Projectile(const Vector2 pos, const Vector2 dir, uint32 id, const uint32 owner)
		: renderer_(nullptr)
		, window_rect_()
		, sprite_(nullptr)
		, point()
		, id_(id)
		, owner_(owner)
		, speed_(config::PROJECTILE_SPEED)
		, lifetime_(config::PROJECTILE_LIFETIME)
	{
		transform_.position_ = pos;
		direction_ = dir;
		transform_.set_rotation(dir);
		collider_ = RectangleCollider((int)pos.x_, (int)pos.y_, config::PROJECTILE_WIDTH, config::PROJECTILE_HEIGHT);
	}

	Projectile::Projectile(const Vector2 pos, const float rot, uint32 id, const uint32 owner)
		: renderer_(nullptr)
		, window_rect_()
		, sprite_(nullptr)
		, point()
		, id_(id)
		, owner_(owner)
		, speed_(config::PROJECTILE_SPEED)
		, lifetime_(config::PROJECTILE_LIFETIME)
	{
		transform_.position_ = pos;
		transform_.rotation_ = rot;
		collider_ = RectangleCollider((int)pos.x_, (int)pos.y_, config::PROJECTILE_WIDTH, config::PROJECTILE_HEIGHT);
	}

	void Projectile::update(Time deltaTime)
	{
		transform_.position_ += transform_.forward() * speed_ * deltaTime.as_seconds();
		collider_.SetPosition((int)transform_.position_.x_, (int)transform_.position_.y_);
		time_alive_ += deltaTime;
	}

	void Projectile::render(SDL_Rect cam)
	{
		if (renderer_ == nullptr)
		{
			printf("No renderer reference");
		}

		window_rect_.x = static_cast<int>(transform_.position_.x_) - cam.x;
		window_rect_.y = static_cast<int>(transform_.position_.y_) - cam.y;

		point.x = static_cast<int>(transform_.origin_.x_);
		point.y = static_cast<int>(transform_.origin_.y_);

		SDL_RenderCopyEx(renderer_, sprite_->get_texture(), nullptr, &window_rect_, (double)transform_.rotation_, &point, SDL_FLIP_NONE);

		SDL_Rect collider_rect_ = {
			collider_.GetBounds().x - cam.x,
			collider_.GetBounds().y - cam.y,
			collider_.GetBounds().w,
			collider_.GetBounds().h
		};

		//SDL_SetRenderDrawColor(renderer_, 0, 0, 255, 255);
		//SDL_RenderDrawRect(renderer_, &collider_rect_);
	}

	void Projectile::load_sprite(std::string body, int srcX, int srcY, int srcW, int srcH)
	{
		sprite_ = Singleton<SpriteHandler>::Get()->create_sprite(body, srcX, srcY, srcW, srcH);
		window_rect_ = { srcX, srcY, sprite_->get_area().w, sprite_->get_area().h };
		transform_.set_origin(Vector2(window_rect_.w / 2, window_rect_.h / 2));
	}

	bool Projectile::is_dead() const
	{
		return time_alive_ > lifetime_;
	}

	void Projectile::destroy()
	{
		sprite_ = nullptr;
		renderer_ = nullptr;
	}

	void Projectile::on_collision()
	{
	}
}
