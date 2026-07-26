#include "Game.hpp"
#include <fstream>
#include <iostream>
#include <vector>

Game::Game()
    : window(sf::VideoMode(500, 800), "Doodle Jump - Phase 1", sf::Style::Titlebar | sf::Style::Close),
      gameState(GameState::Menu)
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
        textureManager.load("platform", "assets/normal_platform.png");
        textureManager.load("platform_broken", "assets/broken_platform.png");
        textureManager.load("platform_moving", "assets/moving_platform.png");
        textureManager.load("spring", "assets/spring_sprite.png");
        textureManager.load("button_start", "assets/start_button.png");
        textureManager.load("button_restart", "assets/restart_button.png");
        textureManager.load("button_menu", "assets/menu_button.png");

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
    // Main game loop: handle events, update state, and render continuously.
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
        "/usr/share/fonts/truetype/liberation2/LiberationSans-Regular.ttf"
    };

    for (const auto &path : fontPaths)
    {
        if (font.loadFromFile(path))
        {
            break;
        }
    }

    auto setupText = [this](sf::Text &text, const std::string &value, unsigned int size, sf::Color color) {
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

    startButton.setPosition((window.getSize().x - startButton.getSize().x) / 2.f, 340.f);
    restartButton.setPosition((window.getSize().x - restartButton.getSize().x) / 2.f, 420.f);
    menuButton.setPosition((window.getSize().x - menuButton.getSize().x) / 2.f, 500.f);

    titleText.setPosition(window.getSize().x / 2.f, 160.f);
    menuHighScoreText.setPosition(window.getSize().x / 2.f, 230.f);
    subtitleText.setPosition(window.getSize().x / 2.f, 280.f);
    instructionText.setPosition(window.getSize().x / 2.f, 520.f);
    gameOverText.setPosition(window.getSize().x / 2.f, 180.f);
    scoreText.setOrigin(0.f, 0.f);
    scoreText.setPosition(20.f, 20.f);
    highScoreText.setOrigin(0.f, 0.f);
    highScoreText.setPosition(20.f, 50.f);
}

void Game::updateOverlayTexts()
{
    scoreText.setString("SCORE: " + std::to_string(score));
    highScoreText.setString("HIGH SCORE: " + std::to_string(highScore));
    menuHighScoreText.setString("HIGH SCORE: " + std::to_string(highScore));

    sf::FloatRect menuHighBounds = menuHighScoreText.getLocalBounds();
    menuHighScoreText.setOrigin(menuHighBounds.width / 2.f, menuHighBounds.height / 2.f);
    menuHighScoreText.setPosition(window.getSize().x / 2.f, 230.f);
}

void Game::startGame()
{
    gameState = GameState::Playing;
    resetGame();
}

void Game::resetGame()
{
    score = 0;
    sf::Texture &texLeft = textureManager.get("player_left");
    sf::Texture &texRight = textureManager.get("player_right");

    player = std::make_unique<Player>(texLeft, texRight);
    worldManager = std::make_unique<WorldManager>(textureManager);
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
        }

        if (event.type == sf::Event::KeyPressed)
        {
            if ((event.key.code == sf::Keyboard::Enter || event.key.code == sf::Keyboard::Space) && gameState != GameState::Playing)
            {
                startGame();
            }
            else if (event.key.code == sf::Keyboard::R && gameState == GameState::GameOver)
            {
                startGame();
            }
            else if (event.key.code == sf::Keyboard::Escape && gameState == GameState::GameOver)
            {
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
    if (gameState != GameState::Playing || !player || !worldManager)
    {
        return;
    }

    // Update player physics and world scrolling.
    player->update(deltaTime, 500.f);
    float scrollAmount = worldManager->update(*player, deltaTime);

    // Increase score only when the world scrolls upward.
    if (scrollAmount > 0.f)
    {
        score += static_cast<int>(scrollAmount);
        if (score > highScore)
        {
            highScore = score;
            saveHighScore();  // Persist new record immediately.
        }
        updateOverlayTexts();
    }

    // End the game if the player falls too far below the screen.
    if (player->getPosition().y > 900.f)
    {
        gameState = GameState::GameOver;
    }
}

void Game::loadHighScore()
{
    highScore = 0;
    std::ifstream file(highScoreFilename);
    if (!file.is_open())
    {
        return;
    }

    int storedScore;
    if (file >> storedScore)
    {
        if (storedScore > 0)
        {
            highScore = storedScore;
        }
    }
}

void Game::saveHighScore() const
{
    std::ofstream file(highScoreFilename);
    if (!file.is_open())
    {
        return;
    }

    file << highScore;
}

void Game::drawOverlay()
{
    if (gameState == GameState::Menu)
    {
        window.draw(titleText);
        window.draw(menuHighScoreText);
        window.draw(subtitleText);
        window.draw(startButton);
        window.draw(instructionText);
    }
    else if (gameState == GameState::Playing)
    {
        window.draw(scoreText);
        window.draw(highScoreText);
    }
    else if (gameState == GameState::GameOver)
    {
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

    if (worldManager)
    {
        worldManager->draw(window);
    }

    if (player)
    {
        player->draw(window);
    }

    drawOverlay();
    window.display();
}
