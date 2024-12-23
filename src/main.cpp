#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <iostream>

float rand_f(float min, float max)
{
    auto precision = 1000.f;
    return ((float)(std::rand() % (int)((max - min) * precision)) / precision) + min;
}

class Character : public sf::CircleShape
{
public:
    Character()
        : velocity{sf::Vector2f(0, 0)}
    {
    }

    void accelerate(sf::Vector2f thrust)
    {
        this->velocity += thrust;
    }

    void tick(float elapsed)
    {
        this->move(velocity * elapsed);
    }

    bool collide(const sf::CircleShape &circle2)
    {
        auto &circle1 = (*this);

        auto relative = circle1.getPosition() - circle2.getPosition();

        auto distance = sqrtf(powf(relative.x, 2.f) + powf(relative.y, 2.f));

        auto radius_sum = circle1.getRadius() + circle2.getRadius();

        return distance < radius_sum;
    }

protected:
    sf::Vector2f velocity;
};

class Player : public Character
{
public:
    Player() : Character()
    {
        this->setRadius(40.f);
        this->setPointCount(100);
        this->setFillColor(sf::Color::Blue);

        // Make the origin the center of the circle
        this->setOrigin(sf::Vector2f(this->getRadius(), this->getRadius()));
    }

    void reset(sf::Vector2u size)
    {
        this->setPosition((sf::Vector2f)size / 2.f);
    }
};

class Enemy : public Character
{
public:
    Enemy() : Character()
    {
        this->setRadius(20.f);
        this->setPointCount(6);
        this->setFillColor(sf::Color::Red);

        // Make the origin the center of the circle
        this->setOrigin(sf::Vector2f(this->getRadius(), this->getRadius()));
    }

    void randomise(sf::Vector2u size)
    {
        this->setPosition(sf::Vector2f(std::rand() % size.x, std::rand() % size.y));

        this->velocity += sf::Vector2f(rand_f(-10.f, 10.f), rand_f(-10.f, 10.f));
    }
};

const auto ACCEL = 1000.0f;

int main()
{
    std::srand(std::time(nullptr));

    auto video_mode = sf::VideoMode({1920u, 1080u});

    auto window = sf::RenderWindow(video_mode, "CMake SFML Project");
    window.setFramerateLimit(60);

    sf::Clock clock;

    auto player = Player();

    player.reset(video_mode.size);

    std::vector<Enemy> enemies(20);

    for (auto &enemy : enemies)
    {
        enemy.randomise(video_mode.size);
    }

    while (window.isOpen())
    {
        // Get the time between frames
        auto elapsed = clock.restart().asSeconds();

        while (const auto event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
            {
                window.close();
            }

            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right))
            {
                player.accelerate(sf::Vector2f(elapsed * ACCEL, 0));
            }
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left))
            {
                player.accelerate(sf::Vector2f(-elapsed * ACCEL, 0));
            }
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Down))
            {
                player.accelerate(sf::Vector2f(0, elapsed * ACCEL));
            }
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Up))
            {
                player.accelerate(sf::Vector2f(0, -elapsed * ACCEL));
            }
        }

        window.clear();

        player.tick(elapsed);

        window.draw(player);

        for (auto &enemy : enemies)
        {
            enemy.tick(elapsed);

            if (player.collide(enemy))
            {
                player.reset(video_mode.size);
            }

            window.draw(enemy);
        }

        window.display();
    }
}
