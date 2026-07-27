#include "WorldManager.hpp"
#include "AudioManager.hpp"
#include <random>
#include <limits>
#include <vector>
#include <algorithm>

WorldManager::WorldManager(ResourceManager<sf::Texture> &texMgr, Difficulty diff)
    : textureManager(texMgr), 
    gen(std::random_device{}()), 
    lastPlatformX(200.f), 
    lastPlatformType(Platform::PlatformType::Normal),
    difficulty(diff),
    gameOver(false) {
        monsterTex1 = &texMgr.get("monster1");
        monsterTex2 = &texMgr.get("monster2");
        spawnInitialPlatforms();
}

void WorldManager::spawnMonsterNearPlatform(const Platform& platform, float windowWidth) {
    // احتمال ظاهر شدن هیولا (مثلاً ۱ در ۵)
    std::uniform_int_distribution<int> prob(0, 4);
    if (prob(gen) != 0) return;

    sf::FloatRect platBounds = platform.getBounds();
    if (platBounds.width == 0) return;

    float monsterWidth = 40.f;  
    float monsterHeight = 40.f;
    std::uniform_real_distribution<float> xOffset(0.f, platBounds.width - monsterWidth);
    float x = platBounds.left + xOffset(gen);
    float y = platBounds.top - monsterHeight - 5.f;

    x = std::max(0.f, std::min(x, windowWidth - monsterWidth));

    sf::Vector2f pos(x, y);
    if (isPositionOverlappingWithAnyObject(pos, monsterWidth, monsterHeight)) {
        return; 
    }

    int health = 1;
    if (difficulty == Difficulty::Medium) health = 2;
    else if (difficulty == Difficulty::Hard) health = 3;

    monsters.emplace_back(*monsterTex1, *monsterTex2, pos, health);
    monsters.back().setSpeedMultiplier(getSpeedMultiplier());
}

bool WorldManager::isPositionOverlappingWithAnyObject(sf::Vector2f pos, float width, float height) const {
    sf::FloatRect rect(pos, sf::Vector2f(width, height));

    for (const auto& plat : platforms) {
        if (plat.isActive() && plat.getBounds().intersects(rect)) {
            return true;
        }
    }

    for (const auto& mon : monsters) {
        if (mon.isActive() && mon.getBounds().intersects(rect)) {
            return true;
        }
    }

    return false;
}

float WorldManager::getSpeedMultiplier() const {
    if (difficulty == Difficulty::Medium) return 1.5f;
    if (difficulty == Difficulty::Hard) return 2.0f;
    return 1.0f;
}

static sf::Texture &getTextureForType(ResourceManager<sf::Texture> &manager, Platform::PlatformType type)
{
    switch (type)
    {
    case Platform::PlatformType::Broken:
        return manager.get("platform_broken");
    case Platform::PlatformType::Moving:
        return manager.get("platform_moving");
    default:
        return manager.get("platform");
    }
}

static Platform::PlatformType choosePlatformType(std::mt19937 &gen, Platform::PlatformType previousType, Difficulty diff)
{
    std::uniform_int_distribution<int> typeDist(0, 4);
    int draw = typeDist(gen);
    Platform::PlatformType type;

    if (draw == 0)
    {
        type = Platform::PlatformType::Broken;
    }
    else if (draw == 1)
    {
        type = Platform::PlatformType::Moving;
    }
    else
    {
        type = Platform::PlatformType::Normal;
    }

    if (previousType == Platform::PlatformType::Broken && type == Platform::PlatformType::Broken)
    {
        type = Platform::PlatformType::Normal;
    }
    return type;
}

static bool chooseHasSpring(std::mt19937 &gen, Platform::PlatformType type)
{
    if (type == Platform::PlatformType::Broken)
    {
        return false;
    }

    std::uniform_int_distribution<int> springDist(0, 4);
    return springDist(gen) == 0;
}

static float chooseNextPlatformX(float previousX, std::mt19937 &gen, float minX, float maxX)
{
    std::uniform_real_distribution<float> offset(-60.f, 60.f);
    float nextX = previousX + offset(gen);
    return std::clamp(nextX, minX, maxX);
}

void WorldManager::spawnInitialPlatforms()
{
    auto &normalTex = textureManager.get("platform");
    auto &springTex = textureManager.get("spring");
    platforms.push_back(Platform(normalTex, sf::Vector2f(200.f, 750.f), Platform::PlatformType::Normal));

    std::uniform_real_distribution<float> disXOffset(-80.f, 80.f);
    std::uniform_real_distribution<float> disY(70.f, 95.f);

    float currentY = 750.f;
    lastPlatformX = 200.f;
    std::vector<float> occupiedYs;
    occupiedYs.push_back(currentY);

    float speedMultiplier = 1.0f;
    if (difficulty == Difficulty::Medium) {
        speedMultiplier = 2.0f; 
    } else if (difficulty == Difficulty::Hard) {
        speedMultiplier = 3.0f; 
    }

    for (int i = 0; i < 9; ++i)
    {
        float nextY;
        do
        {
            nextY = currentY - disY(gen);
        } while (std::any_of(occupiedYs.begin(), occupiedYs.end(), [&](float y)
                             { return std::abs(y - nextY) < 60.f; }));

        currentY = nextY;
        occupiedYs.push_back(currentY);

        float nextX = chooseNextPlatformX(lastPlatformX, gen, 50.f, 500.f - 100.f - 50.f);
        lastPlatformX = nextX;

        Platform::PlatformType type = choosePlatformType(gen, lastPlatformType, difficulty);
        lastPlatformType = type;

        sf::Texture &texture = getTextureForType(textureManager, type);
        bool hasSpring = chooseHasSpring(gen, type);
        platforms.push_back(Platform(texture, sf::Vector2f(nextX, currentY), type, &springTex, hasSpring));
        platforms.back().setSpeedMultiplier(speedMultiplier);
        spawnMonsterNearPlatform(platforms.back(), 500.f);
    }
}

float WorldManager::update(Player &player, float deltaTime)
{
    if (gameOver) return 0.f;

    // آپدیت هیولاها
    for (auto& mon : monsters) {
        mon.update(deltaTime, 500.f);
    }

    // --- بررسی برخورد با هیولاها ---
    sf::FloatRect playerBounds = player.getBounds();
    float playerBottom = playerBounds.top + playerBounds.height;

    for (auto& mon : monsters) {
        if (!mon.isActive()) continue;
        
        sf::FloatRect monBounds = mon.getBounds();
        if (playerBounds.intersects(monBounds)) {
            // بررسی دقیق‌تر برای اطمینان از اینکه بازیکن از بالا و در حال سقوط روی هیولا فرود آمده است
            bool fallingDown = player.getVelocity().y > 0.f;
            bool landedFromAbove = previousPlayerBottom <= monBounds.top + 10.f;

            if (fallingDown && landedFromAbove) {
                player.springJump(); 
                AudioManager::getInstance().playJump();
                mon.setActive(false); 
                break;
            } else {
                gameOver = true;
                AudioManager::getInstance().playGameOver();
                return 0.f;
            }
        }
    }

    // --- بررسی برخورد با پلتفرم‌ها (فقط وقتی در حال سقوط است) ---
    if (player.getVelocity().y > 0.f)
    {
        for (auto &plat : platforms)
        {
            if (!plat.isActive()) continue;

            sf::FloatRect platBounds = plat.getBounds();
            sf::FloatRect springBounds = plat.getSpringBounds();
            
            if (plat.containsSpring() && playerBounds.intersects(springBounds))
            {
                if (playerBottom < springBounds.top + springBounds.height)
                {
                    plat.activateSpring();
                    player.springJump();
                    AudioManager::getInstance().playJump(); 
                    break;
                }
            }
            else if (playerBounds.intersects(platBounds))
            {
                if (playerBottom < platBounds.top + platBounds.height)
                {
                    if (plat.getType() == Platform::PlatformType::Broken)
                    {
                        plat.breakPlatform();
                    }
                    else
                    {
                        player.jump();
                        AudioManager::getInstance().playJump(); 
                    }
                    break;
                }
            }
        }
    }

    float scrollAmount = 0.f;
    
    // اسکرول صفحه
    if (player.getPosition().y < 400.f)
    {
        float diff = 400.f - player.getPosition().y;
        scrollAmount = diff;

        sf::Vector2f pPos = player.getPosition();
        pPos.y = 400.f;
        player.setPosition(pPos);

        std::uniform_real_distribution<float> disY(75.f, 95.f);
        float minY = std::numeric_limits<float>::max();

        // اسکرول پلتفرم‌ها
        for (auto &plat : platforms)
        {
            sf::Vector2f pos = plat.getPosition();
            pos.y += diff;
            plat.setPosition(pos);
            minY = std::min(minY, pos.y);
            plat.update(deltaTime, 500.f);
        }

        // اسکرول هیولاها
        for (auto &mon : monsters) {
            sf::Vector2f pos = mon.getPosition();
            pos.y += diff;
            mon.setPosition(pos);
        }

        float speedMultiplier = 1.0f;
        if (difficulty == Difficulty::Medium) {
            speedMultiplier = 2.0f; 
        } else if (difficulty == Difficulty::Hard) {
            speedMultiplier = 3.0f; 
        }
        
        // بازیافت پلتفرم‌های خارج شده از پایین صفحه
        for (auto &plat : platforms)
        {
            if (plat.getPosition().y > 800.f)
            {
                float newY;
                do
                {
                    newY = minY - disY(gen);
                } while (std::any_of(platforms.begin(), platforms.end(), [&](const Platform &other)
                                     { return std::abs(other.getPosition().y - newY) < 60.f; }));

                float nextX = chooseNextPlatformX(lastPlatformX, gen, 50.f, 500.f - 100.f - 50.f);
                lastPlatformX = nextX;

                Platform::PlatformType type = choosePlatformType(gen, lastPlatformType, difficulty);
                lastPlatformType = type;

                sf::Texture &texture = getTextureForType(textureManager, type);
                sf::Texture &springTex = textureManager.get("spring");
                bool hasSpring = chooseHasSpring(gen, type);
                plat.reset(texture, type, sf::Vector2f(nextX, newY), &springTex, hasSpring);
                plat.setSpeedMultiplier(speedMultiplier);
                minY = newY;
                
                spawnMonsterNearPlatform(plat, 500.f);
            }
        }
    }
    else
    {
        // آپدیت پلتفرم‌های متحرک در حالت عادی
        for (auto &plat : platforms)
        {
            plat.update(deltaTime, 500.f);
        }
    }

    // حذف هیولاهای مرده یا خارج شده از پایین صفحه
    for (auto it = monsters.begin(); it != monsters.end();) {
        if (!it->isActive() || it->getPosition().y > 900.f) {
            it = monsters.erase(it);
        } else {
            ++it;
        }
    }

    previousPlayerBottom = player.getBounds().top + player.getBounds().height;
    return scrollAmount;
}

void WorldManager::draw(sf::RenderWindow &window)
{
    for (auto &plat : platforms)
    {
        plat.draw(window);
    }

    for (auto &mon : monsters) {
        if (mon.isActive()) {
            mon.draw(window);
        }
    }
}