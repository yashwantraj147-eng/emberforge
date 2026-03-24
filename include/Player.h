// ============================================================================
//  Emberforge — Player Character
//  The hero controlled by the user; tracks XP, gold, potions, and kills.
// ============================================================================
#pragma once

#include "Character.h"
#include <string>

namespace emberforge {

class Player : public Character {
    int xp_;
    int xpToNext_;
    int potions_;
    int gold_;
    int kills_;

public:
    explicit Player(const std::string& name);

    // ── Observers ──────────────────────────────────────────────────────────
    int  getXp()       const { return xp_; }
    int  getXpToNext() const { return xpToNext_; }
    int  getPotions()  const { return potions_; }
    int  getGold()     const { return gold_; }
    int  getKills()    const { return kills_; }

    // ── Combat actions ─────────────────────────────────────────────────────
    /// Normal attack — costs 15 stamina.
    int  performStrike();
    /// Powerful swing — costs 30 stamina, 1.8× damage.
    int  performHeavyStrike();
    /// Brace for impact — costs 10 stamina, next hit reduced by 75 %.
    void performDodge();
    /// Catch your breath — recover 25 stamina, skip your attack.
    void performRest();
    /// Drink a healing potion — restores 40 HP.
    bool usePotion();

    // ── Progression ────────────────────────────────────────────────────────
    void gainXP(int amount);
    void gainGold(int amount);
    void addPotion(int count = 1);
    void incrementKills();
    /// Returns true when a level-up occurs (auto-applies stat boosts).
    bool checkLevelUp();
    /// Full heal between encounters (campfire).
    void restAtCampfire();

    // ── Polymorphic identity ───────────────────────────────────────────────
    std::string getAsciiArt()  const override;
    std::string getColorCode() const override;
};

} // namespace emberforge
