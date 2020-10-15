#pragma once
#include "charlie.hpp"

namespace charlie
{
	struct Leveldata
	{
		Leveldata();
		void create_level(const char* path);
		int get_tile_type(int x, int y);
		DynamicArray<int> tiles_; // Array of tiletypes
		int sizeX_;
		int sizeY_;
	};
}
