#include "leveldata.h"
#include <fstream>

#include "level_manager.h"

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

		sizeX_ = 18;
		sizeY_ = 22;
		tiles_.resize(size_t(sizeY_ * sizeX_));

		for (int y = 0; y < sizeY_; y++) {
			for (int x = 0; x < sizeX_; x++) {

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
				if (id < 0)
				{
					printf("Wrong id for x: %i y: %i", x, y);
					return;
				}
				tiles_[y * sizeX_ + x] = id;

			}
		}
	}

	int Leveldata::get_tile_type(int x, int y)
	{
		int type = tiles_[y * sizeX_ + x];

		if (type != LevelObjectType::COLLIDER && type != LevelObjectType::FREE)
		{
			printf("Wrong tile type: %i \n", type);
		}
		return type;
	}
}
