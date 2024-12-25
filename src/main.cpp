#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <iostream>

#include "resources.hpp"
#include "game_object.hpp"

const auto ACCEL = 250.0f;
const auto ROTATE_ACCEL = 5.0f;

int main()
{
    try
    {
        auto &resourceManager = ResourceManager::getInstance();
        resourceManager.loadResource("spaceship", "spaceship.png");
        resourceManager.loadResource("mine", "mine.png");
        resourceManager.loadResource("torpedo", "torpedo.png");
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error loading resources: " << e.what() << '\n';
        return EXIT_FAILURE;
    }

    std::srand(std::time(nullptr));

    auto video_mode = sf::VideoMode({1920u, 1080u});

    auto window = sf::RenderWindow(video_mode, "CMake SFML Project");
    window.setFramerateLimit(60);

    sf::Clock clock;
    auto time = 0.f;

    TorpedoManager torpedoManager;

    auto player = Player("spaceship");

    player.reset(video_mode.size);

    std::vector<Enemy> enemies(50, Enemy("mine"));

    for (auto &enemy : enemies)
    {
        enemy.randomise(video_mode.size);
    }

    while (window.isOpen())
    {
        // Get the time between frames
        auto elapsed = clock.restart().asSeconds();
        time += elapsed;

        while (const auto event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
            {
                window.close();
            }

            // enemies[0].setPosition((sf::Vector2f)sf::Mouse::getPosition(window));

            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right))
            {
                // player.accelerate(sf::Vector2f(elapsed * ACCEL, 0));

                player.rotate_thrust(sf::degrees(ROTATE_ACCEL));
            }
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left))
            {
                // player.accelerate(sf::Vector2f(-elapsed * ACCEL, 0));

                player.rotate_thrust(sf::degrees(-ROTATE_ACCEL));
            }
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Down))
            {
                player.accelerate(sf::Vector2f(0, elapsed * ACCEL));
            }
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Up))
            {
                player.accelerate(sf::Vector2f(0, -elapsed * ACCEL));
            }
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Escape))
            {
                player.reset(video_mode.size);
            }

            // Fire torpedo via player
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Space))
            {
                player.fireTorpedo(torpedoManager, time);
            }
        }

        window.clear();

        // Physics...
        player.tick(elapsed);

        for (auto &enemy : enemies)
        {
            enemy.tick(elapsed);

            if (player.collide(enemy))
            {
                // player.reset(video_mode.size);

                auto player_vel = player.getVelocity();
                auto enemy_vel = enemy.getVelocity();

                calculateBounce(player.getPosition(), player_vel,
                                enemy.getPosition(), enemy_vel);

                player.setVelocity(player_vel);
                enemy.setVelocity(enemy_vel);
            }

            for (auto &enemy_inner : enemies)
            {
                if (enemy_inner.collide(enemy))
                {
                    auto enemy_inner_vel = enemy_inner.getVelocity();
                    auto enemy_vel = enemy.getVelocity();

                    calculateBounce(enemy_inner.getPosition(), enemy_inner_vel,
                                    enemy.getPosition(), enemy_vel);

                    enemy_inner.setVelocity(enemy_inner_vel);
                    enemy.setVelocity(enemy_vel);
                }
            }
        }

        torpedoManager.update(elapsed, enemies);

        // Drawing...
        window.draw(player);

        for (auto &enemy : enemies)
        {
            window.draw(enemy);
        }

        torpedoManager.render(window);

        window.display();
    }
}
