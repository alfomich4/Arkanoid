#pragma once
#include "SFML/Graphics.hpp"
#include "GameObject.h"
#include "Collidable.h"

namespace ArkanoidGame
{
	class Ball final : public GameObject, public Colladiable
	{
	public:
		Ball(const sf::Vector2f& position);
		~Ball() = default;
		void Update(float timeDelta) override;

		void InvertDirectionX();
		void InvertDirectionY();

		bool GetCollision(std::shared_ptr<Colladiable> collidable) const  override;
		void ChangeAngle(float x);
		void SetSpeedMultiplier(float m) { speedMultiplier = m; }
		void ResetSpeedMultiplier() { speedMultiplier = 1.f; }
	private:
		void OnHit();
		sf::Vector2f direction;
		float lastAngle = 90;
		float speedMultiplier = 1.f;
	};
}


