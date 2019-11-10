#include "pch.h"
#include "Color.h"
#include "Block.h"
#include <array>

Block::Block(float lowerLimit, float horizontalValue, Color& color, std::array<float, 4>& colorValue) 
{
	m_lowerLimit = lowerLimit;
	m_startingHorizontalValue = horizontalValue;
	m_color = color;
	m_colorValues = colorValue;
}

Block::Block() 
{

}

float Block::GetLowerLimit()
{
	return m_lowerLimit;
}

float Block::GetStartingHorizontalValue()
{
	return m_startingHorizontalValue;
}

Color& Block::GetColor()
{
	return m_color;
}

std::array<float, 4>& Block::GetColorValue()
{
	return m_colorValues;
}

bool Block::IsDestroyed() 
{
	return m_destroyed;
}

void Block::SetDestroyed() 
{
	m_destroyed = true;
}