#include "pch.h"
#include "Paddle.h"

Paddle::Paddle(float paddleWidth, float paddleHeight, float paddleSpeed, Point2f paddlePosition)
{
	m_paddleWidth = paddleWidth;
	m_paddleHeight = paddleHeight;
	m_paddleSpeed = paddleSpeed;
	m_paddlePosition = paddlePosition;
}

Paddle::Paddle() {

}


float Paddle::GetPaddleHeight() 
{
	return m_paddleHeight;
}

float Paddle::GetPaddleWidth() 
{
	return m_paddleWidth;
}

float Paddle::GetPaddleSpeed()
{
	return m_paddleSpeed;
}

Point2f Paddle::GetPaddlePosition()
{
	return m_paddlePosition;
}

void Paddle::ChangePaddlePosition(int direction, float elapsetTime)
{
	if (direction == 0)
	{
		m_paddlePosition.x += m_paddleSpeed * elapsetTime;
	}
	else
	{
		m_paddlePosition.x -= m_paddleSpeed * elapsetTime;
	}
}
