// ============================================================================
//  Emberforge — Player Implementation
// ============================================================================
#include "Player.h"
#include "Random.h"

#include <algorithm>

namespace emberforge {

static int initialHp(PlayerClass c) {
    switch (c) {
        case PlayerClass::Warrior: return 120;
        case PlayerClass::Rogue:   return 90;
        case PlayerClass::Mage:    return 75;
    }
    return 100;
}

static int initialStamina(PlayerClass c) {
    switch (c) {
        case PlayerClass::Warrior: return 90;
        case PlayerClass::Rogue:   return 120;
        case PlayerClass::Mage:    return 130;
    }
    return 100;
}

static int initialAtk(PlayerClass c) {
    switch (c) {
        case PlayerClass::Warrior: return 16;
        case PlayerClass::Rogue:   return 22;
        case PlayerClass::Mage:    return 26;
    }
    return 18;
}

static int initialDef(PlayerClass c) {
    switch (c) {
        case PlayerClass::Warrior: return 12;
        case PlayerClass::Rogue:   return 6;
        case PlayerClass::Mage:    return 4;
    }
    return 8;
}

Player::Player(const std::string& name, PlayerClass playerClass)
    : Character(name, initialHp(playerClass), initialStamina(playerClass),
                initialAtk(playerClass), initialDef(playerClass), /*lvl=*/1)
    , playerClass_(playerClass)
    , xp_(0), xpToNext_(50), potions_(3), gold_(0), kills_(0)
    , lastHitCrit_(false)
{}

std::string Player::getClassName() const {
    switch (playerClass_) {
        case PlayerClass::Warrior: return "Warrior";
        case PlayerClass::Rogue:   return "Rogue";
        case PlayerClass::Mage:    return "Mage";
    }
    return "Warrior";
}

// ── Combat actions ─────────────────────────────────────────────────────────
int Player::performStrike() {
    consumeStamina(cost::STRIKE);
    int dmg = dealDamage();
    lastHitCrit_ = false;
    // Rogue perk: 25% chance of critical strike (+50% damage)
    if (playerClass_ == PlayerClass::Rogue && Random::chance(25)) {
        dmg = static_cast<int>(dmg * 1.5);
        lastHitCrit_ = true;
    }
    return dmg;
}

int Player::performHeavyStrike() {
    consumeStamina(cost::HEAVY_STRIKE);
    int dmg = static_cast<int>(dealDamage() * 1.8);
    lastHitCrit_ = false;
    // Rogue perk: 35% chance of heavy crit (+50% damage)
    if (playerClass_ == PlayerClass::Rogue && Random::chance(35)) {
        dmg = static_cast<int>(dmg * 1.5);
        lastHitCrit_ = true;
    }
    // Mage perk: Arcane surge (additional elemental burn)
    if (playerClass_ == PlayerClass::Mage) {
        dmg = static_cast<int>(dmg * Random::getReal(1.15, 1.35));
    }
    return dmg;
}

void Player::performDodge() {
    int dodgeCost = (playerClass_ == PlayerClass::Rogue) ? 7 : cost::DODGE;
    consumeStamina(dodgeCost);
    setDodging(true);
}

void Player::performRest() {
    restoreStamina(cost::REST_GAIN);
}

bool Player::usePotion() {
    if (potions_ <= 0) return false;
    --potions_;
    heal(cost::POTION_HEAL);
    return true;
}

// ── Progression ────────────────────────────────────────────────────────────
void Player::gainXP(int amount)        { xp_ += amount; }
void Player::gainGold(int amount)      { gold_ += amount; }
bool Player::spendGold(int amount) {
    if (gold_ < amount) return false;
    gold_ -= amount;
    return true;
}
void Player::addPotion(int count)      { potions_ += count; }
void Player::incrementKills()          { ++kills_; }
void Player::upgradeAttack(int amount) { attack_ += amount; }
void Player::upgradeDefense(int amount){ defense_ += amount; }
void Player::upgradeMaxHp(int amount)  {
    maxHp_ += amount;
    hp_    += amount;
}
void Player::upgradeMaxStamina(int amount) {
    maxStamina_ += amount;
    stamina_    += amount;
}

bool Player::checkLevelUp() {
    if (xp_ < xpToNext_) return false;

    xp_ -= xpToNext_;
    ++level_;
    xpToNext_ = static_cast<int>(xpToNext_ * 1.5);

    switch (playerClass_) {
    case PlayerClass::Warrior:
        maxHp_      += 20;
        maxStamina_ += 10;
        attack_     += 3;
        defense_    += 3;
        break;
    case PlayerClass::Rogue:
        maxHp_      += 12;
        maxStamina_ += 15;
        attack_     += 5;
        defense_    += 1;
        break;
    case PlayerClass::Mage:
        maxHp_      += 10;
        maxStamina_ += 15;
        attack_     += 6;
        defense_    += 1;
        break;
    }

    hp_      = maxHp_;          // full heal on level-up
    stamina_ = maxStamina_;
    return true;
}

void Player::restAtCampfire() {
    hp_      = maxHp_;
    stamina_ = maxStamina_;
}

// ── Identity ───────────────────────────────────────────────────────────────
std::string Player::getAsciiArt() const {
    switch (playerClass_) {
    case PlayerClass::Warrior:
        return
            "      |===|      \n"
            "      (o.o)      \n"
            "     /|===|\\    \n"
            "    / |   | \\[]  \n"
            "   [] |===|  ||  \n"
            "     /     \\ ||  \n"
            "    /       \\    \n"
            "   [_________]   \n";
    case PlayerClass::Rogue:
        return
            "      ,-.      \n"
            "     (o.o)     \n"
            "     /`v'\\     \n"
            "   =/{   }\\=   \n"
            "    /|   |\\    \n"
            "   / |   | \\   \n"
            "     /   \\     \n"
            "    '     '    \n";
    case PlayerClass::Mage:
        return
            "       /\\       \n"
            "      /  \\      \n"
            "     /____\\     \n"
            "      (o.o)   * \n"
            "     <|   |> /  \n"
            "      |   | //  \n"
            "     /|   |//   \n"
            "    /__|_|__\\   \n";
    }
    return "";
}

std::string Player::getColorCode() const {
    switch (playerClass_) {
    case PlayerClass::Warrior: return ansi::BR_CYAN;
    case PlayerClass::Rogue:   return ansi::BR_MAGENTA;
    case PlayerClass::Mage:    return ansi::BR_YELLOW;
    }
    return ansi::BR_CYAN;
}

} // namespace emberforge
