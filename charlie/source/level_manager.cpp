#include "level_manager.h"

#include "Camera.h"
#include "leveldata.h"
#include "Singleton.hpp"
#include "sprite_handler.hpp"

namespace charlie
{
	LevelObject::LevelObject() : sprite_(nullptr)
	{
	}

	LevelManager::LevelManager() : height_(0), width_(0)
	{
	}

	LevelManager::~LevelManager()
	{
		levelObjects_.clear();
	}

	SDLSprite* LevelManager::load_asset_with_id(int type, int x, int y)
	{
		switch (type)
		{
		case 1:
			return Singleton<SpriteHandler>::Get()->create_sprite("../assets/block.png", x, y, 25, 25);
			break;
		default:
			SDL_Log("No asset found with id: %i", type);
			break;
		}
		return nullptr;
	}

	bool LevelManager::load_assets(Leveldata& data)
	{
		bg_ = Singleton<SpriteHandler>::Get()->create_sprite("../assets/bg.png", 0, 0, 1280, 720);

		height_ = data.sizeY_;
		width_ = data.sizeX_;
		levelObjects_.resize(height_ * width_);

		for (int y = 0; y < height_; y++) {
			for (int x = 0; x < width_; x++) {
				int type = data.get_tile_type(x, y);
				LevelObject levelObject;
				levelObject.pos_ = Vector2(x * 25, y * 25);
				levelObject.sprite_ = load_asset_with_id(type, x, y);
				levelObjects_[y * width_ + x] = levelObject;
			}
		}

		return true;
	}

	void LevelManager::render(Camera camera, SDL_Renderer* renderer)
	{
		SDL_Rect r = { 0 - camera.lookAt_.x,0 - camera.lookAt_.y,1280, 720 };
		SDL_RenderCopy(renderer, bg_->get_texture(), nullptr, &camera.lookAt_);

		for (auto& levelObject : levelObjects_)
		{
			SDL_Rect rect = { (int)levelObject.pos_.x_ - camera.lookAt_.x, (int)levelObject.pos_.y_ - camera.lookAt_.y, levelObject.sprite_->get_area().w, levelObject.sprite_->get_area().h };
			SDL_RenderCopy(renderer, levelObject.sprite_->get_texture(), nullptr, &rect);
		}
	}
}
