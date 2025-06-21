#pragma once
#include <SFML/Graphics.hpp>

enum class BonusType { SizeBuff, SpeedBuff, ReduceSpeed };

class Bonus : public sf::Sprite {
public:
    Bonus(BonusType type, const sf::Texture& texture, sf::Vector2f position);

    void update(float dt);
    BonusType getType() const;
    bool isActive() const;
    void deactivate();

private:
    BonusType type;
    bool active = true;
    float speed = 200.f; 
};