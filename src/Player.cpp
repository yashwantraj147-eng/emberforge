// ============================================================================
//  Emberforge — Player Implementation
// ============================================================================
#include "Player.h"

#include <algorithm>

namespace emberforge {

Player::Player(const std::string& name)
    : Character(name, /*hp=*/100, /*stamina=*/100, /*atk=*/18, /*def=*/8, /*lvl=*/1)
    , xp_(0), xpToNext_(50), potions_(3), gold_(0), kills_(0)
{}

// ── Combat actions ─────────────────────────────────────────────────────────
int Player::performStrike() {
    consumeStamina(cost::STRIKE);
    return dealDamage();
}

int Player::performHeavyStrike() {
    consumeStamina(cost::HEAVY_STRIKE);
    return static_cast<int>(dealDamage() * 1.8);
}

void Player::performDodge() {
    consumeStamina(cost::DODGE);
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
void Player::addPotion(int count)      { potions_ += count; }
void Player::incrementKills()          { ++kills_; }

bool Player::checkLevelUp() {
    if (xp_ < xpToNext_) return false;

    xp_ -= xpToNext_;
    ++level_;
    xpToNext_    = static_cast<int>(xpToNext_ * 1.5);
    maxHp_      += 15;
    hp_          = maxHp_;          // full heal on level-up
    maxStamina_ += 10;
    stamina_     = maxStamina_;
    attack_     += 4;
    defense_    += 2;
    return true;
}

void Player::restAtCampfire() {
    hp_      = maxHp_;
    stamina_ = maxStamina_;
}

// ── Identity ───────────────────────────────────────────────────────────────
std::string Player::getAsciiArt() const {
    return R"(
        O
       /|\
      / | \
       / \
      /   \
    )";
}

std::string Player::getColorCode() const {
    return ansi::BR_CYAN;
}

} // namespace emberforge
