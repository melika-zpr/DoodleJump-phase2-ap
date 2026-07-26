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
        textureManager.load("button_settings", "assets/Settings_button.png");
        isDraggingSlider = false;
        textureManager.load("button_back", "assets/back_button.png");

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




    // --- تنظیم چینش دکمه‌های صفحه منو ---
    startButton.setPosition((window.getSize().x - startButton.getSize().x) / 2.f, 250.f);
    
    // دکمه تنظیمات در منوی اصلی (قرارگیری مستقیم زیر دکمه استارت)
    sf::Texture &settingsTex = textureManager.get("button_settings");
    settingsMenuButton.setSize(sf::Vector2f(static_cast<float>(settingsTex.getSize().x), static_cast<float>(settingsTex.getSize().y)));
    settingsMenuButton.setTexture(&settingsTex);
    settingsMenuButton.setPosition((window.getSize().x - settingsMenuButton.getSize().x) / 2.f, 340.f);

    restartButton.setPosition((window.getSize().x - restartButton.getSize().x) / 2.f, 420.f);
    menuButton.setPosition((window.getSize().x - menuButton.getSize().x) / 2.f, 500.f);

    // --- تنظیم موقعیت متن‌های صفحه منو ---
    titleText.setPosition(window.getSize().x / 2.f, 100.f);
    menuHighScoreText.setPosition(window.getSize().x / 2.f, 170.f);
    
    // دریافت درجه سختی ذخیره شده برای نمایش اولیه صحیح
    std::string diffStr;
    switch (gameSettings.getDifficulty()) {
        case Difficulty::Easy: diffStr = "EASY"; break;
        case Difficulty::Medium: diffStr = "MEDIUM"; break;
        case Difficulty::Hard: diffStr = "HARD"; break;
    }

    // راه‌اندازی و موقعیت‌دهی متن Mode با توجه به تنظیمات ذخیره شده
    setupText(modeText, "Mode: " + diffStr, 16, sf::Color(17, 52, 84));
    modeText.setPosition(window.getSize().x / 2.f, 430.f);

    // تغییر و موقعیت‌دهی متن دستورالعمل‌ها (ادغام شده)
    instructionText.setString("Use Left / Right arrows to move\nHold Space to shoot the monsters");
    sf::FloatRect instBounds = instructionText.getLocalBounds();
    instructionText.setOrigin(instBounds.width / 2.f, instBounds.height / 2.f);
    instructionText.setPosition(window.getSize().x / 2.f, 500.f);

    // --- موقعیت متن‌های بازی و باخت (بدون تغییر) ---
    gameOverText.setPosition(window.getSize().x / 2.f, 180.f);
    scoreText.setOrigin(0.f, 0.f);
    scoreText.setPosition(20.f, 20.f);
    highScoreText.setOrigin(0.f, 0.f);
    highScoreText.setPosition(20.f, 50.f);



    // رنگ‌های اصلی رابط کاربری
    sf::Color darkBlue(17, 52, 84);
    sf::Color lightBlue(220, 230, 235);

    // متن‌های ثابت صفحه تنظیمات
    setupText(settingsTitleText, "SETTINGS", 36, darkBlue);
    settingsTitleText.setPosition(window.getSize().x / 2.f, 100.f);

    setupText(volumeLabelText, "Volume", 20, darkBlue);
    volumeLabelText.setPosition(window.getSize().x / 2.f, 180.f);

    setupText(difficultyLabelText, "Difficulty", 20, darkBlue);
    difficultyLabelText.setPosition(window.getSize().x / 2.f, 320.f);

    setupText(volumeValueText, "0%", 16, darkBlue);

    // ساخت نوار صدا (Slider)
    sliderTrack.setSize(sf::Vector2f(300.f, 6.f));
    sliderTrack.setFillColor(sf::Color(180, 180, 180));
    sliderTrack.setOrigin(0.f, 3.f);
    sliderTrack.setPosition(100.f, 240.f); // بازه حرکت از x=100 تا x=400

    sliderHandle.setRadius(10.f);
    sliderHandle.setFillColor(darkBlue);
    sliderHandle.setOrigin(10.f, 10.f);
    sliderHandle.setPosition(100.f, 240.f); // موقعیت y ثابت، x متغیر است

    // تنظیمات مشترک دکمه‌های سطح سختی
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

   // ساخت دکمه برگشت با استفاده از تصویر تکسچر و سایز ثابت
    sf::Texture &backTex = textureManager.get("button_back");
    backButton.setTexture(&backTex);
    // استفاده از سایز ثابت برای جلوگیری از اشغال شدن نصف صفحه!
    backButton.setSize(sf::Vector2f(200.f, 65.f)); 
    backButton.setFillColor(sf::Color::White);
    backButton.setOrigin(backButton.getSize().x / 2.f, backButton.getSize().y / 2.f);
    backButton.setPosition(window.getSize().x / 2.f, 650.f);

    // اعمال مقادیر اولیه (از تنظیمات خوانده شده) روی رابط کاربری
    updateSettingsUi();
}

void Game::updateSettingsUi()
{
    sf::Color darkBlue(17, 52, 84);
    sf::Color lightBlue(220, 230, 235);

    // ریست کردن رنگ همه دکمه‌ها
    easyButton.setFillColor(lightBlue);
    mediumButton.setFillColor(lightBlue);
    hardButton.setFillColor(lightBlue);
    easyText.setFillColor(darkBlue);
    mediumText.setFillColor(darkBlue);
    hardText.setFillColor(darkBlue);

    // اعمال رنگ تیره به دکمه انتخاب شده
    Difficulty currentDiff = gameSettings.getDifficulty(); // این متد را در مرحله قبل ساختید
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

    // تنظیم جایگاه دستگیره نوار صدا
    int vol = gameSettings.getVolume();
    float newX = 100.f + (vol / 100.f) * 300.f;
    sliderHandle.setPosition(newX, sliderHandle.getPosition().y);

    // آپدیت متن درصد
    volumeValueText.setString(std::to_string(vol) + "%");
    sf::FloatRect bounds = volumeValueText.getLocalBounds();
    volumeValueText.setOrigin(bounds.width / 2.f, bounds.height / 2.f);
    volumeValueText.setPosition(window.getSize().x / 2.f, 270.f);
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
            sf::Vector2i mousePos = sf::Mouse::getPosition(window);
            sf::Vector2f mousePosF(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y));

            if (gameState == GameState::Settings)
            {
                // چک کردن کلیک روی نوار صدا یا دستگیره
                if (sliderHandle.getGlobalBounds().contains(mousePosF) || sliderTrack.getGlobalBounds().contains(mousePosF))
                {
                    isDraggingSlider = true;
                }
                // کلیک روی دکمه‌های سختی
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
                // کلیک روی دکمه برگشت
                else if (backButton.getGlobalBounds().contains(mousePosF))
                {
                    gameSettings.save(); // ذخیره تنظیمات روی دیسک
                    loadHighScore();     // آپدیت نمایش رکورد بر اساس سختی جدید
                    updateOverlayTexts();
                    gameState = GameState::Menu;
                }
            }
            else // اگر در منو یا بازی هستیم
            {
                handleButtonClick(mousePos);

                // کلیک روی دکمه تنظیمات در منوی اصلی
                if (gameState == GameState::Menu && settingsMenuButton.getGlobalBounds().contains(mousePosF))
                {
                    gameState = GameState::Settings;
                    updateSettingsUi();
                }
            }
        }

        // --- پایان کشیدن موس (رها کردن کلیک) ---
        if (event.type == sf::Event::MouseButtonReleased && event.mouseButton.button == sf::Mouse::Left)
        {
            if (isDraggingSlider)
            {
                isDraggingSlider = false;
                gameSettings.save(); // ذخیره ولوم جدید بعد از رها کردن کلیک
            }
        }

        // --- حرکت موس برای نوار صدا ---
        if (event.type == sf::Event::MouseMoved && isDraggingSlider && gameState == GameState::Settings)
        {
            float mouseX = static_cast<float>(event.mouseMove.x);
            // محدود کردن حرکت دستگیره روی خط (از 100 تا 400)
            mouseX = std::max(100.f, std::min(mouseX, 400.f));
            sliderHandle.setPosition(mouseX, sliderHandle.getPosition().y);

            // محاسبه ولوم (0 تا 100) و ذخیره آن
            int newVolume = static_cast<int>(((mouseX - 100.f) / 300.f) * 100.f);
            gameSettings.setVolume(newVolume);

            // آپدیت متن درصد زیر نوار
            volumeValueText.setString(std::to_string(newVolume) + "%");
            sf::FloatRect bounds = volumeValueText.getLocalBounds();
            volumeValueText.setOrigin(bounds.width / 2.f, bounds.height / 2.f);
            // در فاز بعدی که سیستم صوتی را اضافه کردیم، اینجا باید متد تغییر ولوم صدا را هم صدا بزنید
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
            highScores[static_cast<int>(gameSettings.getDifficulty())] = highScore;
            saveHighScore(); // Persist new record immediately.
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
    // پیش‌فرض: صفر کردن همه امتیازها
    for (int i = 0; i < 3; ++i)
        highScores[i] = 0;
    highScore = 0;

    std::ifstream file(highScoreFilename);
    if (file.is_open())
    {
        // خواندن سه عدد (آسان، متوسط، سخت)
        file >> highScores[0] >> highScores[1] >> highScores[2];
        file.close();
    }
    // تنظیم highScore فعلی بر اساس درجه سختی انتخاب شده در تنظیمات
    highScore = highScores[static_cast<int>(gameSettings.getDifficulty())];
}

void Game::saveHighScore() const
{
    std::ofstream file(highScoreFilename);
    if (file.is_open())
    {
        // ذخیره سه عدد با فاصله
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
        window.draw(subtitleText);
        window.draw(startButton);
        window.draw(instructionText);
        window.draw(settingsMenuButton); // رسم دکمه چرخ‌دنده در منو
    }
    else if (gameState == GameState::Settings)
    {
        // رسم صفحه تنظیمات
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

    // رسم بازیکن و پلتفرم‌ها فقط در صفحه بازی یا باخت انجام شود
    if (gameState == GameState::Playing || gameState == GameState::GameOver)
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
