#include "level_manager.h"
#include "leveldata.h"
#include "Singleton.hpp"
#include "sprite_handler.hpp"

namespace charlie
{
	LevelManager::LevelManager() : height_(0), width_(0)
	{
	}

	LevelManager::LevelManager(Leveldata& data) : height_(0), width_(0)
	{
		data_ = data;
		load_assets();
	}

	void LevelManager::load_asset_with_id(int type, int x, int y)
	{
		sprite_ = Singleton<SpriteHandler>::Get()->create_sprite("../assets/block.png", x, y, 100, 100);
	}

	bool LevelManager::load_assets()
	{
		height_ = data_.sizeY_;
		width_ = data_.sizeX_;

		for (int y = 0; y < height_; y++) {
			for (int x = 0; x < width_; x++) {
				int type = data_.get_tile_type(x, y);
				load_asset_with_id(type, x, y);
			}
		}

		return true;
	}

	void LevelManager::render(SDL_Renderer* renderer)
	{
		for (auto& tile : data_.tiles_)
		{
			SDL_Rect rect = { 50, 50, sprite_->get_area().w, sprite_->get_area().h };
			SDL_RenderCopy(renderer, sprite_->get_texture(), nullptr, &rect);
		}
	}
}
