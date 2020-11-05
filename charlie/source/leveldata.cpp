#include "leveldata.h"
#include <fstream>
#include <sstream>
#include <string>
#include "level_manager.h"
#include <config.h>

namespace charlie
{
	Leveldata::Leveldata() : sizeX_(0), sizeY_(0), tiles_loaded_(0)
	{
	}

	void Leveldata::get_size(const char* path)
	{
		std::ifstream map(path);
		//If the map couldn't be loaded
		if (!map.is_open())
		{
			printf("Unable to load map file!\n");
		}

		std::string line;
		sizeY_ = 0;

		std::string tile;
		sizeX_ = 0;

		while (std::getline(map, line)) {
			++sizeY_;

			if (sizeY_ > 1)
			{
				continue;
			}

			std::stringstream ss(line);
			while (ss >> tile) {
				++sizeX_;
			}
		}

		map.close();
	}

	bool Leveldata::create_level(uint8 level_id)
	{
		const std::string id_string = "map" + std::to_string(level_id) + ".txt";
		std::string prefix = config::LEVEL_PATH_PREFIX;
		const char* path = (prefix.append(id_string)).c_str();
		get_size(path);

		std::ifstream map(path);

		if (!map.is_open())
		{
			printf("Unable to load map file!\n");
			return false;
		}

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
					return false;
				}
				if (id < 0)
				{
					printf("Wrong id for x: %i y: %i", x, y);
					return false;
				}

				const Tile tile{ (uint8)id, (uint8)x, (uint8)y };
				tiles_[y * sizeX_ + x] = tile;

			}
		}
		map.close();
		return true;
	}

	int Leveldata::get_tile_type(int x, int y)
	{
		int type = tiles_[y * sizeX_ + x].tile_id_;

		if (type != LevelObjectType::COLLIDER && type != LevelObjectType::FREE && type != LevelObjectType::SPAWN_POINT)
		{
			printf("Wrong tile type: %i \n", type);
		}
		return type;
	}

	Tile Leveldata::get_tile(uint32 tile_number)
	{
		if (sizeX_ > 255 || sizeY_ > 255)
		{
			assert(!"Level size too big");
		}

		if (sizeX_ <= 0 || sizeY_ <= 0)
		{
			assert(!"Level size zero");
		}

		if (tile_number > tiles_.size())
		{
			assert(!"Trying to get tile outside the map");
		}
		tiles_loaded_ += 1;
		return tiles_[tile_number];
	}
}
