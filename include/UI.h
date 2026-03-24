// ============================================================================
//  Emberforge — Terminal UI Renderer
//  All visual output: ANSI-colored bars, box-drawing menus, ASCII art, etc.
// ============================================================================
#pragma once

#include <string>

namespace emberforge {

// Forward declarations (avoid circular includes)
class Player;
class Enemy;

class UI {
public:
    // ── Platform ───────────────────────────────────────────────────────────
    static void enableAnsiColors();
    static void clearScreen();
    static void pause(const std::string& msg = "");
    static void sleepMs(int ms);

    // ── Title & menus ──────────────────────────────────────────────────────
    static void printTitleScreen();
    static void printMainMenu();
    static int  getChoice(int min, int max);
    static std::string getPlayerName();

    // ── Combat HUD ─────────────────────────────────────────────────────────
    static void printCombatHeader(const Player& p, const Enemy& e);
    static void printHealthBar(const std::string& label, int cur, int max,
                               const std::string& color);
    static void printStaminaBar(const std::string& label, int cur, int max);
    static void printCombatMenu(const Player& p);
    static void printCombatAction(const std::string& msg,
                                  const std::string& color = "");
    static void printDamageReport(const std::string& attacker,
                                  const std::string& target,
                                  int damage, bool dodged);

    // ── Info screens ───────────────────────────────────────────────────────
    static void printPlayerStats(const Player& p);
    static void printEncounterIntro(const Enemy& e);
    static void printVictory(int xp, int gold, bool potionDrop);
    static void printDefeat();
    static void printLevelUp(const Player& p);
    static void printCampfire();
    static void printGameComplete(const Player& p);

    // ── Drawing helpers ────────────────────────────────────────────────────
    static void printSeparator(int width = 60, const std::string& color = "");
    static void printBoxTop(int width = 60, const std::string& color = "");
    static void printBoxBottom(int width = 60, const std::string& color = "");
    static void printBoxLine(const std::string& text, int width = 60,
                             const std::string& color = "");
};

} // namespace emberforge
