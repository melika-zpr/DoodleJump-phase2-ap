#include "Game.hpp"
#include <fstream>
#include <iostream>
#include <vector>

Game::Game()
    : window(sf::VideoMode(500, 800), "Doodle Jump - Phase 2", sf::Style::Titlebar | sf::Style::Close),
      gameState(GameState::Menu),
      shootCooldown(0.f)
{
    window.setFramerateLimit(60);

    try
    {
        // Load the stored high score first so the menu displays the correct value.
        loadHighScore();

        textureManager.load("background", "assets/background.png");
        backgroundSprite.setTexture(textureManager.get("background"));

        textureManager.load("player_left", "assets/left_doodle.png");
        textureManager.load("player_right", "assets/right_doodle.png");
        textureManager.load("player_shoot_body", "assets/Shooting@Pose.png");
        textureManager.load("snout", "assets/Nose.png");
        textureManager.load("platform", "assets/normal_platform.png");
        textureManager.load("platform_broken", "assets/broken_platform.png");
        textureManager.load("platform_moving", "assets/moving_platform.png");
        textureManager.load("spring", "assets/spring_sprite.png");
        textureManager.load("button_start", "assets/start_button.png");
        textureManager.load("button_restart", "assets/restart_button.png");
        textureManager.load("button_menu", "assets/menu_button.png");
        textureManager.load("button_settings", "assets/Settings_button.png");
        textureManager.load("monster1", "assets/green_monster.png");
        textureManager.load("monster2", "assets/BlueMonster.png");
        textureManager.load("hole", "assets/hole.png");
        textureManager.load("hole_large", "assets/hole@2x.png");
        
        isDraggingSlider = false;
        textureManager.load("button_back", "assets/back_button.png");

        AudioManager::getInstance().init();
        updateAudioVolume();
        AudioManager::getInstance().playMusic();

        setupUi();
        resetGame();
    }
    catch (const std::exception &e)
    {
        std::cerr << "Initialization Error: " << e.what() << std::endl;
    }
}

void Game::run()
{
    while (window.isOpen())
    {
        float deltaTime = clock.restart().asSeconds();
        processEvents();
        update(deltaTime);
        render();
    }
}

void Game::setupUi()
{
    const std::vector<std::string> fontPaths = {
        "fonts/ariblk.ttf",
        "/System/Library/Fonts/Supplemental/Arial Unicode.ttf",
        "/Library/Fonts/Arial Unicode.ttf",
        "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf",
        "/usr/share/fonts/truetype/liberation2/LiberationSans-Regular.ttf"};

    for (const auto &path : fontPaths)
    {
        if (font.loadFromFile(path))
        {
            break;
        }
    }

    auto setupText = [this](sf::Text &text, const std::string &value, unsigned int size, sf::Color color)
    {
        text.setFont(font);
        text.setString(value);
        text.setCharacterSize(size);
        text.setFillColor(color);
        text.setStyle(sf::Text::Bold);
        sf::FloatRect bounds = text.getLocalBounds();
        text.setOrigin(bounds.width / 2.f, bounds.height / 2.f);
    };

    setupText(titleText, "DOODLE JUMP", 48, sf::Color(17, 52, 84));
    setupText(subtitleText, "Press Enter, Space or click Start", 20, sf::Color(80, 80, 80));
    setupText(menuHighScoreText, "HIGH SCORE: 0", 24, sf::Color(17, 52, 84));
    setupText(instructionText, "Use Left / Right arrows to move", 18, sf::Color(80, 80, 80));
    setupText(scoreText, "SCORE: 0", 20, sf::Color(17, 52, 84));
    setupText(highScoreText, "HIGH SCORE: 0", 20, sf::Color(17, 52, 84));
    setupText(gameOverText, "YOU LOST", 42, sf::Color(180, 40, 40));

    {
        sf::Texture &startTexture = textureManager.get("button_start");
        startButton.setSize(sf::Vector2f(static_cast<float>(startTexture.getSize().x), static_cast<float>(startTexture.getSize().y)));
        startButton.setTexture(&startTexture);
        startButton.setFillColor(sf::Color::White);
    }

    {
        sf::Texture &restartTexture = textureManager.get("button_restart");
        restartButton.setSize(sf::Vector2f(static_cast<float>(restartTexture.getSize().x), static_cast<float>(restartTexture.getSize().y)));
        restartButton.setTexture(&restartTexture);
        restartButton.setFillColor(sf::Color::White);
    }

    {
        sf::Texture &menuTexture = textureManager.get("button_menu");
        menuButton.setSize(sf::Vector2f(static_cast<float>(menuTexture.getSize().x), static_cast<float>(menuTexture.getSize().y)));
        menuButton.setTexture(&menuTexture);
        menuButton.setFillColor(sf::Color::White);
    }

    startButton.setSize(sf::Vector2f(220.f, 70.f));
    startButton.setPosition((window.getSize().x - startButton.getSize().x) / 2.f, 250.f);

    sf::Texture &settingsTex = textureManager.get("button_settings");
    settingsMenuButton.setTexture(&settingsTex);
    settingsMenuButton.setSize(sf::Vector2f(220.f, 70.f));
    settingsMenuButton.setPosition((window.getSize().x - settingsMenuButton.getSize().x) / 2.f, 340.f);

    sf::Texture &restartTexture = textureManager.get("button_restart");
    restartButton.setTexture(&restartTexture);
    restartButton.setSize(sf::Vector2f(220.f, 70.f));
    restartButton.setPosition((window.getSize().x - restartButton.getSize().x) / 2.f, 420.f);

    sf::Texture &menuTexture = textureManager.get("button_menu");
    menuButton.setTexture(&menuTexture);
    menuButton.setSize(sf::Vector2f(220.f, 70.f));
    menuButton.setPosition((window.getSize().x - menuButton.getSize().x) / 2.f, 500.f);

    titleText.setPosition(window.getSize().x / 2.f, 100.f);
    menuHighScoreText.setPosition(window.getSize().x / 2.f, 170.f);

    std::string diffStr;
    switch (gameSettings.getDifficulty())
    {
    case Difficulty::Easy: diffStr = "EASY"; break;
    case Difficulty::Medium: diffStr = "MEDIUM"; break;
    case Difficulty::Hard: diffStr = "HARD"; break;
    }

    setupText(modeText, "Mode: " + diffStr, 16, sf::Color(17, 52, 84));
    modeText.setPosition(window.getSize().x / 2.f, 430.f);

    instructionText.setString("Use Left / Right arrows to move\nHold Space to shoot the monsters");
    sf::FloatRect instBounds = instructionText.getLocalBounds();
    instructionText.setOrigin(instBounds.width / 2.f, instBounds.height / 2.f);
    instructionText.setPosition(window.getSize().x / 2.f, 500.f);

    gameOverText.setPosition(window.getSize().x / 2.f, 180.f);
    scoreText.setOrigin(0.f, 0.f);
    scoreText.setPosition(20.f, 20.f);
    highScoreText.setOrigin(0.f, 0.f);
    highScoreText.setPosition(20.f, 50.f);

    sf::Color darkBlue(17, 52, 84);
    sf::Color lightBlue(220, 230, 235);

    setupText(settingsTitleText, "SETTINGS", 36, darkBlue);
    settingsTitleText.setPosition(window.getSize().x / 2.f, 100.f);

    setupText(volumeLabelText, "Volume", 20, darkBlue);
    volumeLabelText.setPosition(window.getSize().x / 2.f, 180.f);

    setupText(difficultyLabelText, "Difficulty", 20, darkBlue);
    difficultyLabelText.setPosition(window.getSize().x / 2.f, 320.f);

    setupText(volumeValueText, "0%", 16, darkBlue);

    sliderTrack.setSize(sf::Vector2f(300.f, 6.f));
    sliderTrack.setFillColor(sf::Color(180, 180, 180));
    sliderTrack.setOrigin(0.f, 3.f);
    sliderTrack.setPosition(100.f, 240.f);

    sliderHandle.setRadius(10.f);
    sliderHandle.setFillColor(darkBlue);
    sliderHandle.setOrigin(10.f, 10.f);
    sliderHandle.setPosition(100.f, 240.f);

    auto setupDifficultyButton = [&](sf::RectangleShape &btn, sf::Text &txt, const std::string &str, float posX)
    {
        btn.setSize(sf::Vector2f(90.f, 40.f));
        btn.setOutlineThickness(2.f);
        btn.setOutlineColor(darkBlue);
        btn.setOrigin(45.f, 20.f);
        btn.setPosition(posX, 400.f);

        txt.setFont(font);
        txt.setString(str);
        txt.setCharacterSize(16);
        txt.setStyle(sf::Text::Bold);
        sf::FloatRect bounds = txt.getLocalBounds();
        txt.setOrigin(bounds.width / 2.f, bounds.height / 2.f);
        txt.setPosition(posX, 400.f);
    };

    setupDifficultyButton(easyButton, easyText, "EASY", 120.f);
    setupDifficultyButton(mediumButton, mediumText, "MEDIUM", 250.f);
    setupDifficultyButton(hardButton, hardText, "HARD", 380.f);

    sf::Texture &backTex = textureManager.get("button_back");
    backButton.setTexture(&backTex);
    backButton.setSize(sf::Vector2f(200.f, 65.f));
    backButton.setFillColor(sf::Color::White);
    backButton.setOrigin(backButton.getSize().x / 2.f, backButton.getSize().y / 2.f);
    backButton.setPosition(window.getSize().x / 2.f, 650.f);

    gameOverOverlay.setSize(sf::Vector2f(window.getSize().x, window.getSize().y));
    gameOverOverlay.setFillColor(sf::Color(255, 255, 255, 210));

    updateSettingsUi();
}

void Game::updateSettingsUi()
{
    sf::Color darkBlue(17, 52, 84);
    sf::Color lightBlue(220, 230, 235);

    easyButton.setFillColor(lightBlue);
    mediumButton.setFillColor(lightBlue);
    hardButton.setFillColor(lightBlue);
    easyText.setFillColor(darkBlue);
    mediumText.setFillColor(darkBlue);
    hardText.setFillColor(darkBlue);

    Difficulty currentDiff = gameSettings.getDifficulty();
    if (currentDiff == Difficulty::Easy)
    {
        easyButton.setFillColor(darkBlue);
        easyText.setFillColor(sf::Color::White);
    }
    else if (currentDiff == Difficulty::Medium)
    {
        mediumButton.setFillColor(darkBlue);
        mediumText.setFillColor(sf::Color::White);
    }
    else if (currentDiff == Difficulty::Hard)
    {
        hardButton.setFillColor(darkBlue);
        hardText.setFillColor(sf::Color::White);
    }

    int vol = gameSettings.getVolume();
    float newX = 100.f + (vol / 100.f) * 300.f;
    sliderHandle.setPosition(newX, sliderHandle.getPosition().y);

    volumeValueText.setString(std::to_string(vol) + "%");
    sf::FloatRect bounds = volumeValueText.getLocalBounds();
    volumeValueText.setOrigin(bounds.width / 2.f, bounds.height / 2.f);
    volumeValueText.setPosition(window.getSize().x / 2.f, 270.f);
}

void Game::updateOverlayTexts()
{
    std::string diffStr;
    switch (gameSettings.getDifficulty())
    {
    case Difficulty::Easy: diffStr = "EASY"; break;
    case Difficulty::Medium: diffStr = "MEDIUM"; break;
    case Difficulty::Hard: diffStr = "HARD"; break;
    }

    scoreText.setString("SCORE: " + std::to_string(score));
    highScoreText.setString("HIGH SCORE (" + diffStr + "): " + std::to_string(highScore));
    menuHighScoreText.setString("HIGH SCORE (" + diffStr + "): " + std::to_string(highScore));

    sf::FloatRect menuHighBounds = menuHighScoreText.getLocalBounds();
    menuHighScoreText.setOrigin(menuHighBounds.width / 2.f, menuHighBounds.height / 2.f);
    menuHighScoreText.setPosition(window.getSize().x / 2.f, 230.f);
}

void Game::startGame()
{
    gameState = GameState::Playing;
    AudioManager::getInstance().stopMusic();
    resetGame();
}

void Game::resetGame()
{
    score = 0;
    shootCooldown = 0.f;
    sf::Texture &texLeft = textureManager.get("player_left");
    sf::Texture &texRight = textureManager.get("player_right");
    sf::Texture &texShootBody = textureManager.get("player_shoot_body");
    sf::Texture &texSnout = textureManager.get("snout");

    player = std::make_unique<Player>(texLeft, texRight, texShootBody, texSnout);
    worldManager = std::make_unique<WorldManager>(textureManager, gameSettings.getDifficulty());

    scoreText.setOrigin(0.f, 0.f);
    scoreText.setPosition(20.f, 20.f);
    highScoreText.setOrigin(0.f, 0.f);
    highScoreText.setPosition(20.f, 50.f);

    updateOverlayTexts();
}

void Game::handleButtonClick(const sf::Vector2i &mousePosition)
{
    const sf::Vector2f clickPos(static_cast<float>(mousePosition.x), static_cast<float>(mousePosition.y));
    if (gameState == GameState::Menu && startButton.getGlobalBounds().contains(clickPos))
    {
        startGame();
    }
    else if (gameState == GameState::GameOver)
    {
        if (restartButton.getGlobalBounds().contains(clickPos))
        {
            startGame();
        }
        else if (menuButton.getGlobalBounds().contains(clickPos))
        {
            AudioManager::getInstance().playMusic();
            gameState = GameState::Menu;
        }
    }
}

void Game::processEvents()
{
    sf::Event event;
    while (window.pollEvent(event))
    {
        if (event.type == sf::Event::Closed)
        {
            window.close();
        }

        if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left)
        {
            handleButtonClick(sf::Mouse::getPosition(window));
            sf::Vector2i mousePos = sf::Mouse::getPosition(window);
            sf::Vector2f mousePosF(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y));

            if (gameState == GameState::Settings)
            {
                if (sliderHandle.getGlobalBounds().contains(mousePosF) || sliderTrack.getGlobalBounds().contains(mousePosF))
                {
                    isDraggingSlider = true;
                }
                else if (easyButton.getGlobalBounds().contains(mousePosF))
                {
                    gameSettings.setDifficulty(Difficulty::Easy);
                    updateSettingsUi();
                }
                else if (mediumButton.getGlobalBounds().contains(mousePosF))
                {
                    gameSettings.setDifficulty(Difficulty::Medium);
                    updateSettingsUi();
                }
                else if (hardButton.getGlobalBounds().contains(mousePosF))
                {
                    gameSettings.setDifficulty(Difficulty::Hard);
                    updateSettingsUi();
                }
                else if (backButton.getGlobalBounds().contains(mousePosF))
                {
                    gameSettings.save();
                    loadHighScore();
                    updateOverlayTexts();
                    gameState = GameState::Menu;
                }
            }
            else
            {
                handleButtonClick(mousePos);

                if (gameState == GameState::Menu && settingsMenuButton.getGlobalBounds().contains(mousePosF))
                {
                    gameState = GameState::Settings;
                    updateSettingsUi();
                }
            }
        }

        if (event.type == sf::Event::MouseButtonReleased && event.mouseButton.button == sf::Mouse::Left)
        {
            if (isDraggingSlider)
            {
                isDraggingSlider = false;
                gameSettings.save();
            }
        }

        if (event.type == sf::Event::MouseMoved && isDraggingSlider && gameState == GameState::Settings)
        {
            float mouseX = static_cast<float>(event.mouseMove.x);
            mouseX = std::max(100.f, std::min(mouseX, 400.f));
            sliderHandle.setPosition(mouseX, sliderHandle.getPosition().y);

            int newVolume = static_cast<int>(((mouseX - 100.f) / 300.f) * 100.f);
            gameSettings.setVolume(newVolume);

            volumeValueText.setString(std::to_string(newVolume) + "%");
            sf::FloatRect bounds = volumeValueText.getLocalBounds();
            volumeValueText.setOrigin(bounds.width / 2.f, bounds.height / 2.f);

            updateAudioVolume();
        }

        if (event.type == sf::Event::KeyPressed)
        {
            if ((event.key.code == sf::Keyboard::Enter || event.key.code == sf::Keyboard::Space) && gameState != GameState::Playing && gameState != GameState::HoleSuction)
            {
                AudioManager::getInstance().stopMusic();
                startGame();
            }
            else if (event.key.code == sf::Keyboard::R && gameState == GameState::GameOver)
            {
                startGame();
            }
            else if (event.key.code == sf::Keyboard::Escape && gameState == GameState::GameOver)
            {
                AudioManager::getInstance().playMusic();
                gameState = GameState::Menu;
            }
        }
    }

    if (gameState == GameState::Playing && player)
    {
        player->handleInput();
    }
}

void Game::update(float deltaTime)
{
    // مدیریت حالت مکش توسط سیاه‌چاله
    if (gameState == GameState::HoleSuction && player)
    {
        bool suctionFinished = false;
        player->updateSuction(holeCenterForSuction, deltaTime, suctionFinished);
        if (suctionFinished)
        {
            gameState = GameState::GameOver;
            AudioManager::getInstance().playGameOver();

            sf::FloatRect scoreBounds = scoreText.getLocalBounds();
            scoreText.setOrigin(scoreBounds.width / 2.f, scoreBounds.height / 2.f);
            scoreText.setPosition(window.getSize().x / 2.f, 250.f);

            sf::FloatRect hsBounds = highScoreText.getLocalBounds();
            highScoreText.setOrigin(hsBounds.width / 2.f, hsBounds.height / 2.f);
            highScoreText.setPosition(window.getSize().x / 2.f, 290.f);
        }
        return;
    }

    if (gameState != GameState::Playing || !player || !worldManager)
    {
        return;
    }

    // بررسی شلیک پیوسته با نگه‌داشتن کلید Space و نرخ آتش وابسته به دشواری
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space))
    {
        player->triggerShoot();

        shootCooldown -= deltaTime;
        if (shootCooldown <= 0.f)
        {
            sf::Vector2f shootPos = player->getPosition();
            shootPos.x += player->getBounds().width / 2.f; 
            shootPos.y -= 10.f; 
            worldManager->spawnBullet(shootPos);

            // نرخ آتش: در حالت آسان سریع‌تر و در متوسط/سخت کندتر
            if (gameSettings.getDifficulty() == Difficulty::Easy)
            {
                shootCooldown = 0.25f; 
            }
            else
            {
                shootCooldown = 0.5f;  
            }
        }
    }
    else
    {
        shootCooldown = 0.f;
    }

    player->update(deltaTime, 500.f);

    // بررسی برخورد با سیاه‌چاله
    if (worldManager->checkHoleCollision(*player, holeCenterForSuction))
    {
        gameState = GameState::HoleSuction;
        return;
    }

    float scrollAmount = worldManager->update(*player, deltaTime);
    if (worldManager->isGameOver() && gameState != GameState::GameOver)
    {
            gameState = GameState::GameOver;
            AudioManager::getInstance().playGameOver();
            
            sf::FloatRect scoreBounds = scoreText.getLocalBounds();
            scoreText.setOrigin(scoreBounds.width / 2.f, scoreBounds.height / 2.f);
            scoreText.setPosition(window.getSize().x / 2.f, 250.f);

            sf::FloatRect hsBounds = highScoreText.getLocalBounds();
            highScoreText.setOrigin(hsBounds.width / 2.f, hsBounds.height / 2.f);
            highScoreText.setPosition(window.getSize().x / 2.f, 290.f);
            return;
    }

    // --- اضافه شدن این بخش: بررسی باخت فوری بر اثر برخورد با مانستر ---
    if (worldManager->isGameOver()) 
    {
        if (gameState != GameState::GameOver)
        { 
            gameState = GameState::GameOver;
            
            // تنظیم جایگاه متن‌های امتیاز برای وسط صفحه در زمان باخت
            sf::FloatRect scoreBounds = scoreText.getLocalBounds();
            scoreText.setOrigin(scoreBounds.width / 2.f, scoreBounds.height / 2.f);
            scoreText.setPosition(window.getSize().x / 2.f, 250.f);

            sf::FloatRect hsBounds = highScoreText.getLocalBounds();
            highScoreText.setOrigin(hsBounds.width / 2.f, hsBounds.height / 2.f);
            highScoreText.setPosition(window.getSize().x / 2.f, 290.f);
        }
        return; // توقف ادامه آپدیت برای جلوگیری از باگ‌های احتمالی
    }

    // Increase score only when the world scrolls upward.
    if (scrollAmount > 0.f)
    {
        score += static_cast<int>(scrollAmount);
        if (score > highScore)
        {
            highScore = score;
            highScores[static_cast<int>(gameSettings.getDifficulty())] = highScore;
            saveHighScore();
        }
        updateOverlayTexts();
    }

    if (player->getPosition().y > 900.f)
    {
        if (gameState != GameState::GameOver)
        { 
            gameState = GameState::GameOver;
            AudioManager::getInstance().playGameOver();
            
            sf::FloatRect scoreBounds = scoreText.getLocalBounds();
            scoreText.setOrigin(scoreBounds.width / 2.f, scoreBounds.height / 2.f);
            scoreText.setPosition(window.getSize().x / 2.f, 250.f);

            sf::FloatRect hsBounds = highScoreText.getLocalBounds();
            highScoreText.setOrigin(hsBounds.width / 2.f, hsBounds.height / 2.f);
            highScoreText.setPosition(window.getSize().x / 2.f, 290.f);
        }
    } 
}

void Game::loadHighScore()
{
    for (int i = 0; i < 3; ++i)
        highScores[i] = 0;
    highScore = 0;

    std::ifstream file(highScoreFilename);
    if (file.is_open())
    {
        file >> highScores[0] >> highScores[1] >> highScores[2];
        file.close();
    }
    highScore = highScores[static_cast<int>(gameSettings.getDifficulty())];
}

void Game::saveHighScore() const
{
    std::ofstream file(highScoreFilename);
    if (file.is_open())
    {
        file << highScores[0] << " " << highScores[1] << " " << highScores[2] << "\n";
        file.close();
    }
}

void Game::drawOverlay()
{
    if (gameState == GameState::Menu)
    {
        window.draw(titleText);
        window.draw(menuHighScoreText);
        window.draw(startButton);
        window.draw(instructionText);
        window.draw(settingsMenuButton); 
    }
    else if (gameState == GameState::Settings)
    {
        window.draw(settingsTitleText);
        window.draw(volumeLabelText);
        window.draw(sliderTrack);
        window.draw(sliderHandle);
        window.draw(volumeValueText);
        window.draw(difficultyLabelText);

        window.draw(easyButton);
        window.draw(easyText);
        window.draw(mediumButton);
        window.draw(mediumText);
        window.draw(hardButton);
        window.draw(hardText);

        window.draw(backButton);
    }
    else if (gameState == GameState::Playing || gameState == GameState::HoleSuction)
    {
        window.draw(scoreText);
        window.draw(highScoreText);
    }
    else if (gameState == GameState::GameOver)
    {
        window.draw(gameOverOverlay); 
        window.draw(gameOverText);
        window.draw(scoreText);
        window.draw(highScoreText);
        window.draw(restartButton);
        window.draw(menuButton);
    }
}

void Game::render()
{
    window.clear();
    window.draw(backgroundSprite);

    if (gameState == GameState::Playing || gameState == GameState::HoleSuction || gameState == GameState::GameOver)
    {
        if (worldManager)
        {
            worldManager->draw(window);
        }
        if (player)
        {
            player->draw(window);
        }
    }

    drawOverlay();
    window.display();
}

void Game::updateAudioVolume()
{
    AudioManager::getInstance().setVolume(static_cast<float>(gameSettings.getVolume()));
}