#include "entity.h"


#include "Singleton.hpp"
#include "sprite_handler.hpp"

namespace charlie
{
	Entity::Entity()
		: renderer_(nullptr)
		, body_sprite_(nullptr)
		, body_window_rect_()
		, turret_sprite_(nullptr)
		, turret_window_rect_()
		, point_()
		, id_(0)
		, turret_rotation_(0)
	{
	}

	Entity::~Entity()
	{
		renderer_ = nullptr;
		body_sprite_ = nullptr;
		turret_sprite_ = nullptr;
	}

	Entity& Entity::operator=(const Entity& rhs)
	{
		if (this == &rhs)
		{
			return *this;
		}

		id_ = rhs.id_;
		renderer_ = rhs.renderer_;
		transform_ = rhs.transform_;
		turret_rotation_ = rhs.turret_rotation_;

		return *this;
	}

	void Entity::init(SDL_Renderer* renderer, Vector2& pos, uint32 id)
	{
		renderer_ = renderer;
		transform_.position_ = pos;
		id_ = id;
	}

	void Entity::load_body_sprite(std::string body, int srcX, int srcY, int srcW, int srcH)
	{
		body_sprite_ = Singleton<SpriteHandler>::Get()->create_sprite(body, srcX, srcY, srcW, srcH);
		body_window_rect_ = { 0, 0, body_sprite_->get_area().w, body_sprite_->get_area().h };
		transform_.set_origin(Vector2(body_window_rect_.w / 2, body_window_rect_.h / 2));
	}

	void Entity::load_turret_sprite(std::string turret, int srcX, int srcY, int srcW, int srcH)
	{
		turret_sprite_ = Singleton<SpriteHandler>::Get()->create_sprite(turret, srcX, srcY, srcW, srcH);
		turret_window_rect_ = { 0, 0, turret_sprite_->get_area().w, turret_sprite_->get_area().h };
	}

	void Entity::render(SDL_Rect cam)
	{
		body_window_rect_.x = int(transform_.position_.x_) - cam.x;
		body_window_rect_.y = int(transform_.position_.y_) - cam.y;
		turret_window_rect_.x = body_window_rect_.x;
		turret_window_rect_.y = body_window_rect_.y;

		point_.x = (int)transform_.origin_.x_;
		point_.y = (int)transform_.origin_.y_;

		SDL_RenderCopyEx(renderer_, body_sprite_->get_texture(), nullptr, &body_window_rect_, transform_.rotation_, &point_, SDL_FLIP_NONE);
		SDL_RenderCopyEx(renderer_, turret_sprite_->get_texture(), nullptr, &turret_window_rect_, turret_rotation_, &point_, SDL_FLIP_NONE);
	}
}
