#include "camera.h"

namespace charlie
{
	Camera::Camera()
		: screenWidth_(0)
		, screenHeight_(0)
		, screenCenterX_(0)
		, screenCenterY_(0)
		, metersToShowX_(0)
		, metersToShowY_(0)
		, BUFFER(0)
		, halfDistX_(0)
		, halfDistY_(0)
		, pixelsPerMeterX_(0)
		, pixelsPerMeterY_(0)
		, lookAt_()
	{
	}

	void Camera::init(float screenWidth, float screenHeight, float metersToShowX, float metersToShowY) {
		screenWidth_ = screenWidth;
		screenHeight_ = screenHeight;
		screenCenterX_ = screenWidth_ / 2;
		screenCenterY_ = screenHeight_ / 2;
		BUFFER = 2.0f;
		lookAt_ = { 0,0, (int)screenWidth, (int)screenHeight };
		set_meters_to_show(metersToShowX, metersToShowY);
	}

	void Camera::set_meters_to_show(float meters_to_show_x, float meters_to_show_y)
	{
		if (meters_to_show_x <= 0.0f && meters_to_show_y <= 0.0f) {
			printf("You must provide at least one non zero argument");
			return;
		}
		metersToShowX_ = meters_to_show_x;
		metersToShowY_ = meters_to_show_y;
		if (meters_to_show_x == 0.0f || meters_to_show_y == 0.0f) {
			if (meters_to_show_y > 0.0f) { //if Y is configured, calculate X
				metersToShowX_ = (screenWidth_ / screenHeight_) * meters_to_show_y;
			}
			else { //if X is configured, calculate Y
				metersToShowY_ = (screenHeight_ / screenWidth_) * meters_to_show_x;
			}
		}
		halfDistX_ = (int)((metersToShowX_ + BUFFER) * 0.5f);
		halfDistY_ = (int)((metersToShowY_ + BUFFER) * 0.5f);
		pixelsPerMeterX_ = screenWidth_ / metersToShowX_;
		pixelsPerMeterY_ = screenHeight_ / metersToShowY_;
	}

	void Camera::look_at(Vector2 pos)
	{
		lookAt_.x = int(pos.x_) - (int)(screenWidth_ / 2.0f);
		lookAt_.y = int(pos.y_) - (int)(screenHeight_ / 2.0f);

		if (lookAt_.x < 0)
		{
			lookAt_.x = 0;
		}
		if (lookAt_.y < 0)
		{
			lookAt_.y = 0;
		}
		if (lookAt_.x > LEVEL_WIDTH - lookAt_.w)
		{
			lookAt_.x = LEVEL_WIDTH - lookAt_.w;
		}
		if (lookAt_.y > LEVEL_HEIGHT - lookAt_.h)
		{
			lookAt_.y = LEVEL_HEIGHT - lookAt_.h;
		}
	}

	void Camera::worldToScreen(Vector2 worldPos, Vector2& screenPos) const
	{
		screenPos.x_ = screenCenterX_ - ((lookAt_.x - (int)worldPos.x_) * pixelsPerMeterX_);
		screenPos.y_ = screenCenterY_ - ((lookAt_.y - (int)worldPos.y_) * pixelsPerMeterY_);
	}
}
