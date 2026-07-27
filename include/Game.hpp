#ifndef GAME_HPP
#define GAME_HPP

#include <SFML/Graphics.hpp>
#include <memory>
#include <string>
#include <SFML/Audio.hpp>
#include "ResourceManager.hpp"
#include "Player.hpp"
#include "WorldManager.hpp"
#include "Settings.hpp"
#include "AudioManager.hpp"

class Game
{
private:
    enum class GameState
    {
        Menu,
        Playing,
        GameOver,
        Settings,
        HoleSuction
    };

    Settings gameSettings; 
    int highScores[3];     

    sf::Text settingsTitleText;
    sf::Text volumeLabelText;
    sf::Text difficultyLabelText;
    sf::Text volumeValueText;

    sf::RectangleShape sliderTrack; 
    sf::CircleShape sliderHandle;   
    bool isDraggingSlider;          

    sf::RectangleShape easyButton;
    sf::RectangleShape mediumButton;
    sf::RectangleShape hardButton;
    sf::Text easyText;
    sf::Text mediumText;
    sf::Text hardText;

    sf::Text modeText;

    sf::RectangleShape backButton;
    
    void updateSettingsUi();

    sf::RectangleShape settingsMenuButton;
    sf::Text settingsMenuText;

    sf::RenderWindow window;
    sf::Clock clock;

    ResourceManager<sf::Texture> textureManager;

    std::unique_ptr<Player> player;
    std::unique_ptr<WorldManager> worldManager;

    sf::Sprite backgroundSprite;
    sf::Font font;
    sf::Text titleText;
    sf::Text subtitleText;
    sf::Text actionText;
    sf::Text restartText;
    sf::Text menuText;
    sf::Text scoreText;
    sf::Text highScoreText;
    sf::Text menuHighScoreText;
    sf::Text instructionText;
    sf::Text gameOverText;
    sf::RectangleShape gameOverOverlay;
    
    sf::RectangleShape startButton;   // Texture button for starting the game
    sf::RectangleShape restartButton; // Texture button for restarting after game over
    sf::RectangleShape menuButton;    // Texture button for returning to the main menu

    int score;                                                        // Current score in the active game session
    int highScore;                                                    // Stored best score across launches
    static constexpr const char *highScoreFilename = "highscore.txt"; // File where high score is persisted

    GameState gameState; // Current screen state: Menu, Playing, HoleSuction, or GameOver

    sf::Vector2f holeCenterForSuction; // ذخیره مختصات مرکز سیاه‌چاله برای انیمیشن مکش

    void updateAudioVolume(); // اعمال ولوم تنظیمات روی صداها

    void processEvents();         // Read window events and player input
    void update(float deltaTime); // Advance game state and handle scrolling
    void render();                // Draw the current frame

    void setupUi();                                            // Load fonts, configure text, and prepare buttons
    void updateOverlayTexts();                                 // Refresh HUD text strings after score changes
    void startGame();                                          // Switch into Playing state and reset game objects
    void resetGame();                                          // Reset player, world, and score for a new game
    void handleButtonClick(const sf::Vector2i &mousePosition); // Manage menu button clicks
    void drawOverlay();                                        // Draw menu/game UI on top of the game world
    void loadHighScore();                                      // Load saved high score from disk
    void saveHighScore() const;                                // Save current high score to disk

public:
    Game();
    void run();
};

#endif