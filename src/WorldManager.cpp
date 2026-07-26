#include "WorldManager.hpp"
#include <random>
#include <limits>
#include <vector>
#include <algorithm>

WorldManager::WorldManager(ResourceManager<sf::Texture>& texMgr)
    : textureManager(texMgr), gen(std::random_device{}()), lastPlatformX(200.f), lastPlatformType(Platform::PlatformType::Normal) {
    // Initialize the world with safe starting platform positions.
    spawnInitialPlatforms();
}

static sf::Texture& getTextureForType(ResourceManager<sf::Texture>& manager, Platform::PlatformType type) {
    // Select the correct platform texture based on platform type.
    switch (type) {
        case Platform::PlatformType::Broken:
            return manager.get("platform_broken");
        case Platform::PlatformType::Moving:
            return manager.get("platform_moving");
        default:
            return manager.get("platform");
    }
}

static Platform::PlatformType choosePlatformType(std::mt19937 &gen, Platform::PlatformType previousType) {
    // Randomly choose a platform type while avoiding two broken platforms in a row.
    std::uniform_int_distribution<int> typeDist(0, 4);
    int draw = typeDist(gen);
    Platform::PlatformType type;
    if (draw == 0) {
        type = Platform::PlatformType::Broken;
    } else if (draw == 1) {
        type = Platform::PlatformType::Moving;
    } else {
        type = Platform::PlatformType::Normal;
    }

    if (previousType == Platform::PlatformType::Broken && type == Platform::PlatformType::Broken) {
        type = Platform::PlatformType::Normal;
    }
    return type;
}

static bool chooseHasSpring(std::mt19937 &gen, Platform::PlatformType type) {
    if (type == Platform::PlatformType::Broken) {
        return false;
    }

    std::uniform_int_distribution<int> springDist(0, 4);
    return springDist(gen) == 0;
}

static float chooseNextPlatformX(float previousX, std::mt19937 &gen, float minX, float maxX) {
    // Choose the next platform X position relative to the previous one.
    std::uniform_real_distribution<float> offset(-60.f, 60.f);
    float nextX = previousX + offset(gen);
    return std::clamp(nextX, minX, maxX);
}

void WorldManager::spawnInitialPlatforms() {
    // Create the first platform under the player, then add more above it.
    auto &normalTex = textureManager.get("platform");
    auto &springTex = textureManager.get("spring");
    platforms.push_back(Platform(normalTex, sf::Vector2f(200.f, 750.f), Platform::PlatformType::Normal));

    std::uniform_real_distribution<float> disXOffset(-80.f, 80.f);
    std::uniform_real_distribution<float> disY(70.f, 95.f);

    float currentY = 750.f;
    lastPlatformX = 200.f;
    std::vector<float> occupiedYs;
    occupiedYs.push_back(currentY);

    for (int i = 0; i < 9; ++i) {
        float nextY;
        do {
            nextY = currentY - disY(gen);
        } while (std::any_of(occupiedYs.begin(), occupiedYs.end(), [&](float y) {
            return std::abs(y - nextY) < 60.f;
        }));

        currentY = nextY;
        occupiedYs.push_back(currentY);

        float nextX = chooseNextPlatformX(lastPlatformX, gen, 50.f, 500.f - 100.f - 50.f);
        lastPlatformX = nextX;

        Platform::PlatformType type = choosePlatformType(gen, lastPlatformType);
        lastPlatformType = type;

        sf::Texture &texture = getTextureForType(textureManager, type);
        bool hasSpring = chooseHasSpring(gen, type);
        platforms.push_back(Platform(texture, sf::Vector2f(nextX, currentY), type, &springTex, hasSpring));
    }
}

float WorldManager::update(Player& player, float deltaTime) {
    // Handle player-platform collision only when the player is falling downward.
    if (player.getVelocity().y > 0.f) {
        sf::FloatRect playerBounds = player.getBounds();
        float playerBottom = playerBounds.top + playerBounds.height;

        for (auto& plat : platforms) {
            if (!plat.isActive()) {
                continue;
            }

            sf::FloatRect platBounds = plat.getBounds();
            sf::FloatRect springBounds = plat.getSpringBounds();
            if (plat.containsSpring() && playerBounds.intersects(springBounds)) {
                if (playerBottom < springBounds.top + springBounds.height) {
                    plat.activateSpring();
                    player.springJump();
                    break;
                }
            } else if (playerBounds.intersects(platBounds)) {
                if (playerBottom < platBounds.top + platBounds.height) {
                    if (plat.getType() == Platform::PlatformType::Broken) {
                        plat.breakPlatform();
                    } else {
                        player.jump();
                    }
                    break;
                }
            }
        }
    }

    float scrollAmount = 0.f;
    if (player.getPosition().y < 400.f) {
        // Scroll the level up when the player reaches the upper region.
        float diff = 400.f - player.getPosition().y;
        scrollAmount = diff;

        sf::Vector2f pPos = player.getPosition();
        pPos.y = 400.f;
        player.setPosition(pPos);

        std::uniform_real_distribution<float> disY(75.f, 95.f);

        float minY = std::numeric_limits<float>::max();
        for (auto& plat : platforms) {
            sf::Vector2f pos = plat.getPosition();
            pos.y += diff;
            plat.setPosition(pos);
            minY = std::min(minY, pos.y);
            plat.update(deltaTime, 500.f);
        }

        // Recycle platforms that fall below the bottom of the screen.
        for (auto& plat : platforms) {
            if (plat.getPosition().y > 800.f) {
                float newY;
                do {
                    newY = minY - disY(gen);
                } while (std::any_of(platforms.begin(), platforms.end(), [&](const Platform &other) {
                    return std::abs(other.getPosition().y - newY) < 60.f;
                }));

                float nextX = chooseNextPlatformX(lastPlatformX, gen, 50.f, 500.f - 100.f - 50.f);
                lastPlatformX = nextX;

                Platform::PlatformType type = choosePlatformType(gen, lastPlatformType);
                lastPlatformType = type;

                sf::Texture &texture = getTextureForType(textureManager, type);
                sf::Texture &springTex = textureManager.get("spring");
                bool hasSpring = chooseHasSpring(gen, type);
                plat.reset(texture, type, sf::Vector2f(nextX, newY), &springTex, hasSpring);
                minY = newY;
            }
        }
    } else {
        // Update all platforms when the player is not triggering level scroll.
        for (auto& plat : platforms) {
            plat.update(deltaTime, 500.f);
        }
    }

    return scrollAmount;
}

void WorldManager::draw(sf::RenderWindow& window) {
    for (auto& plat : platforms) {
        plat.draw(window);
    }
}
