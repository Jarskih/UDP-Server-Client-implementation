#pragma once
#include <SDL.h>

#include "charlie.hpp"

namespace charlie
{
	struct Camera
	{
		Camera();
		void init(float screenWidth, float screenHeight, float metersToShowX, float metersToShowY);
		void set_meters_to_show(float meters_to_show_x, float meters_to_show_y);
		void look_at(Vector2 pos);
		void worldToScreen(Vector2 worldPos, Vector2& screenPos) const;

		float screenWidth_;
		float screenHeight_;
		float screenCenterX_;
		float screenCenterY_;
		float metersToShowX_;
		float metersToShowY_;
		float BUFFER;
		int halfDistX_;
		int halfDistY_;
		float pixelsPerMeterX_;
		float pixelsPerMeterY_;
		SDL_Rect lookAt_;

		const int LEVEL_WIDTH = 1280;
		const int LEVEL_HEIGHT = 960;
	};
}
