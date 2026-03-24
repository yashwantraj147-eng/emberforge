// ============================================================================
//  Emberforge — Character Implementation
// ============================================================================
#include "Character.h"

#include <algorithm>
#include <random>

namespace emberforge {

// ── Static RNG (thread-safe seed) ──────────────────────────────────────────
static std::mt19937& rng() {
    static std::mt19937 gen(std::random_device{}());
    return gen;
}

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
    std::uniform_real_distribution<double> dist(0.8, 1.2);
    return std::max(1, static_cast<int>(attack_ * dist(rng())));
}

// ── Receive damage (defense reduction + dodge mitigation) ──────────────────
int Character::takeDamage(int rawDamage) {
    int mitigated = rawDamage - defense_ / 3;
    if (dodging_) {
        mitigated = static_cast<int>(mitigated * 0.25);
        dodging_ = false;
    }
    mitigated = std::max(1, mitigated);
    hp_       = std::max(0, hp_ - mitigated);
    return mitigated;
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
