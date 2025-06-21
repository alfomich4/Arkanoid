#include "GameStatePlaying.h"
#include "Application.h"
#include "Block.h"
#include "Game.h"
#include "Text.h"
#include "ThreeHitBlock.h"

#include "randomizer.h" 
#include <assert.h>
#include <sstream>


namespace ArkanoidGame
{
	void GameStatePlayingData::Init()
	{	
		// Init game resources (terminate if error)
		assert(font.loadFromFile(FONTS_PATH + "Roboto-Regular.ttf"));
		assert(gameOverSoundBuffer.loadFromFile(SOUNDS_PATH + "Death.wav"));


		// Init background
		background.setSize(sf::Vector2f(SCREEN_WIDTH, SCREEN_HEIGHT));
		background.setPosition(0.f, 0.f);
		background.setFillColor(sf::Color(0, 0, 0));

		assert(sizeBuffTex.loadFromFile(TEXTURES_PATH + "SizeBuff.png"));
		assert(speedBuffTex.loadFromFile(TEXTURES_PATH + "SpeedBuff.png"));
		assert(reduceSpeedTex.loadFromFile(TEXTURES_PATH + "ReduceSpeed.png"));

		score = 0;
		lives = 3;
		scoreText.setFont(font);
		scoreText.setCharacterSize(24);
		scoreText.setFillColor(sf::Color::Yellow);

		livesText.setFont(font);
		livesText.setCharacterSize(24);
		livesText.setFillColor(sf::Color::Red);
		

		inputHintText.setFont(font);
		inputHintText.setCharacterSize(24);
		inputHintText.setFillColor(sf::Color::White);
		inputHintText.setString("Use arrow keys to move, ESC to pause");
		inputHintText.setOrigin(GetTextOrigin(inputHintText, { 1.f, 0.f }));

		initialPlatformPos = { SCREEN_WIDTH / 2.f, SCREEN_HEIGHT - PLATFORM_HEIGHT / 2.f };
		initialBallPos = { SCREEN_WIDTH / 2.f, SCREEN_HEIGHT - PLATFORM_HEIGHT - BALL_SIZE / 2.f };

		gameObjects.clear();
		gameObjects.emplace_back(std::make_shared<Platform>(initialPlatformPos));
		gameObjects.emplace_back(std::make_shared<Ball>(initialBallPos));
		createBlocks();

		// Init sounds
		gameOverSound.setBuffer(gameOverSoundBuffer);
	}

	void GameStatePlayingData::HandleWindowEvent(const sf::Event& event)
	{
		if (event.type == sf::Event::KeyPressed)
		{
			if (event.key.code == sf::Keyboard::Escape)
			{
				Application::Instance().GetGame().PushState(GameStateType::ExitDialog, false);
			}
		}
	}

	

	void GameStatePlayingData::Update(float timeDelta)
	{
		elapsedTime += timeDelta;

		// Перебираем падающие бонусы
		for (auto it = fallingBonuses.begin(); it != fallingBonuses.end(); )
		{
			// падают вниз, скорость падения 200 px/sec
			it->sprite.move(0.f, 200.f * timeDelta);

			// если подхватил платформой
			if (std::dynamic_pointer_cast<Platform>(gameObjects[0])->GetRect().intersects(
				it->sprite.getGlobalBounds()))
			{
				// активируем эффект на 10 сек
				activeEffects.push_back({ it->type, elapsedTime + 10.f });
				it = fallingBonuses.erase(it);
			}
			// если улетел за экран
			else if (it->sprite.getPosition().y > SCREEN_HEIGHT)
			{
				it = fallingBonuses.erase(it);
			}
			else
			{
				++it;
			}
		}

		auto platform = std::dynamic_pointer_cast<Platform>(gameObjects[0]);
		auto ball = std::dynamic_pointer_cast<Ball>(gameObjects[1]);

		
		platform->ResetSizeMultiplier();
		platform->ResetSpeedMultiplier();
		ball->ResetSpeedMultiplier();

		
		for (auto it = activeEffects.begin(); it != activeEffects.end(); )
		{
			if (elapsedTime <= it->expiresAt)
			{
				// ещё действует
				switch (it->type)
				{
				case BonusType::EnlargePlatform:
					platform->SetSizeMultiplier(2.f);
					break;
				case BonusType::FastPlatform:
					platform->SetSpeedMultiplier(2.f);
					break;
				case BonusType::SlowBall:
					ball->SetSpeedMultiplier(0.5f);
					break;
				}
				++it;
			}
			else
			{
				
				it = activeEffects.erase(it);
			}
		}
		
		static auto updateFunctor = [timeDelta](auto obj) { obj->Update(timeDelta); };
		std::for_each(gameObjects.begin(), gameObjects.end(), updateFunctor);
		std::for_each(blocks.begin(), blocks.end(), updateFunctor);

		
		bool isCollision = platform->CheckCollision(ball);

		
		bool ballBelow = ball->GetPosition().y > platform->GetRect().top;

		if (ballBelow && !isCollision)
		{
			
			if (lives > 1)
			{
				
				--lives;
				

				
				gameObjects[0] = std::make_shared<Platform>(initialPlatformPos);
				gameObjects[1] = std::make_shared<Ball>(initialBallPos);

				
				return;
			}
			else
			{
				
				Game& game = Application::Instance().GetGame();
				game.UpdateRecord(PLAYER_NAME, score);
				gameOverSound.play();
				game.PushState(GameStateType::GameOver, false);
				return;
			}
		}

		
		bool needInverseDirX = false;
		bool needInverseDirY = false;

		bool hasBrokeOneBlock = false;
		std::vector<std::shared_ptr<Block>> brokenBlocks;

		blocks.erase(
			std::remove_if(blocks.begin(), blocks.end(),
				[ball, &hasBrokeOneBlock, &needInverseDirX, &needInverseDirY, &brokenBlocks, this](auto block) {
					
					if (!hasBrokeOneBlock && block->CheckCollision(ball))
					{
						hasBrokeOneBlock = true;
						GetBallInverse(ball->GetPosition(), block->GetRect(), needInverseDirX, needInverseDirY);
					}
					
					if (block->IsBroken())
					{
						brokenBlocks.push_back(block);
						return true;
					}
					return false;
				}),
			blocks.end()
		);

		
		for (auto& b : brokenBlocks)
		{
			if (std::dynamic_pointer_cast<ThreeHitBlock>(b))
			{
				score += 2;
			}
			else
			{
				score += 1;
			}
		}
		for (auto& b : brokenBlocks)
		{
			// 10% шанс выпасть бонусу
			if (random<float>(0.f, 1.f) < 1.f)
			{
				
				int r = random<int>(0, 2);
				Bonus bonus;
				bonus.type = static_cast<BonusType>(r);
				switch (bonus.type)
				{
				case BonusType::EnlargePlatform:
					bonus.texture = &sizeBuffTex;    break;
				case BonusType::FastPlatform:
					bonus.texture = &speedBuffTex;   break;
				case BonusType::SlowBall:
					bonus.texture = &reduceSpeedTex; break;
				}
				bonus.sprite.setTexture(*bonus.texture);
				// центрируем и задаём размер
				InitSprite(bonus.sprite, 32, 32, *bonus.texture);
				// ставим сверху блока
				bonus.sprite.setPosition(b->GetPosition());
				fallingBonuses.push_back(bonus);
			}
		}

		scoreText.setString("Score: " + std::to_string(score));
		livesText.setString("Lives: " + std::to_string(lives));

		// 7) Инвертируем направление мяча, если нужно
		if (needInverseDirX)
		{
			ball->InvertDirectionX();
		}
		if (needInverseDirY)
		{
			ball->InvertDirectionY();
		}

		

		// 8) Проверяем, выиграл ли игрок (не осталось блоков)
		Game& game = Application::Instance().GetGame();
		if (blocks.empty())
		{
			game.PushState(GameStateType::GameWin, false);
		}
		
	}


	void GameStatePlayingData::Draw(sf::RenderWindow& window)
	{
		
		// Draw background
		window.draw(background);
		for (auto& b : fallingBonuses)
		{
			window.draw(b.sprite);
		}
		
		static auto drawFunc = [&window](auto block) { block->Draw(window); };
		// Draw game objects
		std::for_each(gameObjects.begin(), gameObjects.end(), drawFunc);
		std::for_each(blocks.begin(), blocks.end(), drawFunc);

		scoreText.setOrigin(GetTextOrigin(scoreText,{ 0.f, 0.f }));
		scoreText.setPosition(10.f, 10.f);
		window.draw(scoreText);
		livesText.setOrigin(GetTextOrigin(scoreText,{ 0.f,0.f }));
		livesText.setPosition(120.f, 10.f);
		window.draw(livesText);

		sf::Vector2f viewSize = window.getView().getSize();
		inputHintText.setPosition(viewSize.x - 10.f, 10.f);
		window.draw(inputHintText);
	}

	void GameStatePlayingData::createBlocks() 
	{
		int row = 0;
		for (; row < BLOCKS_COUNT_ROWS; ++row)
		{
			for (int col = 0; col < BLOCKS_COUNT_IN_ROW; ++col) 
			{
				blocks.emplace_back(std::make_shared<SmoothDestroyableBlock>(sf::Vector2f({ BLOCK_SHIFT + BLOCK_WIDTH / 2.f + col * (BLOCK_WIDTH + BLOCK_SHIFT), 100.f + row * BLOCK_HEIGHT })));
			}
		}
		
		
		for (int col = 0; col < 15; ++col) 
		{
			blocks.emplace_back(std::make_shared<ThreeHitBlock>(sf::Vector2f({ BLOCK_SHIFT + BLOCK_WIDTH / 2.f + col * (BLOCK_WIDTH + BLOCK_SHIFT), 100.f + row * BLOCK_HEIGHT })));
		}
		
	}

	

	void GameStatePlayingData::GetBallInverse(const sf::Vector2f& ballPos, const sf::FloatRect& blockRect, bool& needInverseDirX, bool& needInverseDirY) {

		if (ballPos.y > blockRect.top + blockRect.height)
		{
			needInverseDirY = true;
		}
		if (ballPos.x < blockRect.left)
		{
			needInverseDirX = true;
		}
		if (ballPos.x > blockRect.left + blockRect.width)
		{
			needInverseDirX = true;
		}
	}
}
