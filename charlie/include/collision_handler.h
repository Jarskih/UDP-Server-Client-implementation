#pragma once
#include "sdl_collider.h"

class Collider;
class RectangleCollider;

namespace charlie
{
	struct CollisionHandler
	{
		static bool IsColliding(RectangleCollider& p_lhs, RectangleCollider& p_rhs);
		bool IsColliding(Collider& p_lhs, Collider& p_rhs) const; // Axis Aligned Bounding Box (AABB)
	};
}