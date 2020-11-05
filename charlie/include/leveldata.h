#pragma once
#include <string>

#include "charlie.hpp"

namespace charlie
{
	struct Tile
	{
		uint8 tile_id_;
		uint8 x_;
		uint8 y_;
	};

	struct Leveldata
	{
		Leveldata();
		void get_size(const char* path);
		bool create_level(uint8 name);
		int get_tile_type(int x, int y);
		Tile get_tile(uint32 tile_number);
		DynamicArray<Tile> tiles_; // Array of tiletypes
		int sizeX_;
		int sizeY_;
		int tiles_loaded_;
	};
}
