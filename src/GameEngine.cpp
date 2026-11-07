// ============================================================================
//  Emberforge — Game Engine
//  Controls the adventure loop: menus → encounters → campfire → boss.
// ============================================================================
#include "GameEngine.h"
#include "CombatEngine.h"
#include "Enemy.h"
#include "Random.h"
#include "UI.h"

#include <iostream>
#include <sstream>

namespace emberforge {

GameEngine::GameEngine()
    : player_(nullptr)
    , encounterNumber_(0)
    , maxEncounters_(7)       // 6 regular fights + 1 boss
    , campfireSupplies_(2)    // 2 campfire rests per adventure
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
    PlayerClass pClass = UI::getPlayerClassChoice();
    player_ = std::make_unique<Player>(name, pClass);

    UI::clearScreen();
    std::cout << "\n";
    std::cout << player_->getColorCode() << player_->getAsciiArt() << ansi::RESET << "\n";
    UI::printBoxTop(52, ansi::CYAN);
    UI::printBoxLine(std::string(ansi::BOLD) + ansi::BR_CYAN +
        "          YOUR JOURNEY BEGINS" + ansi::RESET, 52, ansi::CYAN);
    UI::printBoxLine("", 52, ansi::CYAN);
    UI::printBoxLine(std::string(ansi::DIM) +
        "  The gates of the Ashen Citadel groan open." + ansi::RESET,
        52, ansi::CYAN);
    UI::printBoxLine(std::string(ansi::DIM) +
        "  Darkness spills from within like black water." + ansi::RESET,
        52, ansi::CYAN);
    UI::printBoxLine(std::string(ansi::DIM) +
        "  You tighten your grip and step forward." + ansi::RESET,
        52, ansi::CYAN);
    UI::printBoxLine("", 52, ansi::CYAN);

    std::ostringstream os;
    os << ansi::BR_CYAN << "  " << player_->getName()
       << " [" << player_->getClassName() << "]"
       << ansi::DIM << "  —  HP " << player_->getHp()
       << " | ATK " << player_->getAttack()
       << " | DEF " << player_->getDefense()
       << ansi::RESET;
    UI::printBoxLine(os.str(), 52, ansi::CYAN);
    UI::printBoxBottom(52, ansi::CYAN);
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
    int idx = Random::get(0, static_cast<int>(std::size(flavour)) - 1);

    UI::printSeparator(58, ansi::DIM);
    std::cout << "\n  " << ansi::ITALIC << ansi::DIM
              << flavour[idx] << ansi::RESET << "\n\n";

    std::cout << "  "
              << ansi::DIM << "Encounter " << (encounterNumber_ + 1)
              << " / " << maxEncounters_ << ansi::RESET << "\n\n";

    UI::printBoxTop(45, ansi::CYAN);
    UI::printBoxLine(std::string(ansi::BR_WHITE) +
        "  [1] Advance deeper" + ansi::RESET, 45, ansi::CYAN);
    {
        std::ostringstream os;
        const char* cColor = (campfireSupplies_ > 0) ? ansi::BR_GREEN : ansi::DIM;
        os << cColor << "  [2] Rest at campfire (" << campfireSupplies_ << " left)" << ansi::RESET;
        UI::printBoxLine(os.str(), 45, ansi::CYAN);
    }
    UI::printBoxLine(std::string(ansi::BR_YELLOW) +
        "  [3] Citadel trader" + ansi::RESET, 45, ansi::CYAN);
    UI::printBoxLine(std::string(ansi::BR_CYAN) +
        "  [4] View stats" + ansi::RESET, 45, ansi::CYAN);
    UI::printBoxLine(std::string(ansi::RED) +
        "  [5] Flee (quit)" + ansi::RESET, 45, ansi::CYAN);
    UI::printBoxBottom(45, ansi::CYAN);

    int choice = UI::getChoice(1, 5);

    switch (choice) {
    case 1: encounter();  break;
    case 2: campfire();   break;
    case 3: merchant();   break;
    case 4: showStats();  break;
    case 5:
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
void GameEngine::encounter(bool isAmbush) {
    if (!isAmbush) {
        ++encounterNumber_;
    }

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
    if (campfireSupplies_ <= 0) {
        std::cout << "\n" << ansi::RED << "  You have no campfire supplies left! The ashes are cold." << ansi::RESET << "\n";
        UI::pause();
        return;
    }

    --campfireSupplies_;
    player_->restAtCampfire();
    UI::printCampfire(campfireSupplies_);

    // Strategic trade-off from README: "Heal between encounters at the risk of missing loot"
    if (encounterNumber_ < maxEncounters_ - 1) {
        ++encounterNumber_;
        std::cout << "\n" << ansi::DIM << "  Time slipped away while resting. You advanced deeper into the citadel..." << ansi::RESET << "\n";
        UI::pause();
    }

    // 25% chance of an ambush during rest!
    if (Random::chance(25) && encounterNumber_ < maxEncounters_) {
        std::cout << "\n" << ansi::BOLD << ansi::BR_RED
                  << "  !! AMBUSH !! A creature stalks from the dark while you gather your gear!\n"
                  << ansi::RESET;
        UI::pause("  Press [Enter] to defend yourself!");
        encounter(/*isAmbush=*/true);
    }
}

void GameEngine::merchant() {
    bool inShop = true;
    while (inShop) {
        UI::printMerchantMenu(*player_);
        int choice = UI::getChoice(1, 6);

        switch (choice) {
        case 1: { // Potion (25 gold)
            if (player_->spendGold(25)) {
                player_->addPotion(1);
                UI::printMerchantMessage("  You purchased a Healing Potion! (+1 Potion)", ansi::BR_GREEN);
            } else {
                UI::printMerchantMessage("  Not enough gold! The trader scoffs.", ansi::RED);
            }
            UI::sleepMs(700);
            break;
        }
        case 2: { // Whetstone (+3 ATK) (45 gold)
            if (player_->spendGold(45)) {
                player_->upgradeAttack(3);
                UI::printMerchantMessage("  Your blade gleams with newfound edge! (+3 ATK)", ansi::BR_GREEN);
            } else {
                UI::printMerchantMessage("  Not enough gold for the whetstone!", ansi::RED);
            }
            UI::sleepMs(700);
            break;
        }
        case 3: { // Iron Plating (+2 DEF) (45 gold)
            if (player_->spendGold(45)) {
                player_->upgradeDefense(2);
                UI::printMerchantMessage("  Armor reinforced with blackened iron! (+2 DEF)", ansi::BR_GREEN);
            } else {
                UI::printMerchantMessage("  Not enough gold for the plating!", ansi::RED);
            }
            UI::sleepMs(700);
            break;
        }
        case 4: { // Stamina Elixir (+15 Max ST) (50 gold)
            if (player_->spendGold(50)) {
                player_->upgradeMaxStamina(15);
                UI::printMerchantMessage("  A surge of vigor courses through your veins! (+15 Max Stamina)", ansi::BR_GREEN);
            } else {
                UI::printMerchantMessage("  Not enough gold for the elixir!", ansi::RED);
            }
            UI::sleepMs(700);
            break;
        }
        case 5: { // Life Crystal (+20 Max HP) (50 gold)
            if (player_->spendGold(50)) {
                player_->upgradeMaxHp(20);
                UI::printMerchantMessage("  The crystal dissolves, fortifying your soul! (+20 Max HP)", ansi::BR_GREEN);
            } else {
                UI::printMerchantMessage("  Not enough gold for the crystal!", ansi::RED);
            }
            UI::sleepMs(700);
            break;
        }
        case 6: // Leave shop
            inShop = false;
            break;
        }
    }
}

void GameEngine::showStats() {
    UI::printPlayerStats(*player_);
}

void GameEngine::handleGameOver() {
    UI::clearScreen();

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
    int r = Random::get(1, 100);

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
