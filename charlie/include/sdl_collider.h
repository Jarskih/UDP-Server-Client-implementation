#pragma once

#include <SDL.h>

namespace charlie
{
	enum class COLLIDERTYPE
	{
		RECTANGLE
	};

	class Collider
	{
	protected:
		SDL_Rect m_bounds;
		COLLIDERTYPE m_type;
	public:
		void SetSize(int width, int height);
		void SetPosition(int p_x, int p_y);
		virtual SDL_Rect GetBounds() = 0;
		COLLIDERTYPE GetType() { return m_type; };
		Collider() : m_bounds(), m_type()
		{
		};
		virtual ~Collider() {};
	};

	class RectangleCollider : public Collider
	{
	public:
		SDL_Rect GetBounds();
		RectangleCollider(int p_x, int p_y, int p_w, int p_h);
	};
}