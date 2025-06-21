#include "Bonus.h"

Bonus::Bonus(BonusType type, const sf::Texture& texture, sf::Vector2f position)
    : type(type)
{
    setTexture(texture);
    setPosition(position);
}

void Bonus::update(float dt)
{
    move(0, speed * dt);
    if (getPosition().y > 800) 
        active = false;
}

BonusType Bonus::getType() const { return type; }
bool Bonus::isActive() const { return active; }
void Bonus::deactivate() { active = false; }