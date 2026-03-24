// ============================================================================
//  Emberforge — Character (Abstract Base Class)
//  The root of the entity hierarchy; defines shared combat attributes.
// ============================================================================
#pragma once

#include "Types.h"
#include <string>

namespace emberforge {

class Character {
protected:
    std::string name_;
    int  hp_,        maxHp_;
    int  stamina_,   maxStamina_;
    int  attack_;
    int  defense_;
    int  level_;
    bool dodging_;   // true → next incoming hit is reduced by 75 %

public:
    // ── Construction ───────────────────────────────────────────────────────
    Character(const std::string& name,
              int hp, int stamina, int attack, int defense, int level = 1);
    virtual ~Character() = default;

    // ── Observers ──────────────────────────────────────────────────────────
    const std::string& getName()      const { return name_; }
    int  getHp()          const { return hp_; }
    int  getMaxHp()       const { return maxHp_; }
    int  getStamina()     const { return stamina_; }
    int  getMaxStamina()  const { return maxStamina_; }
    int  getAttack()      const { return attack_; }
    int  getDefense()     const { return defense_; }
    int  getLevel()       const { return level_; }
    bool isDodging()      const { return dodging_; }
    bool isAlive()        const { return hp_ > 0; }

    // ── Mutators ───────────────────────────────────────────────────────────
    void setDodging(bool v)            { dodging_ = v; }
    void restoreStamina(int amount);
    void consumeStamina(int amount);
    void heal(int amount);
    bool hasEnoughStamina(int c) const { return stamina_ >= c; }

    // ── Combat ─────────────────────────────────────────────────────────────
    /// Roll base damage with ±20 % random variance.
    virtual int  dealDamage() const;

    /// Apply incoming raw damage, accounting for defense and dodge.
    /// Returns actual damage dealt.
    virtual int  takeDamage(int rawDamage);

    // ── Polymorphic identity ───────────────────────────────────────────────
    virtual std::string getAsciiArt()  const = 0;
    virtual std::string getColorCode() const = 0;
};

} // namespace emberforge
