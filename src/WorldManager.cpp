#include "WorldManager.hpp"
#include "AudioManager.hpp"
#include <random>
#include <limits>
#include <vector>
#include <algorithm>

WorldManager::WorldManager(ResourceManager<sf::Texture> &texMgr, Difficulty diff)
    : gameOver(false),
    textureManager(texMgr), 
    gen(std::random_device{}()), 
    difficulty(diff),
    lastPlatformX(200.f), 
    lastPlatformType(Platform::PlatformType::Normal),
    totalScrolledDistance(0.f) // مقداردهی اولیه مسافت
{
    monsterTex1 = &texMgr.get("monster1");
    monsterTex2 = &texMgr.get("monster2");
    
    holeSmallTex = &texMgr.get("hole");
    holeLargeTex = &texMgr.get("hole_large");

    // Initialize the world with safe starting platform positions.
    spawnInitialPlatforms();
}

void WorldManager::spawnMonsterNearPlatform(const Platform& platform, float windowWidth) {
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

    // مقدار سلامتی اولیه بر اساس سطح دشواری (مطابق مستندات فاز دوم)
    int health = 1;
    if (difficulty == Difficulty::Medium) health = 2;
    else if (difficulty == Difficulty::Hard) health = 3;

    monsters.emplace_back(*monsterTex1, *monsterTex2, pos, health);
    monsters.back().setSpeedMultiplier(getSpeedMultiplier());
}

void WorldManager::spawnHoleNearPlatform(const Platform& platform, float windowWidth) {
    if (difficulty != Difficulty::Hard) return; // فقط در حالت Hard فعال است

    std::uniform_int_distribution<int> prob(0, 5);
    if (prob(gen) != 0) return;

    sf::FloatRect platBounds = platform.getBounds();
    if (platBounds.width == 0) return;

    bool isSmall = (rand() % 2 == 0);
    float w = isSmall ? 40.f : 60.f;
    float h = isSmall ? 40.f : 60.f;

    std::uniform_real_distribution<float> xOffset(0.f, platBounds.width - w);
    float x = platBounds.left + xOffset(gen);
    float y = platBounds.top - h - 10.f;
    x = std::max(0.f, std::min(x, windowWidth - w));

    sf::Vector2f pos(x, y);
    if (!isAreaClear(sf::FloatRect(pos, sf::Vector2f(w, h)), 10.f)) {
        return;
    }

    holes.emplace_back(*holeSmallTex, *holeLargeTex, pos, isSmall);
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
        
        if (totalScrolledDistance > 1500.f) {
            spawnMonsterNearPlatform(platforms.back(), 500.f);
        }

        spawnHoleNearPlatform(platforms.back(), 500.f);
    }
}

float WorldManager::update(Player &player, float deltaTime)
{
    if (gameOver) return 0.f;

    // ۱. به‌روزرسانی و بررسی برخورد گلوله‌ها با هیولاها
    updateBullets(deltaTime);

    for (auto& mon : monsters) {
        mon.update(deltaTime, 500.f);
    }

    for (auto& hole : holes) {
        hole.update(deltaTime);
    }

    sf::FloatRect playerBounds = player.getBounds();
    float playerBottom = playerBounds.top + playerBounds.height;

    // ۲. بررسی برخورد بازیکن با هیولاها
    for (auto& mon : monsters) {
            if (!mon.isActive()) continue;
            
            sf::FloatRect monBounds = mon.getBounds();
            if (playerBounds.intersects(monBounds)) {
                bool fallingDown = player.getVelocity().y > 0.f;
                
                // شرط دقیق‌تر برای فرود آمدن از بالا روی سر هیولا:
                // ۱. بازیکن در حال سقوط به پایین باشد (velocity.y > 0)
                // ۲. لبه پایینی بازیکن در فریم قبلی یا فعلی بالاتر یا بسیار نزدیک به لبه بالایی هیولا باشد
                // ۳. هم‌پوشانی افقی کافی وجود داشته باشد تا برخورد واقعاً از پهلو محسوب نشود
                float playerCenter = playerBounds.left + playerBounds.width / 2.f;
                bool isAboveMonster = previousPlayerBottom <= monBounds.top + 12.f;
                bool isHorizontallyAligned = (playerCenter >= monBounds.left - 5.f) && (playerCenter <= monBounds.left + monBounds.width + 5.f);

                if (fallingDown && isAboveMonster && isHorizontallyAligned) {
                    player.springJump(); 
                    AudioManager::getInstance().playJump();
                    mon.setActive(false); // نابودی هیولا با پرش از بالا
                    break;
                } else {
                    // برخورد از پهلو، زیر یا زوایای دیگر -> باخت فوری
                    gameOver = true;
                    AudioManager::getInstance().playGameOver();
                    return 0.f;
                }
            }
        }

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
    if (player.getPosition().y < 400.f)
    {
        float diff = 400.f - player.getPosition().y;
        scrollAmount = diff;

        sf::Vector2f pPos = player.getPosition();
        pPos.y = 400.f;
        player.setPosition(pPos);

        std::uniform_real_distribution<float> disY(75.f, 95.f);

        float minY = std::numeric_limits<float>::max();
        for (auto &plat : platforms)
        {
            sf::Vector2f pos = plat.getPosition();
            pos.y += diff;
            plat.setPosition(pos);
            minY = std::min(minY, pos.y);
            plat.update(deltaTime, 500.f);
        }

        for (auto &mon : monsters) {
            sf::Vector2f pos = mon.getPosition();
            pos.y += diff;
            mon.setPosition(pos);
        }

        for (auto &hole : holes) {
            sf::Vector2f pos = hole.getPosition();
            pos.y += diff;
            hole.setPosition(pos);
        }

        float speedMultiplier = 1.0f;
        if (difficulty == Difficulty::Medium) {
            speedMultiplier = 2.0f; 
        } else if (difficulty == Difficulty::Hard) {
            speedMultiplier = 3.0f; 
        }
        
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

                if (totalScrolledDistance > 1500.f) {
                    spawnMonsterNearPlatform(plat, 500.f);
                }

                spawnHoleNearPlatform(plat, 500.f);
            }
        }
    }
    else
    {
        for (auto &plat : platforms)
        {
            plat.update(deltaTime, 500.f);
        }
    }

    if (scrollAmount > 0.f)
    {
        totalScrolledDistance += scrollAmount;
    }

    // پاکسازی هیولاهای مرده یا خارج شده از صفحه
    for (auto it = monsters.begin(); it != monsters.end();) {
        if (!it->isActive() || it->getPosition().y > 900.f) {
            it = monsters.erase(it);
        } else {
            ++it;
        }
    }

    for (auto it = holes.begin(); it != holes.end();) {
        if (!it->isActive() || it->getPosition().y > 900.f) {
            it = holes.erase(it);
        } else {
            ++it;
        }
    }

    // ذخیره موقعیت پایینی بازیکن برای استفاده در فریم بعدی
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

    for (auto &hole : holes) {
        if (hole.isActive()) {
            hole.draw(window);
        }
    }

    // رسم گلوله‌ها روی صفحه
    renderBullets(window);
}

bool WorldManager::isAreaClear(const sf::FloatRect& area, float padding) const {
    sf::FloatRect paddedArea(
        area.left - padding,
        area.top - padding,
        area.width + (padding * 2.f),
        area.height + (padding * 2.f)
    );

    for (const auto& plat : platforms) {
        if (plat.isActive() && plat.getBounds().intersects(paddedArea)) {
            return false; 
        }
    }

    for (const auto& mon : monsters) {
        if (mon.isActive() && mon.getBounds().intersects(paddedArea)) {
            return false;
        }
    }

    for (const auto& hole : holes) {
        if (hole.isActive() && hole.getBounds().intersects(paddedArea)) {
            return false;
        }
    }
    return true; 
}

bool WorldManager::checkHoleCollision(Player& player, sf::Vector2f& outHoleCenter) {
    sf::FloatRect playerBounds = player.getBounds();
    for (auto& hole : holes) {
        if (hole.isActive() && hole.getBounds().intersects(playerBounds)) {
            outHoleCenter = hole.getPosition();
            return true;
        }
    }
    return false;
}

void WorldManager::spawnBullet(sf::Vector2f startPosition) {
    AudioManager::getInstance().playShoot();
    sf::Vector2f bulletVelocity(0.f, -600.f);
    bullets.emplace_back(startPosition, bulletVelocity);
}

void WorldManager::updateBullets(float deltaTime) {
    for (auto& bullet : bullets) {
        if (!bullet.isActive()) continue;

        bullet.update(deltaTime);

        for (auto& monster : monsters) {
            // بررسی برخورد گلوله فعال با هیولای فعال
            if (monster.isActive() && bullet.getBounds().intersects(monster.getBounds())) {
                bullet.setActive(false); // غیرفعال کردن گلوله
                monster.takeDamage(1);   // کاهش یک واحد از جان هیولا (طبق مستندات)
                break;
            }
        }
    }

    // پاکسازی گلوله‌های غیرفعال یا خارج شده از صفحه
    bullets.erase(
        std::remove_if(bullets.begin(), bullets.end(), [](const Bullet& b) { return !b.isActive(); }),
        bullets.end()
    );
}

void WorldManager::renderBullets(sf::RenderWindow& window) {
    for (auto& bullet : bullets) {
        bullet.render(window);
    }
}

