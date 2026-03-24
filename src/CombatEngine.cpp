// ============================================================================
//  Emberforge — Combat Engine
//  Orchestrates a full turn-based encounter between Player and Enemy.
// ============================================================================
#include "CombatEngine.h"
#include "UI.h"

#include <random>

namespace emberforge {

static std::mt19937& rng() {
    static std::mt19937 gen(std::random_device{}());
    return gen;
}

CombatEngine::CombatEngine(Player& player, std::unique_ptr<Enemy> enemy)
    : player_(player), enemy_(std::move(enemy)), turnCount_(0)
{}

// ── Public entry point ─────────────────────────────────────────────────────
bool CombatEngine::engage() {
    UI::printEncounterIntro(*enemy_);
    UI::pause("  Press [Enter] to draw your weapon...");

    turnCount_ = 0;
    while (!isCombatOver()) {
        ++turnCount_;
        displayCombatState();

        playerTurn();
        if (!enemy_->isAlive()) break;

        UI::sleepMs(600);
        enemyTurn();
    }

    if (player_.isAlive()) {
        int xp   = enemy_->getXpReward();
        int gold = enemy_->getGoldReward();
        bool potionDrop = (std::uniform_int_distribution<>(1, 100)(rng()) <= 35);

        player_.gainXP(xp);
        player_.gainGold(gold);
        player_.incrementKills();
        if (potionDrop) player_.addPotion();

        UI::printVictory(xp, gold, potionDrop);
        if (player_.checkLevelUp()) {
            UI::printLevelUp(player_);
        }
        return true;
    }
    UI::printDefeat();
    return false;
}

// ── Display the current combat HUD ─────────────────────────────────────────
void CombatEngine::displayCombatState() const {
    UI::clearScreen();
    UI::printCombatHeader(player_, *enemy_);
}

bool CombatEngine::isCombatOver() const {
    return !player_.isAlive() || !enemy_->isAlive();
}

// ── Player's turn ──────────────────────────────────────────────────────────
void CombatEngine::playerTurn() {
    // Make sure dodge flag is cleared at the start of the player's turn
    player_.setDodging(false);

    UI::printCombatMenu(player_);
    int choice = UI::getChoice(1, 5);

    switch (choice) {
    case 1: { // Strike
        if (!player_.hasEnoughStamina(cost::STRIKE)) {
            UI::printCombatAction("  Not enough stamina! You stumble and waste your turn.",
                                  ansi::DIM);
            break;
        }
        int raw = player_.performStrike();
        int dmg = enemy_->takeDamage(raw);
        UI::printDamageReport(player_.getName(), enemy_->getName(), dmg,
                              enemy_->isDodging());
        break;
    }
    case 2: { // Heavy Strike
        if (!player_.hasEnoughStamina(cost::HEAVY_STRIKE)) {
            UI::printCombatAction("  Your arms are too heavy... you need to rest!",
                                  ansi::DIM);
            break;
        }
        int raw = player_.performHeavyStrike();
        int dmg = enemy_->takeDamage(raw);
        UI::printDamageReport(player_.getName(), enemy_->getName(), dmg,
                              enemy_->isDodging());
        break;
    }
    case 3: // Dodge
        if (!player_.hasEnoughStamina(cost::DODGE)) {
            UI::printCombatAction("  Too exhausted to dodge!", ansi::DIM);
            break;
        }
        player_.performDodge();
        UI::printCombatAction("  You brace and prepare to evade the next blow!",
                              ansi::BLUE);
        break;
    case 4: // Rest
        player_.performRest();
        UI::printCombatAction("  You catch your breath, recovering stamina...",
                              ansi::GREEN);
        break;
    case 5: // Potion
        if (player_.usePotion()) {
            UI::printCombatAction("  You drink a healing potion! HP restored.",
                                  ansi::BR_GREEN);
        } else {
            UI::printCombatAction("  No potions left!", ansi::RED);
        }
        break;
    }

    UI::sleepMs(800);
}

// ── Enemy's turn ───────────────────────────────────────────────────────────
void CombatEngine::enemyTurn() {
    if (!enemy_->isAlive()) return;

    enemy_->setDodging(false);

    int hpPct = (enemy_->getHp() * 100) / std::max(1, enemy_->getMaxHp());
    CombatAction action = enemy_->chooseAction(hpPct);

    switch (action) {
    case CombatAction::Strike: {
        if (!enemy_->hasEnoughStamina(cost::STRIKE)) {
            enemy_->restoreStamina(cost::REST_GAIN);
            UI::printCombatAction("  " + enemy_->getName() + " hesitates, catching its breath...",
                                  ansi::DIM);
            break;
        }
        enemy_->consumeStamina(cost::STRIKE);
        int raw = enemy_->dealDamage();
        int dmg = player_.takeDamage(raw);
        UI::printDamageReport(enemy_->getName(), player_.getName(), dmg,
                              player_.isDodging());
        break;
    }
    case CombatAction::HeavyStrike: {
        if (!enemy_->hasEnoughStamina(cost::HEAVY_STRIKE)) {
            enemy_->restoreStamina(cost::REST_GAIN);
            UI::printCombatAction("  " + enemy_->getName() + " gathers its strength...",
                                  ansi::DIM);
            break;
        }
        enemy_->consumeStamina(cost::HEAVY_STRIKE);
        int raw = static_cast<int>(enemy_->dealDamage() * 1.8);
        int dmg = player_.takeDamage(raw);
        UI::printCombatAction("  " + enemy_->getName() + " unleashes a devastating blow!",
                              ansi::BR_RED);
        UI::printDamageReport(enemy_->getName(), player_.getName(), dmg,
                              player_.isDodging());
        break;
    }
    case CombatAction::Dodge:
        if (!enemy_->hasEnoughStamina(cost::DODGE)) {
            enemy_->restoreStamina(cost::REST_GAIN);
            break;
        }
        enemy_->consumeStamina(cost::DODGE);
        enemy_->setDodging(true);
        UI::printCombatAction("  " + enemy_->getName() + " shifts into a defensive stance!",
                              ansi::MAGENTA);
        break;
    case CombatAction::Rest:
        enemy_->restoreStamina(cost::REST_GAIN);
        UI::printCombatAction("  " + enemy_->getName() + " steadies itself, recovering stamina...",
                              ansi::DIM);
        break;
    default:
        break;
    }

    UI::sleepMs(600);
}

} // namespace emberforge
