#include "sdl_collider.h"

namespace charlie
{
	void Collider::SetSize(int width, int height)
	{
		m_bounds.w = width;
		m_bounds.h = height;
	}

	void Collider::SetPosition(int p_x, int p_y)
	{
		m_bounds.x = p_x; m_bounds.y = p_y;
	}

	SDL_Rect Collider::GetBounds()
	{
		return m_bounds;
	}

	Collider::Collider(): m_bounds(), m_type()
	{
	}

	Collider::~Collider() = default;

	RectangleCollider::RectangleCollider(int p_x, int p_y, int p_w, int p_h)
	{
		m_type = COLLIDERTYPE::RECTANGLE;
		m_bounds.x = p_x; m_bounds.y = p_y;
		m_bounds.w = p_w; m_bounds.h = p_h;
	}

	SDL_Rect RectangleCollider::GetBounds()
	{
		return m_bounds;
	}
}
