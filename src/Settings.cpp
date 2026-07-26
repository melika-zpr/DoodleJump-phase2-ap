#include "Settings.hpp"
#include <fstream>
#include <algorithm>

Settings::Settings() : volume(50), difficulty(Difficulty::Medium) {
    load(); // هنگام ساخته شدن شیء، مستقیماً داده‌ها را از فایل می‌خواند
}

void Settings::load() {
    std::ifstream file(filename);
    if (file.is_open()) {
        int diffInt;
        // خواندن ولوم و درجه سختی از فایل
        if (file >> volume >> diffInt) {
            // اطمینان از اینکه مقادیر در بازه مجاز هستند
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
        // ذخیره به فرمت: [Volume] [Difficulty]
        file << volume << " " << static_cast<int>(difficulty) << "\n";
        file.close();
    }
}

int Settings::getVolume() const { return volume; }
void Settings::setVolume(int v) { volume = std::clamp(v, 0, 100); }

Difficulty Settings::getDifficulty() const { return difficulty; }
void Settings::setDifficulty(Difficulty d) { difficulty = d; }