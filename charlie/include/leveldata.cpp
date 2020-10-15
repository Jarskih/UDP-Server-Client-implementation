#include "leveldata.h"
#include <fstream>

namespace charlie
{
	Leveldata::Leveldata() : sizeX_(0), sizeY_(0)
	{
	}

	void Leveldata::create_level(const char* path)
	{
		std::ifstream map(path);

		//If the map couldn't be loaded
		if (!map.is_open())
		{
			printf("Unable to load map file!\n");
		}

		sizeX_ = 3;
		sizeY_ = 3;
		tiles_.resize(sizeY_ * sizeX_);

		for (int y = 0; y < sizeX_; y++) {
			for (int x = 0; x < sizeY_; x++) {
				int posX = x * 20;
				int posY = y * 20;
				bool hasCollider = true;

				//Determines what kind of block will be made
				int id = -1;

				//Read block from map file
				map >> id;

				//If the was a problem in reading the map
				if (map.fail())
				{
					//Stop loading map
					printf("Error loading map: Unexpected end of file!\n");
					break;
				}
				else
				{
					tiles_[y * sizeX_ + x] = id;
				}
			}
		}
	}

	int Leveldata::get_tile_type(int x, int y)
	{
		return tiles_[y * sizeX_ + x];
	}
}
