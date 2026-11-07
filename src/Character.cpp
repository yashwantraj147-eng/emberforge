// ============================================================================
//  Emberforge — Character Implementation
// ============================================================================
#include "Character.h"
#include "Random.h"

#include <algorithm>

namespace emberforge {

// ── Constructor ────────────────────────────────────────────────────────────
Character::Character(const std::string& name,
                     int hp, int stamina, int attack, int defense, int level)
    : name_(name)
    , hp_(hp),        maxHp_(hp)
    , stamina_(stamina), maxStamina_(stamina)
    , attack_(attack)
    , defense_(defense)
    , level_(level)
    , dodging_(false)
{}

// ── Damage output with ±20 % variance ──────────────────────────────────────
int Character::dealDamage() const {
    double mult = Random::getReal(0.8, 1.2);
    return std::max(1, static_cast<int>(attack_ * mult));
}

// ── Receive damage (defense reduction + dodge mitigation) ──────────────────
DamageResult Character::takeDamage(int rawDamage) {
    bool wasDodging = dodging_;
    int mitigated = rawDamage - defense_ / 3;
    if (wasDodging) {
        mitigated = static_cast<int>(mitigated * 0.25);
        dodging_ = false;
    }
    mitigated = std::max(1, mitigated);
    hp_       = std::max(0, hp_ - mitigated);
    return {mitigated, wasDodging};
}

// ── Resource helpers ───────────────────────────────────────────────────────
void Character::restoreStamina(int amount) {
    stamina_ = std::min(maxStamina_, stamina_ + amount);
}

void Character::consumeStamina(int amount) {
    stamina_ = std::max(0, stamina_ - amount);
}

void Character::heal(int amount) {
    hp_ = std::min(maxHp_, hp_ + amount);
}

} // namespace emberforge
