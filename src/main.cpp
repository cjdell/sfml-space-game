#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <iostream>

float rand_f(float min, float max)
{
    auto precision = 1000.f;
    return ((float)(std::rand() % (int)((max - min) * precision)) / precision) + min;
}

class Character : public sf::Sprite
{
public:
    Character(sf::Texture &texture)
        : Sprite(texture)
    {
        auto size = texture.getSize().x / 2.f;

        // Make the origin the center of the texture
        this->setOrigin(sf::Vector2f(size, size));

        this->setScale(sf::Vector2f(0.25f, 0.25f));
    }

    void accelerate(sf::Vector2f thrust)
    {
        this->velocity += thrust;
    }

    void tick(float elapsed)
    {
        this->move(velocity * elapsed);
    }

    bool collide(const Character &char2)
    {
        auto &char1 = (*this);

        auto b1 = char1.getGlobalBounds();
        auto b2 = char2.getGlobalBounds();

        return b1.findIntersection(b2).has_value();
    }

protected:
    sf::Vector2f velocity;
    float _radius;
};

class Player : public Character
{
public:
    Player(sf::Texture &texture) : Character(texture)
    {
    }

    void reset(sf::Vector2u size)
    {
        this->setPosition((sf::Vector2f)size / 2.f);
    }
};

class Enemy : public Character
{
public:
    Enemy(sf::Texture &texture) : Character(texture)
    {
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

    auto texture = sf::Texture(sf::Image("spaceship.png"));
    texture.setSmooth(true);

    sf::Clock clock;

    auto player = Player(texture);

    player.reset(video_mode.size);

    std::vector<Enemy> enemies(20, Enemy(texture));

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
