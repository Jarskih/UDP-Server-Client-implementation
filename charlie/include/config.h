#pragma once

#ifndef CONFIG_HPP_INCLUDED
#define CONFIG_HPP_INCLUDED
#include <xstring>

namespace charlie
{
	namespace config
	{
		static const int  PORT = 54345;
		static const uint8  IP_A = 82;
		static const uint8  IP_B = 117;
		static const uint8  IP_C = 111;
		static const uint8  IP_D = 100;
		static const int  SCREEN_WIDTH = 640;
		static const int  SCREEN_HEIGHT = 480;
		static const int  PLAYER_WIDTH = 100;
		static const int  PLAYER_HEIGHT = 100;
		static const float PLAYER_SPEED = 70.0f;
		static const float PLAYER_REVERSE_SPEED = 35.0f;
		static const float PLAYER_TURN_SPEED = 30.0f;
		static const int  PROJECTILE_WIDTH = 15;
		static const int  PROJECTILE_HEIGHT = 15;
		static const double  FIRE_DELAY = 2.0;
		static const float PROJECTILE_SPEED = 800.0f;
		static const Time PROJECTILE_LIFETIME = Time(3.0);
		static const char* TANK_BODY_SPRITE = "../assets/tank_body.png";
		static const char* TANK_TURRET_SPRITE = "../assets/tank_turret.png";
		static const char* TANK_SHELL = "../assets/Light_Shell.png";
		static const char* FONT_PATH = "../assets/font/font.ttf";
		static const int LEVEL_OBJECT_WIDTH = 50;
		static const int LEVEL_OBJECT_HEIGHT = 50;
		static const char* MENU_SCREEN = "../assets/menu_screen.png";
		static const char* DISCONNECTED = "../assets/disconnected.png";
		static const char* DEFEAT_SCREEN = "../assets/defeat.png";
		static const char* GRASS_TEXTURE = "../assets/grass.png";
		static const char* BLOCK_TEXTURE = "../assets/block.png";
		static std::string LEVEL_PATH_PREFIX = "../assets/";
		static const uint8 map = 1;
		static const char* MASTER_SERVER_FILE = "../assets/masterserver.txt";
	};
}
#endif
