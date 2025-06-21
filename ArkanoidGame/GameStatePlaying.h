#pragma once
#include "SFML/Graphics.hpp"
#include "SFML/Audio.hpp"
#include "GameStateData.h"
#include "Platform.h"
#include "Ball.h"


namespace ArkanoidGame
{
	class Game;
	class Block;
	enum class BonusType { EnlargePlatform, FastPlatform, SlowBall };

	struct Bonus
	{
		BonusType      type;
		sf::Sprite     sprite;
		sf::Texture* texture;    // указатель на одно из трёх заранее загруженных
	};

	class GameStatePlayingData : public GameStateData
	{

	public:
		void Init() override;
		void HandleWindowEvent(const sf::Event& event) override;
		void Update(float timeDelta) override;
		void Draw(sf::RenderWindow& window) override;

	private:
		void createBlocks();
		void GetBallInverse(const sf::Vector2f& ballPos, const sf::FloatRect& blockRect, bool& needInverseDirX,
			bool& needInverseDirY);

		// Resources
		
		sf::Font font;
		sf::SoundBuffer eatAppleSoundBuffer;
		sf::SoundBuffer gameOverSoundBuffer;
		sf::Texture bonusTextureSize, bonusTextureSpeed, bonusTextureReduce;

		// Game data
		std::vector<std::shared_ptr<GameObject>> gameObjects;
		std::vector<std::shared_ptr<Block>> blocks;
		

		// UI data
		std::vector<Bonus> fallingBonuses;
		struct ActiveEffect { BonusType type; float expiresAt; };
		std::vector<ActiveEffect> activeEffects;
		sf::Texture sizeBuffTex, speedBuffTex, reduceSpeedTex;
		float elapsedTime = 0.f;         
		int score = 0;
		int lives = 3;
		sf::Text livesText;
		sf::Text scoreText;
		sf::Text inputHintText;
		sf::RectangleShape background;
		sf::Vector2f initialBallPos;            
		sf::Vector2f initialPlatformPos;        

		// Sounds
		sf::Sound gameOverSound;
	};
}
