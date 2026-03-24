// ============================================================================
//  Emberforge — Enemy Hierarchy
//  Five distinct enemy archetypes, each with unique AI and ASCII art.
//  Demonstrates polymorphism through the factory + virtual chooseAction().
// ============================================================================
#pragma once

#include "Character.h"
#include "Types.h"
#include <memory>
#include <string>

namespace emberforge {

// ── Abstract enemy base ────────────────────────────────────────────────────
class Enemy : public Character {
protected:
    EnemyType   type_;
    int         xpReward_;
    int         goldReward_;
    std::string description_;

public:
    Enemy(const std::string& name, int hp, int stamina,
          int attack, int defense, int level,
          EnemyType type, int xpReward, int goldReward,
          const std::string& desc);

    // Factory — select + scale an enemy to the player's level.
    static std::unique_ptr<Enemy> create(EnemyType type, int playerLevel);

    // Observers
    EnemyType          getType()        const { return type_; }
    int                getXpReward()    const { return xpReward_; }
    int                getGoldReward()  const { return goldReward_; }
    const std::string& getDescription() const { return description_; }

    // Each archetype implements its own AI decision tree.
    virtual CombatAction chooseAction(int selfHpPercent) = 0;

    std::string getColorCode() const override;
};

// ── Skeleton ───────────────────────────────────────────────────────────────
//  Aggressive but fragile; favors relentless strikes.
class Skeleton : public Enemy {
public:
    explicit Skeleton(int playerLevel);
    CombatAction chooseAction(int selfHpPercent) override;
    std::string  getAsciiArt() const override;
};

// ── Dark Knight ────────────────────────────────────────────────────────────
//  A balanced warrior; mixes heavy attacks with defensive dodges.
class DarkKnight : public Enemy {
public:
    explicit DarkKnight(int playerLevel);
    CombatAction chooseAction(int selfHpPercent) override;
    std::string  getAsciiArt() const override;
};

// ── Wraith ─────────────────────────────────────────────────────────────────
//  Evasive phantom; dodges frequently and strikes from the shadows.
class Wraith : public Enemy {
public:
    explicit Wraith(int playerLevel);
    CombatAction chooseAction(int selfHpPercent) override;
    std::string  getAsciiArt() const override;
};

// ── Fire Drake ─────────────────────────────────────────────────────────────
//  Devastating brute; relies on raw power and heavy flame attacks.
class FireDrake : public Enemy {
public:
    explicit FireDrake(int playerLevel);
    CombatAction chooseAction(int selfHpPercent) override;
    std::string  getAsciiArt() const override;
};

// ── Lich King (Boss) ───────────────────────────────────────────────────────
//  The final adversary; adaptive AI with high stats.
class LichKing : public Enemy {
public:
    explicit LichKing(int playerLevel);
    CombatAction chooseAction(int selfHpPercent) override;
    std::string  getAsciiArt() const override;
};

} // namespace emberforge
