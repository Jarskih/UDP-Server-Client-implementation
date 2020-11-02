#pragma once
#include "sdl_collider.h"
#include "leveldata.h"
#include "sprite_handler.hpp"

namespace charlie
{
	struct Leveldata;

	enum LevelObjectType
	{
		FREE = 1,
		COLLIDER = 2,
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
		SDLSprite* load_asset_with_id(int type, int x, int y);
		bool load_assets(Leveldata& data);
		void render(SDL_Rect camera, SDL_Renderer* renderer);

		int height_;
		int width_;
		DynamicArray<LevelObject> levelObjects_;
		DynamicArray<LevelObject> colliders_;
		int PIXEL_PER_UNIT = 50;
	};
}
