#include "game_object.hpp"

GameObject::GameObject(const std::string &resourceName)
    : Sprite(ResourceManager::getInstance().getTexture(resourceName)),
      _image(ResourceManager::getInstance().getImage(resourceName))
{
    auto size = getTexture().getSize().x / 2.f;
    setOrigin(sf::Vector2f(size, size));
}

void GameObject::accelerate(sf::Vector2f thrust)
{
    this->_velocity += thrust.rotatedBy(this->getRotation());
}

void GameObject::rotate_thrust(sf::Angle angle)
{
    this->_angular_velocity += angle;
}

void GameObject::tick(float elapsed)
{
    this->move(_velocity * elapsed);
    this->rotate(_angular_velocity * elapsed);
}

bool GameObject::collide(const GameObject &other) const
{
    auto b1 = this->getGlobalBounds();
    auto b2 = other.getGlobalBounds();

    auto intersection = b1.findIntersection(b2);

    if (!intersection.has_value())
    {
        return false;
    }

    auto image_size = this->_image.getSize();
    auto other_image_size = other._image.getSize();

    auto inv_transform = this->getInverseTransform();
    auto transform = this->getTransform();
    auto other_inv_transform = other.getInverseTransform();

    auto pixels_intersecting = inv_transform.transformRect(intersection.value());

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
                            return true;
                        }
                    }
                }
            }
        }
    }

    return false;
}

Player::Player(const std::string &resourceName) : GameObject(resourceName)
{
    this->setScale(sf::Vector2f(0.25f, 0.25f));
}

void Player::reset(sf::Vector2u size)
{
    this->setPosition((sf::Vector2f)size / 2.f);
    this->setRotation(sf::degrees(0));

    this->_velocity = sf::Vector2f();
    this->_angular_velocity = sf::Angle::Zero;
}

void Player::fireTorpedo(TorpedoManager &torpedoManager, float time)
{
    if (this->_last_torpedo_time > time - TORPEDO_INTERVAL)
    {
        return;
    }

    this->_last_torpedo_time = time;

    auto direction = sf::Vector2f(0.f, -300.f).rotatedBy(this->getRotation());

    torpedoManager.fire(this->getPosition(), this->getRotation(), direction);
}

Enemy::Enemy(const std::string &resourceName) : GameObject(resourceName)
{
    this->setScale(sf::Vector2f(0.1f, 0.1f));
}

void Enemy::randomise(sf::Vector2u size)
{
    this->setPosition(sf::Vector2f(std::rand() % size.x, std::rand() % size.y));

    this->_velocity += sf::Vector2f(rand_f(-25.f, 25.f), rand_f(-25.f, 25.f));
}

void Enemy::hit()
{
    this->setScale(sf::Vector2f(0.f, 0.f));
}

Torpedo::Torpedo(const std::string &resourceName)
    : GameObject(resourceName), _active(false)
{
    this->setScale(sf::Vector2f(0.05f, 0.05f));
}

void Torpedo::fire(const sf::Vector2f &position, sf::Angle rotation, const sf::Vector2f &velocity)
{
    this->setPosition(position);
    this->setRotation(rotation);

    this->_velocity = velocity;
    this->_active = true;
}

void Torpedo::update(float elapsed)
{
    if (!this->_active)
    {
        return;
    }

    // Move the torpedo
    this->move(this->_velocity * elapsed);

    // Deactivate if it goes out of bounds
    auto pos = this->getPosition();

    if (pos.x < 0 || pos.x > 1920 || pos.y < 0 || pos.y > 1080)
    {
        this->_active = false;
    }
}

bool Torpedo::isActive() const { return this->_active; }

void Torpedo::deactivate() { this->_active = false; }
