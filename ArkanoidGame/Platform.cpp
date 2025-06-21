#include "Platform.h"
#include "Ball.h"
#include "GameSettings.h"
#include "Sprite.h"
#include <algorithm>

namespace
{
	const std::string TEXTURE_ID = "platform";
}

namespace ArkanoidGame
{
	Platform::Platform(const sf::Vector2f& position)
		: GameObject(TEXTURES_PATH + TEXTURE_ID + ".png", position, PLATFORM_WIDTH, PLATFORM_HEIGHT)
	{
		
			
			baseScale = sprite.getScale();
		
	}

	

	void Platform::SetSizeMultiplier(float m)
	{
		sprite.setScale(baseScale.x * m, baseScale.y * m);
	}

	void Platform::ResetSizeMultiplier()
	{
		sprite.setScale(baseScale);
	}

	void Platform::Update(float timeDelta)
	{
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
		{
			Move(-timeDelta * PLATFORM_SPEED* speedMultiplier);
		}
		else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
		{
			Move(timeDelta * PLATFORM_SPEED* speedMultiplier);
		}
	}

	void Platform::Move(float speed)
	{
		auto pos = sprite.getPosition();
		auto bounds = sprite.getGlobalBounds();
		float halfW = bounds.width / 2.f;

		pos.x = std::clamp(pos.x + speed, halfW, SCREEN_WIDTH - halfW);
		sprite.setPosition(pos);

		
	}

	bool Platform::GetCollision(std::shared_ptr<Colladiable> collidable) const
	{
		auto ball = std::static_pointer_cast<Ball>(collidable);
		if (!ball) return false;

		auto sqr = [](float x) {
			return x * x;
		};
		const auto rect = sprite.getGlobalBounds();
		const auto ballPos = ball->GetPosition();
		if (ballPos.x < rect.left) {
			return sqr(ballPos.x - rect.left) + sqr(ballPos.y - rect.top) < sqr(BALL_SIZE / 2.0);
		}

		if (ballPos.x > rect.left + rect.width) {
			return sqr(ballPos.x - rect.left - rect.width) + sqr(ballPos.y - rect.top) < sqr(BALL_SIZE / 2.0);
		}

		return std::fabs(ballPos.y - rect.top) <= BALL_SIZE / 2.0;
	}

	bool Platform::CheckCollision(std::shared_ptr<Colladiable> collidable) {
		auto ball = std::static_pointer_cast<Ball>(collidable);
		if (!ball)
			return false;


		if (GetCollision(ball)) {
			auto rect = GetRect();
			auto ballPosInOlatform = (ball->GetPosition().x - (rect.left + rect.width / 2)) / (rect.width / 2);
			ball->ChangeAngle(90 - 20 * ballPosInOlatform);
			return true;
		}
		return false;
	}
}