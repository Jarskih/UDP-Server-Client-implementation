#pragma once

#include <SDL.h>

#include "charlie.hpp"

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
		Collider();
		virtual ~Collider();
		void SetSize(int width, int height);
		void SetPosition(int p_x, int p_y);
		void SetPosition(Vector2 pos);
		virtual SDL_Rect GetBounds();
		COLLIDERTYPE GetType() const { return m_type; }
	};

	class RectangleCollider : public Collider
	{
	public:
		RectangleCollider() = default;
		RectangleCollider(int p_x, int p_y, int p_w, int p_h);
		SDL_Rect GetBounds() override;
	};
}
