// ============================================================================
//  Emberforge — Unit Tests
// ============================================================================
#include "Character.h"
#include "Player.h"
#include "Enemy.h"
#include "Random.h"
#include <iostream>
#include <cassert>
#include <string>

using namespace emberforge;

static int testsPassed = 0;
static int testsFailed = 0;

#define CHECK(expr) do { \
    if (expr) { \
        ++testsPassed; \
    } else { \
        ++testsFailed; \
        std::cerr << "FAIL: " << #expr << " at line " << __LINE__ << "\n"; \
    } \
} while(0)

#define CHECK_EQ(a, b) CHECK((a) == (b))

void testCharacterTakeDamageAndDodge() {
    Player player("Hero");
    int initialHp = player.getHp();
    
    // Normal damage mitigation: defense is 12 for Warrior, 12 / 3 = 4 mitigation
    DamageResult r1 = player.takeDamage(20);
    CHECK_EQ(r1.damage, 16); // 20 - 4
    CHECK_EQ(r1.dodged, false);
    CHECK_EQ(player.getHp(), initialHp - 16);
    
    // Activate dodge
    player.setDodging(true);
    CHECK(player.isDodging());
    
    // Incoming damage while dodging:
    // raw = 20, def mit = 4 -> mitigated = 16 * 0.25 = 4
    DamageResult r2 = player.takeDamage(20);
    CHECK_EQ(r2.damage, 4);
    CHECK_EQ(r2.dodged, true);
    CHECK_EQ(player.isDodging(), false); // flag reset after hit
    
    // Next hit should NOT be dodged
    DamageResult r3 = player.takeDamage(10);
    CHECK_EQ(r3.damage, 6); // 10 - 4
    CHECK_EQ(r3.dodged, false);
}

void testPlayerLevelUp() {
    Player player("Hero");
    CHECK_EQ(player.getLevel(), 1);
    CHECK_EQ(player.getXpToNext(), 50);

    // Gaining 30 XP (not enough)
    player.gainXP(30);
    CHECK_EQ(player.checkLevelUp(), false);
    CHECK_EQ(player.getLevel(), 1);

    // Gaining 30 more XP -> total 60 XP -> triggers level up
    player.gainXP(30);
    CHECK_EQ(player.checkLevelUp(), true);
    CHECK_EQ(player.getLevel(), 2);
    CHECK_EQ(player.getXp(), 10);
    CHECK_EQ(player.getMaxHp(), 140); // 120 + 20 (Warrior)
    CHECK_EQ(player.getAttack(), 19);  // 16 + 3 (Warrior)
}

void testEnemyFactoryAndScaling() {
    auto skel1 = Enemy::create(EnemyType::Skeleton, 1);
    auto skel3 = Enemy::create(EnemyType::Skeleton, 3);
    
    CHECK(skel1 != nullptr);
    CHECK(skel3 != nullptr);
    CHECK_EQ(skel1->getType(), EnemyType::Skeleton);
    CHECK(skel3->getMaxHp() > skel1->getMaxHp());
    CHECK(skel3->getAttack() > skel1->getAttack());
    CHECK(skel3->getDefense() > skel1->getDefense());

    auto boss = Enemy::create(EnemyType::LichKing, 5);
    CHECK_EQ(boss->getType(), EnemyType::LichKing);
    CHECK(boss->getMaxHp() > skel3->getMaxHp());

    // Enemy special attacks
    CHECK_EQ(skel1->getSpecialActionName(), "Bone Frenzy");
    auto knight = Enemy::create(EnemyType::DarkKnight, 2);
    CHECK_EQ(knight->getSpecialActionName(), "Shield Slam");
    auto wraith = Enemy::create(EnemyType::Wraith, 2);
    CHECK_EQ(wraith->getSpecialActionName(), "Life Siphon");
    auto drake = Enemy::create(EnemyType::FireDrake, 3);
    CHECK_EQ(drake->getSpecialActionName(), "Inferno Breath");
    CHECK_EQ(boss->getSpecialActionName(), "Soul Devour");
}

void testRandomUtility() {
    Random::seed(42);
    for (int i = 0; i < 100; ++i) {
        int val = Random::get(5, 15);
        CHECK(val >= 5 && val <= 15);
    }
}

void testPlayerEconomyAndUpgrades() {
    Player player("Hero");
    CHECK_EQ(player.getGold(), 0);

    // Cannot spend what you don't have
    CHECK_EQ(player.spendGold(20), false);
    CHECK_EQ(player.getGold(), 0);

    // Earn gold and spend
    player.gainGold(50);
    CHECK_EQ(player.getGold(), 50);
    CHECK_EQ(player.spendGold(30), true);
    CHECK_EQ(player.getGold(), 20);
    CHECK_EQ(player.spendGold(25), false); // not enough
    CHECK_EQ(player.getGold(), 20);

    // Upgrades
    int baseAtk = player.getAttack();
    player.upgradeAttack(3);
    CHECK_EQ(player.getAttack(), baseAtk + 3);

    int baseDef = player.getDefense();
    player.upgradeDefense(2);
    CHECK_EQ(player.getDefense(), baseDef + 2);

    int baseHp = player.getMaxHp();
    player.upgradeMaxHp(20);
    CHECK_EQ(player.getMaxHp(), baseHp + 20);

    int baseSt = player.getMaxStamina();
    player.upgradeMaxStamina(15);
    CHECK_EQ(player.getMaxStamina(), baseSt + 15);
}

void testPlayerClassesAndPerks() {
    Player warrior("Thorin", PlayerClass::Warrior);
    Player rogue("Locke", PlayerClass::Rogue);
    Player mage("Raistlin", PlayerClass::Mage);

    CHECK_EQ(warrior.getClassName(), "Warrior");
    CHECK_EQ(rogue.getClassName(), "Rogue");
    CHECK_EQ(mage.getClassName(), "Mage");

    // Stat distinction
    CHECK_EQ(warrior.getMaxHp(), 120);
    CHECK_EQ(warrior.getDefense(), 12);

    CHECK_EQ(rogue.getMaxHp(), 90);
    CHECK_EQ(rogue.getMaxStamina(), 120);
    CHECK_EQ(rogue.getAttack(), 22);

    CHECK_EQ(mage.getMaxHp(), 75);
    CHECK_EQ(mage.getMaxStamina(), 130);
    CHECK_EQ(mage.getAttack(), 26);

    // Rogue dodge cost is 7 instead of 10
    int stBefore = rogue.getStamina();
    rogue.performDodge();
    CHECK_EQ(rogue.getStamina(), stBefore - 7);

    // Warrior dodge cost is 10
    stBefore = warrior.getStamina();
    warrior.performDodge();
    CHECK_EQ(warrior.getStamina(), stBefore - 10);
}

int main() {
    std::cout << "Running Emberforge unit tests...\n";
    testCharacterTakeDamageAndDodge();
    testPlayerLevelUp();
    testEnemyFactoryAndScaling();
    testRandomUtility();
    testPlayerEconomyAndUpgrades();
    testPlayerClassesAndPerks();

    std::cout << "\nTest Results: " << testsPassed << " passed, " << testsFailed << " failed.\n";
    return (testsFailed == 0) ? 0 : 1;
}

