#pragma once
#include "camera.h"
#include "leveldata.h"
#include "sprite_handler.hpp"

namespace charlie
{
	struct Leveldata;

	struct LevelObject
	{
		LevelObject();
		Vector2 pos_;
		SDLSprite* sprite_;
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
		int PIXEL_PER_UNIT = 50;
	};
}
