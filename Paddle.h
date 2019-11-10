#pragma once
#include "structs.h"

class Paddle
{
	float m_paddleHeight{};
	float m_paddleWidth{};
	float m_paddleSpeed{};
	Point2f m_paddlePosition{};


public:
	Paddle(float paddleWidth, float paddleHeight, float paddleSpeed, Point2f paddlePosition);
	Paddle();

	float GetPaddleHeight();
	float GetPaddleWidth();
	float GetPaddleSpeed();
	Point2f GetPaddlePosition();

	void ChangePaddlePosition(int direction, float elapsetTime);
};
