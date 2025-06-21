#pragma once
#include "SFML/Graphics.hpp"
#include "GameObject.h"
#include "Collidable.h"

namespace ArkanoidGame
{
	class Platform : public GameObject, public Colladiable
	{
	public:
		Platform(const sf::Vector2f& position);
		void Update(float timeDelta) override;
		
		bool GetCollision(std::shared_ptr<Colladiable> collidable) const override;
		void OnHit() override {}
		bool CheckCollision(std::shared_ptr<Colladiable> collidable) override;
		void SetSpeedMultiplier(float m) { speedMultiplier = m; }
		void ResetSpeedMultiplier() { speedMultiplier = 1.f; }

		void SetSizeMultiplier(float m);  // увеличивает размер спрайта
		void ResetSizeMultiplier();
	private:
		void Move(float speed);
		float speedMultiplier = 1.f;
		sf::Vector2f baseScale;
	};
}