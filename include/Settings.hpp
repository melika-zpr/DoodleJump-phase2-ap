#ifndef SETTINGS_HPP
#define SETTINGS_HPP

#include <string>

// تعریف سطوح دشواری به صورت یک Enum امن
enum class Difficulty {
    Easy = 0,
    Medium = 1,
    Hard = 2
};

class Settings {
private:
    int volume;
    Difficulty difficulty;
    const std::string filename = "settings.txt";

public:
    Settings();

    void load();
    void save() const;

    int getVolume() const;
    void setVolume(int v);

    Difficulty getDifficulty() const;
    void setDifficulty(Difficulty d);
};

#endif