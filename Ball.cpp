#include "pch.h"
#include "Ball.h"

Ball::Ball(Point2f position, float radius, float initialSpeed)
{
	m_radius = radius;
	m_position = position;
	m_xSpeed = initialSpeed;
	m_ySpeed = initialSpeed;
}

Ball::Ball() 
{

}

float Ball::GetRadius() 
{
	return m_radius;
}

Point2f Ball::GetPosition() 
{
	return m_position;
}

float Ball::GetXSpeed() 
{
	return m_xSpeed;
}

float Ball::GetYSpeed() 
{
	return m_ySpeed;
}

void Ball::SetXSpeed(float xSpeed) 
{
	m_xSpeed = xSpeed;
}

void Ball::SetYSpeed(float ySpeed) 
{
	m_ySpeed = ySpeed;
}

void Ball::SetLocation(Point2f position) 
{
	m_position = position;
}

void Ball::ChangeLocation(float elapsedSeconds) 
{
	m_position.x += m_xSpeed * elapsedSeconds;
	m_position.y += m_ySpeed * elapsedSeconds;
}

void Ball::IncreseaSpeedDifficulty() 
{
	if (m_xSpeed >= 0) m_xSpeed += 10.f;
	else m_xSpeed -= 10.f;

	if (m_ySpeed >= 0) m_ySpeed += 10.f;
	else m_ySpeed -= 10.f;
}

void Ball::DecreaseSpeedDifficulty() {
	if (m_xSpeed >= 0) m_xSpeed -= 10.f;
	else m_xSpeed += 10.f;

	if (m_ySpeed >= 0) m_ySpeed -= 10.f;
	else m_ySpeed += 10.f;
}