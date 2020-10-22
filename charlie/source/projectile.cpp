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
	, owner_(0)
	, speed_(config::PROJECTILE_SPEED)
	{
	}

	Projectile::Projectile(const Vector2 pos, const Vector2 dir, const uint32 owner)
	: renderer_(nullptr)
	, window_rect_()
	, sprite_(nullptr)
	, point()
	, owner_(owner)
	, speed_(config::PROJECTILE_SPEED)
	{
		transform_.position_ = pos;
		direction_ = dir;
		transform_.set_rotation(dir);
	}

	Projectile::Projectile(const Vector2 pos, const float rot, const uint32 owner)
	: renderer_(nullptr)
	, window_rect_()
	, sprite_(nullptr)
	, point()
	, owner_(owner)
	, speed_(config::PROJECTILE_SPEED)
	{
		transform_.position_ = pos;
		transform_.rotation_ = rot;
	}

	void Projectile::update(Time deltaTime)
	{
		transform_.position_ += transform_.forward() * speed_ * deltaTime.as_seconds();
	}

	void Projectile::render(SDL_Rect cam)
	{
		if(renderer_ == nullptr)
		{
			printf("No renderer reference");
		}
		
		window_rect_.x = static_cast<int>(transform_.position_.x_) - cam.x;
		window_rect_.y = static_cast<int>(transform_.position_.y_) - cam.y;

		point.x = static_cast<int>(transform_.origin_.x_);
		point.y = static_cast<int>(transform_.origin_.y_);

		SDL_RenderCopyEx(renderer_, sprite_->get_texture(), nullptr, &window_rect_, (double)transform_.rotation_, &point,
		                 SDL_FLIP_NONE);
	}

	void Projectile::load_sprite(const char* body, int srcX, int srcY, int srcW, int srcH)
	{
		sprite_ = Singleton<SpriteHandler>::Get()->create_sprite(body, srcX, srcY, srcW, srcH);
		window_rect_ = { srcX, srcY, sprite_->get_area().w, sprite_->get_area().h };
		transform_.set_origin(Vector2(window_rect_.w / 2, window_rect_.h / 2));
	}

	void Projectile::destroy()
	{
		sprite_ = nullptr;
		renderer_ = nullptr;
	}
}
