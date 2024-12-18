#pragma once

#include <SFML/Graphics.hpp>
#include <cmath>

inline float Vector2fLength(sf::Vector2f pos1, sf::Vector2f pos2)
{
    float dx = pos2.x - pos1.x;
    float dy = pos2.y - pos1.y;
    return std::sqrt(dx * dx + dy * dy);
}