#include "collision_handler.h"
#include <SDL.h>
#include "sdl_collider.h"

namespace charlie
{

	bool CollisionHandler::IsColliding(RectangleCollider* p_lhs, RectangleCollider* p_rhs)
	{
		SDL_Rect left = p_lhs->GetBounds();
		const SDL_Rect right = p_rhs->GetBounds();
		if (left.x > right.x + right.w ||
			left.x + left.w < right.x ||
			left.y > right.y + right.h ||
			left.y + left.h < right.y)
		{
			return false;
		}
		return true;
	}

	bool CollisionHandler::IsColliding(Collider* p_lhs, Collider* p_rhs)
	{
		if (p_lhs->GetType() == COLLIDERTYPE::RECTANGLE && p_rhs->GetType() == COLLIDERTYPE::RECTANGLE)
		{
			return IsColliding(dynamic_cast<RectangleCollider*>(p_lhs), dynamic_cast<RectangleCollider*>(p_rhs));
		}
		return false;
	}
}
