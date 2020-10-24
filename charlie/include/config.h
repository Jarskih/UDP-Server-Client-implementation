#pragma once

namespace charlie
{
	namespace config
	{
		static const int  PORT = 54345;
		static const uint8  IP_A = 192;
		static const uint8  IP_B = 168;
		static const uint8  IP_C = 0;
		static const uint8  IP_D = 255;
		static const int  SCREEN_WIDTH = 640;
		static const int  SCREEN_HEIGHT = 480;
		static const int  PLAYER_WIDTH = 150;
		static const int  PLAYER_HEIGHT = 150;
		static const float PLAYER_SPEED = 100.0f;
		static const float PLAYER_TURN_SPEED = 50.0f;
		static const int  PROJECTILE_WIDTH = 25;
		static const int  PROJECTILE_HEIGHT = 25;
		static const double  FIRE_DELAY = 2.0;
		static const float PROJECTILE_SPEED = 300;
		static const char* TANK_BODY_SPRITE = "../assets/tank_body.png";
		static const char* TANK_TURRET_SPRITE = "../assets/tank_turret.png";
		static const char* TANK_SHELL = "../assets/Light_Shell.png";
		static const char* LEVEL1 = "../assets/map.txt";
		static const char* FONT_PATH = "../assets/font/font.ttf";
	};
}