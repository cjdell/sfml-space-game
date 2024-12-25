#pragma once

#include <iostream>

inline float rand_f(float min, float max)
{
    auto precision = 1000.f;
    return ((float)(std::rand() % (int)((max - min) * precision)) / precision) + min;
}

inline int min(int a, int b)
{
    return a > b ? b : a;
}
inline int max(int a, int b)
{
    return a > b ? a : b;
}

// Normalize a vector
inline sf::Vector2f normalize(const sf::Vector2f &v)
{
    float length = std::sqrt(v.x * v.x + v.y * v.y);
    if (length == 0)
        return sf::Vector2f(0.f, 0.f);
    return sf::Vector2f(v.x / length, v.y / length);
}

// Dot product of two vectors
inline float dot(const sf::Vector2f &v1, const sf::Vector2f &v2)
{
    return v1.x * v2.x + v1.y * v2.y;
}

// Calculate bounce after collision
inline void calculateBounce(sf::Vector2f pos1, sf::Vector2f &vel1,
                            sf::Vector2f pos2, sf::Vector2f &vel2)
{
    // Step 1: Compute the normalized collision vector
    sf::Vector2f collisionVector = normalize(pos2 - pos1);

    // Step 2: Compute the relative velocity
    sf::Vector2f relativeVelocity = vel2 - vel1;

    // Step 3: Compute the velocity along the collision normal
    float velocityAlongNormal = dot(relativeVelocity, collisionVector);

    // If the objects are already moving away, no need to bounce
    if (velocityAlongNormal > 0)
        return;

    // Step 4: Update velocities (elastic collision)
    vel1 += collisionVector * velocityAlongNormal;
    vel2 -= collisionVector * velocityAlongNormal;
}
