// ============================================================================
//  Emberforge — Game Engine
//  Controls the adventure loop: menus → encounters → campfire → boss.
// ============================================================================
#include "GameEngine.h"
#include "CombatEngine.h"
#include "Enemy.h"
#include "UI.h"

#include <random>

namespace emberforge {

static std::mt19937& rng() {
    static std::mt19937 gen(std::random_device{}());
    return gen;
}

GameEngine::GameEngine()
    : player_(nullptr)
    , encounterNumber_(0)
    , maxEncounters_(7)       // 6 regular fights + 1 boss
    , running_(true)
    , gameWon_(false)
{}

// ============================================================================
//  Public entry point
// ============================================================================
void GameEngine::run() {
    UI::printTitleScreen();
    showMainMenu();

    if (!running_) return;

    characterCreation();
    gameLoop();

    if (gameWon_) {
        UI::printGameComplete(*player_);
    } else if (!player_->isAlive()) {
        handleGameOver();
    }
}

// ============================================================================
//  Menus
// ============================================================================
void GameEngine::showMainMenu() {
    UI::printMainMenu();
    int choice = UI::getChoice(1, 2);
    if (choice == 2) {
        running_ = false;
    }
}

void GameEngine::characterCreation() {
    std::string name = UI::getPlayerName();
    player_ = std::make_unique<Player>(name);

    UI::clearScreen();
    std::cout << "\n";
    UI::printBoxTop(50, ansi::CYAN);
    UI::printBoxLine(std::string(ansi::BOLD) + ansi::BR_CYAN +
        "         YOUR JOURNEY BEGINS" + ansi::RESET, 50, ansi::CYAN);
    UI::printBoxLine("", 50, ansi::CYAN);
    UI::printBoxLine(std::string(ansi::DIM) +
        "  The gates of the Ashen Citadel groan open." + ansi::RESET,
        50, ansi::CYAN);
    UI::printBoxLine(std::string(ansi::DIM) +
        "  Darkness spills from within like black water." + ansi::RESET,
        50, ansi::CYAN);
    UI::printBoxLine(std::string(ansi::DIM) +
        "  You tighten your grip and step forward." + ansi::RESET,
        50, ansi::CYAN);
    UI::printBoxLine("", 50, ansi::CYAN);

    std::ostringstream os;
    os << ansi::BR_CYAN << "  " << player_->getName()
       << ansi::DIM << "  —  Level " << player_->getLevel()
       << "  |  HP " << player_->getHp()
       << "  |  ATK " << player_->getAttack()
       << "  |  DEF " << player_->getDefense()
       << ansi::RESET;
    UI::printBoxLine(os.str(), 50, ansi::CYAN);
    UI::printBoxBottom(50, ansi::CYAN);
    UI::pause();
}

// ============================================================================
//  Core game loop
// ============================================================================
void GameEngine::gameLoop() {
    while (running_ && player_->isAlive() && !gameWon_) {
        exploration();
    }
}

void GameEngine::exploration() {
    UI::clearScreen();
    std::cout << "\n";

    // Flavour text for the corridor
    static const char* flavour[] = {
        "Damp stone corridors stretch into the void ahead...",
        "Distant echoes of dragging chains set your teeth on edge...",
        "The torches on the walls flicker and die, then reignite...",
        "An acrid smell of brimstone chokes the stale air...",
        "Ancient runes on the floor pulse with a sickly glow...",
        "Water drips in rhythm, counting down something terrible...",
        "The walls seem to breathe, contracting in the dark..."
    };
    int idx = std::uniform_int_distribution<>(
        0, static_cast<int>(std::size(flavour)) - 1)(rng());

    UI::printSeparator(58, ansi::DIM);
    std::cout << "\n  " << ansi::ITALIC << ansi::DIM
              << flavour[idx] << ansi::RESET << "\n\n";

    std::cout << "  "
              << ansi::DIM << "Encounter " << (encounterNumber_ + 1)
              << " / " << maxEncounters_ << ansi::RESET << "\n\n";

    UI::printBoxTop(40, ansi::CYAN);
    UI::printBoxLine(std::string(ansi::BR_WHITE) +
        "  [1] Advance deeper" + ansi::RESET, 40, ansi::CYAN);
    UI::printBoxLine(std::string(ansi::BR_GREEN) +
        "  [2] Rest at campfire" + ansi::RESET, 40, ansi::CYAN);
    UI::printBoxLine(std::string(ansi::BR_CYAN) +
        "  [3] View stats" + ansi::RESET, 40, ansi::CYAN);
    UI::printBoxLine(std::string(ansi::RED) +
        "  [4] Flee (quit)" + ansi::RESET, 40, ansi::CYAN);
    UI::printBoxBottom(40, ansi::CYAN);

    int choice = UI::getChoice(1, 4);

    switch (choice) {
    case 1: encounter();  break;
    case 2: campfire();   break;
    case 3: showStats();  break;
    case 4:
        running_ = false;
        std::cout << ansi::DIM
                  << "\n  You turn back, the darkness claiming victory...\n"
                  << ansi::RESET;
        UI::pause();
        break;
    }
}

// ============================================================================
//  Encounter
// ============================================================================
void GameEngine::encounter() {
    ++encounterNumber_;

    // Final encounter = boss
    EnemyType type;
    if (encounterNumber_ >= maxEncounters_) {
        type = EnemyType::LichKing;
    } else {
        type = randomEnemyType();
    }

    auto enemy = Enemy::create(type, player_->getLevel());
    CombatEngine combat(*player_, std::move(enemy));

    bool won = combat.engage();

    if (won && type == EnemyType::LichKing) {
        gameWon_ = true;
    }
    if (!player_->isAlive()) {
        running_ = false;
    }
}

void GameEngine::campfire() {
    player_->restAtCampfire();
    UI::printCampfire();
}

void GameEngine::showStats() {
    UI::printPlayerStats(*player_);
}

void GameEngine::handleGameOver() {
    UI::clearScreen();
    UI::printDefeat();

    std::cout << "\n";
    UI::printBoxTop(45, ansi::RED);
    UI::printBoxLine(std::string(ansi::BR_RED) + ansi::BOLD +
        "           GAME OVER" + ansi::RESET, 45, ansi::RED);
    UI::printBoxLine("", 45, ansi::RED);
    {
        std::ostringstream os;
        os << ansi::DIM << "  " << player_->getName()
           << " fell at Level " << player_->getLevel()
           << " with " << player_->getKills() << " kills." << ansi::RESET;
        UI::printBoxLine(os.str(), 45, ansi::RED);
    }
    UI::printBoxBottom(45, ansi::RED);
    UI::pause();
}

// ============================================================================
//  Utility
// ============================================================================
EnemyType GameEngine::randomEnemyType() const {
    // Weight system: Skeleton and Wraith common early, tougher foes later
    int r = std::uniform_int_distribution<>(1, 100)(rng());

    if (encounterNumber_ <= 2) {
        // Early game: mostly easy enemies
        if (r <= 50) return EnemyType::Skeleton;
        if (r <= 80) return EnemyType::Wraith;
        return EnemyType::DarkKnight;
    }
    if (encounterNumber_ <= 4) {
        // Mid game
        if (r <= 25) return EnemyType::Skeleton;
        if (r <= 50) return EnemyType::Wraith;
        if (r <= 80) return EnemyType::DarkKnight;
        return EnemyType::FireDrake;
    }
    // Late game
    if (r <= 15) return EnemyType::Skeleton;
    if (r <= 35) return EnemyType::Wraith;
    if (r <= 60) return EnemyType::DarkKnight;
    return EnemyType::FireDrake;
}

} // namespace emberforge
