#include "level_manager.h"


#include "charlie_messages.hpp"
#include "config.h"
#include "leveldata.h"
#include "Singleton.hpp"
#include "sprite_handler.hpp"

namespace charlie
{
	LevelObject::LevelObject() : sprite_(nullptr), blocked_(false)
	{
	}

	LevelManager::LevelManager() : height_(0), width_(0), spawn_index_(0), state_(LevelManagerState::INITIALIZED),
		level_id_(0)
	{
	}

	LevelManager::~LevelManager()
	{
		levelObjects_.clear();
	}

	bool LevelManager::waiting_for_data() const
	{
		return state_ == LevelManagerState::WAITING_FOR_DATA;
	}

	void LevelManager::create_tile(uint8 level_id, uint8 x, charlie::uint8 y)
	{
		const Tile tile{ level_id, x, y };
		data_.tiles_.push_back(tile);
		data_.tiles_loaded_ += 1;
		create_level_object(tile.tile_id_, tile.x_, tile.y_);
	}

	uint32 LevelManager::get_chunk(const uint32 send_to)
	{
		for (auto& send : send_progress_)
		{
			if (send.first == send_to)
			{
				return send.second;
			}
		}
		return 0;
	}

	bool LevelManager::is_done_sending() const
	{
		printf("Tiles loaded %i \n", data_.tiles_loaded_);
		return data_.tiles_loaded_ > (data_.sizeY_ * data_.sizeX_) - 1;
	}

	SDLSprite* LevelManager::load_asset_with_id(int type, int x, int y)
	{
		switch (type)
		{
		case 1:
			return Singleton<SpriteHandler>::Get()->create_sprite(config::GRASS_TEXTURE, x, y, config::LEVEL_OBJECT_WIDTH, config::LEVEL_OBJECT_HEIGHT);
			break;
		case 2:
			return Singleton<SpriteHandler>::Get()->create_sprite(config::BLOCK_TEXTURE, x, y, config::LEVEL_OBJECT_WIDTH, config::LEVEL_OBJECT_HEIGHT);
			break;
		case 3:
			return Singleton<SpriteHandler>::Get()->create_sprite(config::GRASS_TEXTURE, x, y, config::LEVEL_OBJECT_WIDTH, config::LEVEL_OBJECT_HEIGHT);
			break;
		default:
			SDL_Log("No asset found with id: %i", type);
			break;
		}
		return nullptr;
	}

	void LevelManager::create_level_object(const int type, int x, int y)
	{
		LevelObject levelObject;
		levelObject.pos_ = Vector2(x * config::LEVEL_OBJECT_WIDTH, y * config::LEVEL_OBJECT_HEIGHT);
		levelObject.sprite_ = load_asset_with_id(type, x, y);
		levelObject.blocked_ = false;

		if (type == COLLIDER)
		{
			levelObject.collider_ = RectangleCollider((int)levelObject.pos_.x_, (int)levelObject.pos_.y_, config::LEVEL_OBJECT_WIDTH, config::LEVEL_OBJECT_HEIGHT);
			levelObject.blocked_ = true;
			colliders_.push_back(levelObject);
		}

		if (type == SPAWN_POINT)
		{
			spawn_points_.push_back(levelObject.pos_);
		}

		levelObjects_[y * data_.sizeX_ + x] = levelObject;
	}

	bool LevelManager::load_assets(Leveldata& data)
	{
		data_ = data;
		height_ = data_.sizeY_ * config::LEVEL_OBJECT_HEIGHT;
		width_ = data_.sizeX_ * config::LEVEL_OBJECT_WIDTH;
		levelObjects_.resize(data_.sizeY_ * data_.sizeX_);

		for (int y = 0; y < data_.sizeY_; y++) {
			for (int x = 0; x < data_.sizeX_; x++) {
				const int type = data_.get_tile_type(x, y);

				create_level_object(type, x, y);
			}
		}
		state_ = LevelManagerState::LEVEL_LOADED;
		return true;
	}

	void LevelManager::render(SDL_Rect camera, SDL_Renderer* renderer)
	{
		for (auto& levelObject : levelObjects_)
		{
			if (levelObject.sprite_ == nullptr)
			{
				continue;
			}
			SDL_Rect rect = { (int)levelObject.pos_.x_ - camera.x, (int)levelObject.pos_.y_ - camera.y, levelObject.sprite_->get_area().w, levelObject.sprite_->get_area().h };
			SDL_RenderCopy(renderer, levelObject.sprite_->get_texture(), nullptr, &rect);
		}
	}

	Vector2 LevelManager::get_spawn_pos()
	{
		if (spawn_index_ > (int)spawn_points_.size())
		{
			spawn_index_ = 0;
		}
		const Vector2 pos = spawn_points_[spawn_index_];
		spawn_index_ += 1;
		return pos;
	}

	void LevelManager::request_map_data(uint8 level_id, uint8 size_x, uint8 size_y)
	{
		if (state_ != LevelManagerState::LEVEL_LOADED)
		{
			data_ = Leveldata();
			data_.sizeX_ = size_x;
			data_.sizeY_ = size_y;
			level_id_ = level_id;
			levelObjects_.resize(data_.sizeY_ * data_.sizeX_);
			state_ = LevelManagerState::WAITING_FOR_DATA;
		}
	}

	Tile LevelManager::get_level_data(uint32 player_id)
	{
		Tile send{};

		// Continue sending
		for (auto& progress : send_progress_)
		{
			if (progress.first == player_id)
			{
				if (progress.second >= (uint32)(data_.sizeY_ * data_.sizeX_))
				{
					printf("Trying to get data outsize array");
					return send;
				}
				send = data_.get_tile(progress.second);
				progress.second += 1;
				return send;
			}
		}
		{
			// Send first time
			std::pair<uint32, uint32> progress(player_id, 0);
			send = data_.get_tile(progress.second);
			progress.second += 1;
			send_progress_.push_back(progress);
		}

		return send;
	}
}
