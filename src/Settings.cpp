#include "Settings.hpp"
#include <fstream>
#include <algorithm>

Settings::Settings() : volume(50), difficulty(Difficulty::Medium) {
    load(); 
}

void Settings::load() {
    std::ifstream file(filename);
    if (file.is_open()) {
        int diffInt;
        if (file >> volume >> diffInt) {
            volume = std::clamp(volume, 0, 100);
            if (diffInt >= 0 && diffInt <= 2) {
                difficulty = static_cast<Difficulty>(diffInt);
            }
        }
        file.close();
    }
}

void Settings::save() const {
    std::ofstream file(filename);
    if (file.is_open()) {
        file << volume << " " << static_cast<int>(difficulty) << "\n";
        file.close();
    }
}

int Settings::getVolume() const { return volume; }
void Settings::setVolume(int v) { volume = std::clamp(v, 0, 100); }

Difficulty Settings::getDifficulty() const { return difficulty; }
void Settings::setDifficulty(Difficulty d) { difficulty = d; }