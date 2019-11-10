#pragma once
#include "structs.h"

class Ball
{
	float m_radius{};
	Point2f m_position{};
	float m_horizontalLocation{};
	float m_verticalLocation{};
	float m_xSpeed{};
	float m_ySpeed{};

public:
	Ball(Point2f position, float radius, float initialSpeed);
	Ball();

	Point2f GetPosition();
	float GetRadius();
	float GetXSpeed();
	float GetYSpeed();

	void SetXSpeed(float xSpeed);
	void SetYSpeed(float ySpeed);
	void SetLocation(Point2f position);

	void IncreseaSpeedDifficulty();
	void DecreaseSpeedDifficulty();

	void ChangeLocation(float elapsedSeconds);
};

