#pragma once
#include "sdl_collider.h"
#include "leveldata.h"
#include "sprite_handler.hpp"

namespace charlie
{
	struct Leveldata;

	enum class LevelManagerState
	{
		INITIALIZED,
		WAITING_FOR_DATA,
		LEVEL_LOADED,
	};

	enum LevelObjectType
	{
		FREE = 1,
		COLLIDER = 2,
		SPAWN_POINT = 3,
		COUNT
	};

	struct LevelObject
	{
		LevelObject();
		Vector2 pos_;
		SDLSprite* sprite_;
		RectangleCollider collider_;
		bool blocked_;
	};

	struct LevelManager
	{
		LevelManager();
		~LevelManager();
		bool waiting_for_data() const;
		void create_tile(uint8 level_tile, uint8 x, uint8 y);
		uint32 get_chunk(const uint32 send_to);
		bool is_done_sending() const;
		static SDLSprite* load_asset_with_id(int type, int x, int y);
		void create_level_object(const int type, int i, int y);
		bool load_assets(Leveldata& data);
		void render(SDL_Rect camera, SDL_Renderer* renderer);
		Vector2 get_spawn_pos();
		void request_map_data(uint8 level_id, uint8 size_x, uint8 size_y);
		Tile get_level_data(uint32 player_id);

		Leveldata data_;
		int height_;
		int width_;
		DynamicArray<LevelObject> levelObjects_;
		DynamicArray<LevelObject> colliders_;
		DynamicArray<Vector2> spawn_points_;
		int spawn_index_;
		LevelManagerState state_;
		uint8 level_id_;
		DynamicArray<std::pair<uint32, uint32>> send_progress_;
	};
}
