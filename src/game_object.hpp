#pragma once

#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <iostream>

class TorpedoManager;

#include "util.hpp"
#include "resources.hpp"
#include "torpedo_manager.hpp"

class GameObject : public sf::Sprite
{
public:
    GameObject(const std::string &resourceName);

    void accelerate(sf::Vector2f thrust);

    void rotate_thrust(sf::Angle angle);

    void tick(float elapsed);

    bool collide(const GameObject &other) const;

    inline sf::Vector2f getVelocity() { return this->_velocity; }
    inline void setVelocity(sf::Vector2f velocity) { this->_velocity = velocity; }

protected:
    sf::Image _image;

    // Units per second
    sf::Vector2f _velocity;

    // Degrees per second
    sf::Angle _angular_velocity;
};

const float TORPEDO_INTERVAL = 0.5f;

class Player : public GameObject
{
public:
    Player(const std::string &resourceName);

    void reset(sf::Vector2u size);

    void fireTorpedo(TorpedoManager &torpedoManager, float time);

private:
    float _last_torpedo_time = 0.f;
};

class Enemy : public GameObject
{
public:
    Enemy(const std::string &resourceName);

    void randomise(sf::Vector2u size);

    void hit();
};

class Torpedo : public GameObject
{
public:
    explicit Torpedo(const std::string &resourceName);

    void fire(const sf::Vector2f &position, sf::Angle rotation, const sf::Vector2f &velocity);

    void update(float elapsed);

    bool isActive() const;

    void deactivate();

private:
    bool _active;
};
