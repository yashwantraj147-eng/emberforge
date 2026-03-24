// ============================================================================
//  Emberforge — Combat Engine
//  Orchestrates the turn-based combat loop between Player and Enemy.
// ============================================================================
#pragma once

#include "Player.h"
#include "Enemy.h"
#include <memory>

namespace emberforge {

class CombatEngine {
    Player&                 player_;
    std::unique_ptr<Enemy>  enemy_;
    int                     turnCount_;

    void playerTurn();
    void enemyTurn();
    void displayCombatState() const;
    bool isCombatOver() const;

public:
    CombatEngine(Player& player, std::unique_ptr<Enemy> enemy);

    /// Run the full combat encounter.
    /// @return true if the player survived.
    bool engage();
};

} // namespace emberforge
