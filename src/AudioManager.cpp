#include "AudioManager.hpp"

AudioManager::AudioManager() {}

AudioManager& AudioManager::getInstance() {
    static AudioManager instance;
    return instance;
}

void AudioManager::init() {
    soundManager.load("jump", "sounds/Jumping_Sound.wav");
    soundManager.load("shoot", "sounds/Shooting_Sound.wav");
    soundManager.load("gameover", "sounds/Loosing_Sound.wav");

    jumpSound.setBuffer(soundManager.get("jump"));
    shootSound.setBuffer(soundManager.get("shoot"));
    gameOverSound.setBuffer(soundManager.get("gameover"));

    if (!backgroundMusic.openFromFile("sounds/MainMenu_Song.flac")) {
        throw std::runtime_error("Failed to load MainMenu_Song.flac");
    }
    backgroundMusic.setLoop(true);
}

void AudioManager::playJump() { jumpSound.play(); }
void AudioManager::playShoot() { shootSound.play(); }
void AudioManager::playGameOver() { gameOverSound.play(); }

void AudioManager::playMusic() {
    if (backgroundMusic.getStatus() != sf::SoundSource::Playing) {
        backgroundMusic.play();
    }
}

void AudioManager::stopMusic() { backgroundMusic.stop(); }

void AudioManager::setVolume(float volume) {
    backgroundMusic.setVolume(volume);
    jumpSound.setVolume(volume);
    shootSound.setVolume(volume);
    gameOverSound.setVolume(volume);
}

bool AudioManager::isMusicPlaying() const {
    return backgroundMusic.getStatus() == sf::SoundSource::Playing;
}