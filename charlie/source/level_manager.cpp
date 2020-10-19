#include "level_manager.h"

#include "camera.h"
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
			return Singleton<SpriteHandler>::Get()->create_sprite("../assets/grass.png", x, y, PIXEL_PER_UNIT, PIXEL_PER_UNIT);
			break;
		case 2:
			return Singleton<SpriteHandler>::Get()->create_sprite("../assets/block.png", x, y, PIXEL_PER_UNIT, PIXEL_PER_UNIT);
			break;
		default:
			SDL_Log("No asset found with id: %i", type);
			break;
		}
		return nullptr;
	}

	bool LevelManager::load_assets(Leveldata& data)
	{
		height_ = data.sizeY_ * PIXEL_PER_UNIT;
		width_ = data.sizeX_ * PIXEL_PER_UNIT;
		levelObjects_.resize(data.sizeY_ * data.sizeX_);

		for (int y = 0; y < data.sizeY_; y++) {
			for (int x = 0; x < data.sizeX_; x++) {
				int type = data.get_tile_type(x, y);
				LevelObject levelObject;
				levelObject.pos_ = Vector2(x * 50, y * 50);
				levelObject.sprite_ = load_asset_with_id(type, x, y);
				levelObjects_[y * data.sizeX_ + x] = levelObject;
			}
		}

		return true;
	}

	void LevelManager::render(SDL_Rect camera, SDL_Renderer* renderer)
	{
		for (auto& levelObject : levelObjects_)
		{
			SDL_Rect rect = { (int)levelObject.pos_.x_ - camera.x, (int)levelObject.pos_.y_ - camera.y, levelObject.sprite_->get_area().w, levelObject.sprite_->get_area().h };
			SDL_RenderCopy(renderer, levelObject.sprite_->get_texture(), nullptr, &rect);
		}
	}
}
