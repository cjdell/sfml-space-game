#include "torpedo_manager.hpp"

void TorpedoManager::fire(const sf::Vector2f &position, sf::Angle rotation, const sf::Vector2f &velocity)
{
    // Reuse inactive torpedoes if available
    for (auto &torpedo : _torpedoes)
    {
        if (!torpedo.isActive())
        {
            torpedo.fire(position, rotation, velocity);
            return;
        }
    }

    // Otherwise, create a new torpedo
    _torpedoes.emplace_back("torpedo");
    _torpedoes.back().fire(position, rotation, velocity);
}

void TorpedoManager::update(float elapsed, std::vector<Enemy> &enemies)
{
    for (auto &torpedo : _torpedoes)
    {
        if (torpedo.isActive())
        {
            torpedo.update(elapsed);

            // Check for collisions with enemies
            for (auto &enemy : enemies)
            {
                if (enemy.collide(torpedo))
                {
                    torpedo.deactivate();

                    enemy.hit();

                    break;
                }
            }
        }
    }
}

void TorpedoManager::render(sf::RenderWindow &window) const
{
    for (const auto &torpedo : _torpedoes)
    {
        if (torpedo.isActive())
        {
            window.draw(torpedo);
        }
    }
}
