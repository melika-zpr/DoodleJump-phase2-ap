#ifndef AUDIOMANAGER_HPP
#define AUDIOMANAGER_HPP

#include <SFML/Audio.hpp>
#include "ResourceManager.hpp"

class AudioManager {
private:
    ResourceManager<sf::SoundBuffer> soundManager;
    sf::Music backgroundMusic;
    
    sf::Sound jumpSound;
    sf::Sound shootSound;
    sf::Sound gameOverSound;

    // سازنده خصوصی (Private) برای الگوی Singleton
    AudioManager(); 

public:
    // جلوگیری از کپی شدن کلاس
    AudioManager(const AudioManager&) = delete;
    AudioManager& operator=(const AudioManager&) = delete;

    // دسترسی سراسری به تنها نمونه موجود از این کلاس
    static AudioManager& getInstance();

    void init(); 
    void playJump();
    void playShoot();
    void playGameOver();
    void playMusic();
    void stopMusic();
    void setVolume(float volume);
    bool isMusicPlaying() const;
};

#endif