// ============================================================================
//  Emberforge — Game Engine
//  Top-level controller: menus, exploration loop, encounters, progression.
// ============================================================================
#pragma once

#include "Player.h"
#include "Types.h"
#include <memory>

namespace emberforge {

class GameEngine {
    std::unique_ptr<Player> player_;
    int  encounterNumber_;
    int  maxEncounters_;       // boss spawns on the final encounter
    bool running_;
    bool gameWon_;

    void showMainMenu();
    void characterCreation();
    void gameLoop();
    void exploration();
    void encounter();
    void campfire();
    void showStats();
    void handleGameOver();

    /// Pick a random enemy type appropriate for the current encounter.
    EnemyType randomEnemyType() const;

public:
    GameEngine();
    void run();
};

} // namespace emberforge
