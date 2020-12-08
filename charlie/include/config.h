#pragma once

#ifndef CONFIG_HPP_INCLUDED
#define CONFIG_HPP_INCLUDED
#include <string>

namespace charlie
{
	namespace config
	{
		static constexpr int  PORT = 54345;
		static constexpr uint8  IP_A = 82;
		static constexpr uint8  IP_B = 117;
		static constexpr uint8  IP_C = 111;
		static constexpr uint8  IP_D = 100;
		static constexpr int  SCREEN_WIDTH = 640;
		static constexpr int  SCREEN_HEIGHT = 480;
		static constexpr int  PLAYER_WIDTH = 100;
		static constexpr int  PLAYER_HEIGHT = 100;
		static constexpr float PLAYER_SPEED = 70.0f;
		static constexpr float PLAYER_REVERSE_SPEED = 35.0f;
		static constexpr float PLAYER_TURN_SPEED = 30.0f;
		static constexpr int  PROJECTILE_WIDTH = 15;
		static constexpr int  PROJECTILE_HEIGHT = 15;
		static constexpr double  FIRE_DELAY = 2.0;
		static constexpr float PROJECTILE_SPEED = 800.0f;
		static const Time PROJECTILE_LIFETIME = Time(3.0);
		static constexpr int LEVEL_OBJECT_WIDTH = 50;
		static constexpr int LEVEL_OBJECT_HEIGHT = 50;
		static constexpr uint8 map = 1;
		static const std::string TANK_BODY_SPRITE("../assets/tank_body.png");
		static const std::string TANK_TURRET_SPRITE("../assets/tank_turret.png");
		static const std::string TANK_SHELL("../assets/Light_Shell.png");
		static const std::string FONT_PATH("../assets/font/font.ttf");
		static const std::string MENU_SCREEN("../assets/menu_master_server.png");
		static const std::string DISCONNECTED("../assets/disconnected.png");
		static const std::string DEFEAT_SCREEN("../assets/defeat.png");
		static const std::string GRASS_TEXTURE("../assets/grass.png");
		static const std::string BLOCK_TEXTURE("../assets/block.png");
		static const std::string LEVEL_PATH_PREFIX("../assets/");
		static const std::string MASTER_SERVER_FILE("../assets/masterserver.txt");
	};
}
#endif
