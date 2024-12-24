#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <iostream>

float rand_f(float min, float max)
{
    auto precision = 1000.f;
    return ((float)(std::rand() % (int)((max - min) * precision)) / precision) + min;
}

int min(int a, int b)
{
    return a > b ? b : a;
}
int max(int a, int b)
{
    return a > b ? a : b;
}

class Character : public sf::Sprite
{
public:
    Character(sf::Image &image, sf::Texture &texture)
        : Sprite(texture), _image{image}
    {
        auto size = texture.getSize().x / 2.f;

        // Make the origin the center of the texture
        this->setOrigin(sf::Vector2f(size, size));
    }

    void accelerate(sf::Vector2f thrust)
    {
        this->_velocity += thrust.rotatedBy(this->getRotation());
    }

    void rotate_thrust(sf::Angle angle)
    {
        this->_angular_velocity += angle;
    }

    void tick(float elapsed)
    {
        this->move(_velocity * elapsed);
        this->rotate(_angular_velocity * elapsed);
    }

    bool collide(const Character &other)
    {
        auto b1 = this->getGlobalBounds();
        auto b2 = other.getGlobalBounds();

        auto intersection = b1.findIntersection(b2);

        if (!intersection.has_value())
        {
            return false;
        }

        // std::cout << "Intersecting" << std::endl;

        auto image_size = this->_image.getSize();
        auto other_image_size = other._image.getSize();

        auto inv_transform = this->getInverseTransform();
        auto transform = this->getTransform();
        auto other_inv_transform = other.getInverseTransform();

        auto pixels_intersecting = inv_transform.transformRect(intersection.value());

        // std::cout << pixels_intersecting.position.x << " " << pixels_intersecting.position.y << std::endl;
        // std::cout << pixels_intersecting.size.x << " " << pixels_intersecting.size.y << std::endl;
        // std::cout << pixels_intersecting.position.x + pixels_intersecting.size.x << " " << pixels_intersecting.position.y + pixels_intersecting.size.y << std::endl;

        // Calculate the range of pixels we need to look at...
        auto x1 = max(0, (int)pixels_intersecting.position.x);
        auto x2 = min(image_size.x, (int)pixels_intersecting.position.x + (int)pixels_intersecting.size.x);

        auto y1 = max(0, (int)pixels_intersecting.position.y);
        auto y2 = min(image_size.y, (int)pixels_intersecting.position.y + (int)pixels_intersecting.size.y);

        for (auto x = x1; x < x2; x++)
        {
            for (auto y = y1; y < y2; y++)
            {
                auto pixel_vec = sf::Vector2u(x, y);
                // std::cout << pixel_vec.x << " " << pixel_vec.y << std::endl;
                auto brightness = this->_image.getPixel(pixel_vec).a;

                if (brightness > 10)
                {
                    // There is a pixel here, so calculate where it is in world space
                    auto world_vec = transform.transformPoint((sf::Vector2f)pixel_vec);

                    // Get the local pixel co-ordinate of the other object
                    auto other_local_vec = other_inv_transform.transformPoint(world_vec);

                    if (other_local_vec.x >= 0 && other_local_vec.x < other_image_size.x)
                    {
                        if (other_local_vec.y >= 0 && other_local_vec.y < other_image_size.y)
                        {
                            auto p_vec = (sf::Vector2u)other_local_vec;

                            // If there is a pixel in the other object then it is a collision!
                            if (other._image.getPixel(p_vec).a > 10)
                            {
                                // std::cout << pixel_vec.x << " " << pixel_vec.y << std::endl;
                                // std::cout << world_vec.x << " " << world_vec.y << std::endl;
                                // std::cout << other_local_vec.x << " " << other_local_vec.y << std::endl;
                                return true;
                            }
                        }
                    }
                }
            }
        }

        return false;
    }

protected:
    sf::Image _image;

    // Units per second
    sf::Vector2f _velocity;

    // Degrees per second
    sf::Angle _angular_velocity;
};

class Player : public Character
{
public:
    Player(sf::Image &image, sf::Texture &texture) : Character(image, texture)
    {
        this->setScale(sf::Vector2f(0.25f, 0.25f));
    }

    void reset(sf::Vector2u size)
    {
        this->setPosition((sf::Vector2f)size / 2.f);
        this->setRotation(sf::degrees(0));

        this->_velocity = sf::Vector2f();
        this->_angular_velocity = sf::Angle::Zero;
    }
};

class Enemy : public Character
{
public:
    Enemy(sf::Image &image, sf::Texture &texture) : Character(image, texture)
    {
        this->setScale(sf::Vector2f(0.1f, 0.1f));
    }

    void randomise(sf::Vector2u size)
    {
        this->setPosition(sf::Vector2f(std::rand() % size.x, std::rand() % size.y));

        this->_velocity += sf::Vector2f(rand_f(-25.f, 25.f), rand_f(-25.f, 25.f));
    }
};

const auto ACCEL = 1000.0f;
const auto ROTATE_ACCEL = 5.0f;

int main()
{
    std::srand(std::time(nullptr));

    auto video_mode = sf::VideoMode({1920u, 1080u});

    auto window = sf::RenderWindow(video_mode, "CMake SFML Project");
    window.setFramerateLimit(60);

    auto spaceship_image = sf::Image("spaceship.png");
    auto spaceship_texture = sf::Texture(spaceship_image);
    spaceship_texture.setSmooth(true);

    auto mine_image = sf::Image("mine.png");
    auto mine_texture = sf::Texture(mine_image);
    mine_texture.setSmooth(true);

    sf::Clock clock;

    auto player = Player(spaceship_image, spaceship_texture);

    player.reset(video_mode.size);

    std::vector<Enemy> enemies(20, Enemy(mine_image, mine_texture));

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
