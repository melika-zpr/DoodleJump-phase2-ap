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

    float shootCooldown = 0.f;
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
    
    sf::RectangleShape startButton;  
    sf::RectangleShape restartButton;
    sf::RectangleShape menuButton;    

    int score;                                                        
    int highScore;                                                    
    static constexpr const char *highScoreFilename = "highscore.txt"; 

    GameState gameState; 
    sf::Vector2f holeCenterForSuction; 

    void updateAudioVolume(); 
    void processEvents();         
    void update(float deltaTime); 
    void render();                

    void setupUi();                                            
    void updateOverlayTexts();                                
    void startGame();                                          
    void resetGame();                                         
    void handleButtonClick(const sf::Vector2i &mousePosition); 
    void drawOverlay();                                        
    void loadHighScore();                                      
    void saveHighScore() const;                                

public:
    Game();
    void run();
};

#endif