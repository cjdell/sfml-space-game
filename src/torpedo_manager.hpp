#pragma once

#include <vector>
#include <SFML/Graphics.hpp>

class Enemy;
class Torpedo;

#include "game_object.hpp"

class TorpedoManager
{
public:
    void fire(const sf::Vector2f &position, sf::Angle rotation, const sf::Vector2f &velocity);

    void update(float elapsed, std::vector<Enemy> &enemies);

    void render(sf::RenderWindow &window) const;

private:
    std::vector<Torpedo> _torpedoes;
};
