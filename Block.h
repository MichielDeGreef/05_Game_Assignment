#pragma once
#include <iostream>
#include <array>
#include "Color.h"

class Block
{
	float m_lowerLimit{};
	float m_startingHorizontalValue{};
	bool m_destroyed{ false };
	Color m_color{ Color::white };
	std::array<float, 4> m_colorValues{};

public:
	Block();
	Block(float lowerLimit, float horizontalValue, Color& color, std::array<float, 4>& colorValue);

	float GetLowerLimit();
	float GetStartingHorizontalValue();
	Color& GetColor();
	std::array<float, 4>& GetColorValue();

	bool IsDestroyed();
	void SetDestroyed();


};