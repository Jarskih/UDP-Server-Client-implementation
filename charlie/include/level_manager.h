#pragma once
#include "leveldata.h"
#include "sprite_handler.hpp"

namespace charlie
{
	struct Leveldata;

	struct LevelManager
	{
		LevelManager();
		LevelManager(Leveldata& data);
		void load_asset_with_id(int type, int x, int y);
		bool load_assets();
		void render(SDL_Renderer* renderer);

		Leveldata data_;
		int height_;
		int width_;
		SDLSprite* sprite_;
	};
}
