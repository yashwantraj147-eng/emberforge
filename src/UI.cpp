// ============================================================================
//  Emberforge — Terminal UI Renderer
//  ANSI-coloured bars, box-drawing frames, ASCII art, and cross-platform I/O.
// ============================================================================
#include "UI.h"
#include "Player.h"
#include "Enemy.h"
#include "Types.h"

#include <algorithm>
#include <iostream>
#include <sstream>
#include <string>
#include <iomanip>

#ifdef _WIN32
  #include <windows.h>
#else
  #include <unistd.h>
#endif

namespace emberforge {

// ============================================================================
//  Platform helpers
// ============================================================================
void UI::enableAnsiColors() {
#ifdef _WIN32
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hOut != INVALID_HANDLE_VALUE) {
        DWORD mode = 0;
        GetConsoleMode(hOut, &mode);
        mode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
        SetConsoleMode(hOut, mode);
    }
#endif
}

void UI::clearScreen() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

void UI::pause(const std::string& msg) {
    std::cout << ansi::DIM;
    if (msg.empty())
        std::cout << "\n  Press [Enter] to continue...";
    else
        std::cout << "\n" << msg;
    std::cout << ansi::RESET;
    std::cin.ignore(10000, '\n');
}

void UI::sleepMs(int ms) {
#ifdef _WIN32
    Sleep(static_cast<DWORD>(ms));
#else
    usleep(static_cast<useconds_t>(ms) * 1000);
#endif
}

// ============================================================================
//  Drawing primitives
// ============================================================================
void UI::printSeparator(int width, const std::string& color) {
    std::string c = color.empty() ? ansi::DIM : color;
    std::cout << c << "  ";
    for (int i = 0; i < width; ++i) std::cout << "\xe2\x94\x80"; // ─
    std::cout << ansi::RESET << "\n";
}

void UI::printBoxTop(int width, const std::string& color) {
    std::string c = color.empty() ? ansi::DIM : color;
    std::cout << c << "  \xe2\x95\x94"; // ╔
    for (int i = 0; i < width; ++i) std::cout << "\xe2\x95\x90"; // ═
    std::cout << "\xe2\x95\x97" << ansi::RESET << "\n"; // ╗
}

void UI::printBoxBottom(int width, const std::string& color) {
    std::string c = color.empty() ? ansi::DIM : color;
    std::cout << c << "  \xe2\x95\x9a"; // ╚
    for (int i = 0; i < width; ++i) std::cout << "\xe2\x95\x90"; // ═
    std::cout << "\xe2\x95\x9d" << ansi::RESET << "\n"; // ╝
}

void UI::printBoxLine(const std::string& text, int width,
                      const std::string& color) {
    std::string c = color.empty() ? ansi::DIM : color;
    // Strip ANSI for length calculation
    std::string plain;
    bool inEsc = false;
    for (char ch : text) {
        if (ch == '\033') { inEsc = true; continue; }
        if (inEsc) { if (ch == 'm') inEsc = false; continue; }
        plain += ch;
    }
    int pad = std::max(0, width - static_cast<int>(plain.size()));
    std::cout << c << "  \xe2\x95\x91 " << ansi::RESET  // ║
              << text << std::string(static_cast<size_t>(pad), ' ')
              << c << " \xe2\x95\x91" << ansi::RESET << "\n"; // ║
}

// ============================================================================
//  Title screen
// ============================================================================
void UI::printTitleScreen() {
    clearScreen();
    std::cout << "\n\n";

    // Fire-gradient ASCII title
    const char* lines[] = {
        "  _____ __  __ ____  _____ ____  _____ ___  ____   ____ _____",
        " | ____|  \\/  | __ )| ____|  _ \\|  ___/ _ \\|  _ \\ / ___| ____|",
        " |  _| | |\\/| |  _ \\|  _| | |_) | |_ | | | | |_) | |  _|  _|",
        " | |___| |  | | |_) | |___|  _ <|  _|| |_| |  _ <| |_| | |___",
        " |_____|_|  |_|____/|_____|_| \\_\\_|   \\___/|_| \\_\\\\____|_____|",
    };
    const char* fireColors[] = {
        ansi::BR_RED, ansi::RED, ansi::BR_YELLOW, ansi::YELLOW, ansi::BR_RED
    };

    for (int i = 0; i < 5; ++i) {
        std::cout << fireColors[i] << ansi::BOLD << lines[i]
                  << ansi::RESET << "\n";
    }

    std::cout << "\n";
    std::cout << ansi::DIM << ansi::ITALIC
              << "          ~ A Dark Fantasy Combat Engine ~\n"
              << ansi::RESET << "\n";

    printBoxTop(56, ansi::RED);
    printBoxLine(std::string(ansi::BR_YELLOW) + ansi::BOLD +
        "  \"In embers forged, in darkness tempered.\"" + ansi::RESET, 56, ansi::RED);
    printBoxLine("", 56, ansi::RED);
    printBoxLine(std::string(ansi::DIM) +
        "  Navigate treacherous encounters.  Manage your" + ansi::RESET, 56, ansi::RED);
    printBoxLine(std::string(ansi::DIM) +
        "  stamina.  Strike, dodge, and survive the dark." + ansi::RESET, 56, ansi::RED);
    printBoxLine(std::string(ansi::DIM) +
        "  Defeat the Lich King to reclaim the light." + ansi::RESET, 56, ansi::RED);
    printBoxBottom(56, ansi::RED);

    std::cout << "\n";
    std::cout << ansi::DIM
              << "  v1.0.0                    Built with Modern C++17\n"
              << ansi::RESET;
}

// ============================================================================
//  Main menu
// ============================================================================
void UI::printMainMenu() {
    std::cout << "\n";
    printBoxTop(40, ansi::CYAN);
    printBoxLine(std::string(ansi::BOLD) + ansi::BR_CYAN +
        "          MAIN  MENU" + ansi::RESET, 40, ansi::CYAN);
    printBoxLine("", 40, ansi::CYAN);
    printBoxLine(std::string(ansi::BR_WHITE) +
        "  [1]  New Adventure" + ansi::RESET, 40, ansi::CYAN);
    printBoxLine(std::string(ansi::BR_WHITE) +
        "  [2]  Quit" + ansi::RESET, 40, ansi::CYAN);
    printBoxBottom(40, ansi::CYAN);
    std::cout << "\n";
}

int UI::getChoice(int min, int max) {
    int choice = min;
    while (true) {
        std::cout << ansi::BR_YELLOW << "  > " << ansi::RESET;
        if (std::cin >> choice && choice >= min && choice <= max) {
            std::cin.ignore(10000, '\n');
            return choice;
        }
        std::cin.clear();
        std::cin.ignore(10000, '\n');
        std::cout << ansi::RED << "  Invalid choice. Try again." << ansi::RESET << "\n";
    }
}

std::string UI::getPlayerName() {
    std::cout << ansi::BR_CYAN << "\n  Enter your name, warrior: " << ansi::RESET;
    std::string name;
    std::getline(std::cin, name);
    if (name.empty()) name = "Ashen One";
    return name;
}

// ============================================================================
//  Health & stamina bars
// ============================================================================
void UI::printHealthBar(const std::string& label, int cur, int max,
                        const std::string& color) {
    constexpr int BAR_WIDTH = 25;
    double ratio = (max > 0) ? static_cast<double>(cur) / max : 0.0;
    int filled = static_cast<int>(ratio * BAR_WIDTH);

    // Colour shifts by percentage
    const char* barColor = ansi::BR_GREEN;
    if (ratio <= 0.25)      barColor = ansi::BR_RED;
    else if (ratio <= 0.55) barColor = ansi::BR_YELLOW;

    if (!color.empty()) barColor = color.c_str();

    std::cout << "  " << ansi::BOLD << label << ansi::RESET << " [";
    for (int i = 0; i < BAR_WIDTH; ++i) {
        if (i < filled)
            std::cout << barColor << "\xe2\x96\x88"; // █
        else
            std::cout << ansi::DIM << "\xe2\x96\x91"; // ░
    }
    std::cout << ansi::RESET << "] "
              << barColor << cur << "/" << max << ansi::RESET << "\n";
}

void UI::printStaminaBar(const std::string& label, int cur, int max) {
    printHealthBar(label, cur, max, ansi::BR_BLUE);
}

// ============================================================================
//  Combat HUD
// ============================================================================
void UI::printCombatHeader(const Player& p, const Enemy& e) {
    std::cout << "\n";
    printSeparator(58, ansi::RED);

    // Enemy side
    std::string art = e.getAsciiArt();
    std::cout << e.getColorCode() << art << ansi::RESET;
    std::cout << "\n";
    std::cout << "  " << e.getColorCode() << ansi::BOLD
              << e.getName() << ansi::RESET
              << ansi::DIM << "  [Lv." << e.getLevel() << "]" << ansi::RESET << "\n";
    printHealthBar(" HP ", e.getHp(), e.getMaxHp(), "");
    printStaminaBar(" ST ", e.getStamina(), e.getMaxStamina());

    std::cout << "\n";
    printSeparator(58, ansi::CYAN);

    // Player side
    std::cout << "  " << p.getColorCode() << ansi::BOLD
              << p.getName() << ansi::RESET
              << ansi::DIM << "  [Lv." << p.getLevel() << "]" << ansi::RESET << "\n";
    printHealthBar(" HP ", p.getHp(), p.getMaxHp(), "");
    printStaminaBar(" ST ", p.getStamina(), p.getMaxStamina());
    std::cout << "  "
              << ansi::BR_MAGENTA << "Potions: " << p.getPotions() << ansi::RESET
              << "   "
              << ansi::BR_YELLOW  << "Gold: " << p.getGold() << ansi::RESET
              << "\n";
    printSeparator(58, ansi::CYAN);
}

void UI::printCombatMenu(const Player& p) {
    std::cout << "\n";
    auto dim = [&](int req) {
        return p.hasEnoughStamina(req) ? ansi::BR_WHITE : ansi::DIM;
    };

    printBoxTop(40, ansi::YELLOW);
    printBoxLine(std::string(ansi::BOLD) + ansi::BR_YELLOW +
        "        YOUR ACTION" + ansi::RESET, 40, ansi::YELLOW);
    printBoxLine("", 40, ansi::YELLOW);

    // Strike
    {
        std::ostringstream os;
        os << dim(cost::STRIKE) << "  [1] Strike         "
           << ansi::DIM << "(" << cost::STRIKE << " ST)" << ansi::RESET;
        printBoxLine(os.str(), 40, ansi::YELLOW);
    }
    // Heavy Strike
    {
        std::ostringstream os;
        os << dim(cost::HEAVY_STRIKE) << "  [2] Heavy Strike   "
           << ansi::DIM << "(" << cost::HEAVY_STRIKE << " ST)" << ansi::RESET;
        printBoxLine(os.str(), 40, ansi::YELLOW);
    }
    // Dodge
    {
        std::ostringstream os;
        os << dim(cost::DODGE) << "  [3] Dodge          "
           << ansi::DIM << "(" << cost::DODGE << " ST)" << ansi::RESET;
        printBoxLine(os.str(), 40, ansi::YELLOW);
    }
    // Rest
    {
        std::ostringstream os;
        os << ansi::GREEN << "  [4] Rest           "
           << ansi::DIM << "(+" << cost::REST_GAIN << " ST)" << ansi::RESET;
        printBoxLine(os.str(), 40, ansi::YELLOW);
    }
    // Potion
    {
        std::ostringstream os;
        const char* potColor = (p.getPotions() > 0) ? ansi::BR_MAGENTA : ansi::DIM;
        os << potColor << "  [5] Use Potion     "
           << ansi::DIM << "(x" << p.getPotions() << ")" << ansi::RESET;
        printBoxLine(os.str(), 40, ansi::YELLOW);
    }

    printBoxBottom(40, ansi::YELLOW);
}

void UI::printCombatAction(const std::string& msg, const std::string& color) {
    std::cout << "\n" << (color.empty() ? ansi::BR_WHITE : color.c_str())
              << msg << ansi::RESET << "\n";
}

void UI::printDamageReport(const std::string& attacker, const std::string& target,
                           int damage, bool dodged) {
    if (dodged) {
        std::cout << "\n  " << ansi::BR_BLUE << target
                  << " partially evades! "
                  << ansi::BR_RED << "-" << damage << " HP"
                  << ansi::DIM << " (reduced)" << ansi::RESET << "\n";
    } else {
        std::cout << "\n  " << ansi::BR_WHITE << attacker << " strikes "
                  << target << "!  "
                  << ansi::BR_RED << ansi::BOLD << "-" << damage << " HP"
                  << ansi::RESET << "\n";
    }
}

// ============================================================================
//  Info screens
// ============================================================================
void UI::printPlayerStats(const Player& p) {
    clearScreen();
    std::cout << "\n";
    printBoxTop(50, ansi::CYAN);
    printBoxLine(std::string(ansi::BOLD) + ansi::BR_CYAN +
        "           CHARACTER STATS" + ansi::RESET, 50, ansi::CYAN);
    printBoxLine("", 50, ansi::CYAN);

    auto stat = [](const std::string& label, int val) {
        std::ostringstream os;
        os << ansi::BR_WHITE << "  " << std::left << std::setw(16) << label
           << ansi::BR_YELLOW << val << ansi::RESET;
        return os.str();
    };

    printBoxLine(stat("Name:", 0).replace(
        stat("Name:", 0).find("0"), 1, p.getName()), 50, ansi::CYAN);
    printBoxLine(stat("Level:", p.getLevel()), 50, ansi::CYAN);
    printBoxLine(stat("HP:", p.getHp()) + " / " +
                 std::to_string(p.getMaxHp()), 50, ansi::CYAN);
    printBoxLine(stat("Stamina:", p.getStamina()) + " / " +
                 std::to_string(p.getMaxStamina()), 50, ansi::CYAN);
    printBoxLine(stat("Attack:", p.getAttack()), 50, ansi::CYAN);
    printBoxLine(stat("Defense:", p.getDefense()), 50, ansi::CYAN);
    printBoxLine("", 50, ansi::CYAN);

    {
        std::ostringstream os;
        os << ansi::BR_YELLOW << "  XP: " << p.getXp() << " / " << p.getXpToNext()
           << "    " << ansi::BR_MAGENTA << "Potions: " << p.getPotions()
           << "    " << ansi::BR_YELLOW << "Gold: " << p.getGold()
           << "    " << ansi::RED << "Kills: " << p.getKills() << ansi::RESET;
        printBoxLine(os.str(), 50, ansi::CYAN);
    }

    printBoxBottom(50, ansi::CYAN);
    pause();
}

void UI::printEncounterIntro(const Enemy& e) {
    clearScreen();
    std::cout << "\n";
    printSeparator(58, ansi::RED);
    std::cout << "\n"
              << ansi::BOLD << ansi::BR_RED
              << "    !! ENCOUNTER !!\n\n"
              << ansi::RESET;

    std::cout << e.getColorCode() << e.getAsciiArt() << ansi::RESET << "\n";

    std::cout << "  " << e.getColorCode() << ansi::BOLD
              << e.getName() << ansi::RESET
              << ansi::DIM << "  [Lv." << e.getLevel() << "]\n\n" << ansi::RESET;

    std::cout << ansi::ITALIC << ansi::DIM
              << "  " << e.getDescription()
              << ansi::RESET << "\n\n";
    printSeparator(58, ansi::RED);
}

void UI::printVictory(int xp, int gold, bool potionDrop) {
    std::cout << "\n";
    printBoxTop(45, ansi::GREEN);
    printBoxLine(std::string(ansi::BOLD) + ansi::BR_GREEN +
        "           VICTORY!" + ansi::RESET, 45, ansi::GREEN);
    printBoxLine("", 45, ansi::GREEN);
    {
        std::ostringstream os;
        os << ansi::BR_YELLOW << "  + " << xp << " XP" << ansi::RESET;
        printBoxLine(os.str(), 45, ansi::GREEN);
    }
    {
        std::ostringstream os;
        os << ansi::BR_YELLOW << "  + " << gold << " Gold" << ansi::RESET;
        printBoxLine(os.str(), 45, ansi::GREEN);
    }
    if (potionDrop) {
        printBoxLine(std::string(ansi::BR_MAGENTA) +
            "  + 1 Healing Potion (dropped!)" + ansi::RESET, 45, ansi::GREEN);
    }
    printBoxBottom(45, ansi::GREEN);
    pause();
}

void UI::printDefeat() {
    std::cout << "\n\n";
    std::cout << ansi::BOLD << ansi::BR_RED
              << "    ============================\n"
              << "         Y O U   D I E D\n"
              << "    ============================\n"
              << ansi::RESET << "\n";

    std::cout << ansi::DIM << ansi::ITALIC
              << "  The darkness claims another soul.\n"
              << "  Your embers fade to ash...\n"
              << ansi::RESET;
    pause();
}

void UI::printLevelUp(const Player& p) {
    std::cout << "\n";
    printBoxTop(45, ansi::BR_YELLOW);
    printBoxLine(std::string(ansi::BOLD) + ansi::BR_YELLOW +
        "         LEVEL UP!" + ansi::RESET, 45, ansi::BR_YELLOW);
    printBoxLine("", 45, ansi::BR_YELLOW);
    {
        std::ostringstream os;
        os << ansi::BR_WHITE << "  Level " << (p.getLevel())
           << " reached!" << ansi::RESET;
        printBoxLine(os.str(), 45, ansi::BR_YELLOW);
    }
    {
        std::ostringstream os;
        os << ansi::BR_GREEN << "  HP +" << 15
           << "  STA +" << 10
           << "  ATK +" << 4
           << "  DEF +" << 2 << ansi::RESET;
        printBoxLine(os.str(), 45, ansi::BR_YELLOW);
    }
    printBoxLine(std::string(ansi::BR_GREEN) +
        "  Fully healed!" + ansi::RESET, 45, ansi::BR_YELLOW);
    printBoxBottom(45, ansi::BR_YELLOW);
    pause();
}

void UI::printCampfire() {
    clearScreen();
    std::cout << "\n";

    std::cout << ansi::BR_YELLOW << R"(
              (
           (  )  )
            ) ( (
       _____________
       \           /
        \  ,    , /
         \( @  @ )/
          \  /\  /
           \/  \/
    )" << ansi::RESET;

    std::cout << "\n";
    printBoxTop(50, ansi::BR_YELLOW);
    printBoxLine(std::string(ansi::BOLD) + ansi::BR_YELLOW +
        "            CAMPFIRE" + ansi::RESET, 50, ansi::BR_YELLOW);
    printBoxLine("", 50, ansi::BR_YELLOW);
    printBoxLine(std::string(ansi::DIM) +
        "  The campfire crackles warmly.  Your wounds" + ansi::RESET, 50, ansi::BR_YELLOW);
    printBoxLine(std::string(ansi::DIM) +
        "  mend and strength returns to your limbs." + ansi::RESET, 50, ansi::BR_YELLOW);
    printBoxLine("", 50, ansi::BR_YELLOW);
    printBoxLine(std::string(ansi::BR_GREEN) +
        "  HP and Stamina fully restored." + ansi::RESET, 50, ansi::BR_YELLOW);
    printBoxBottom(50, ansi::BR_YELLOW);
    pause();
}

void UI::printGameComplete(const Player& p) {
    clearScreen();
    std::cout << "\n\n";
    std::cout << ansi::BOLD << ansi::BR_YELLOW
              << R"(
     _____ ___ _   _    _    _
    |  ___|_ _| \ | |  / \  | |
    | |_   | ||  \| | / _ \ | |
    |  _|  | || |\  |/ ___ \| |___
    |_|   |___|_| \_/_/   \_\_____|

    )" << ansi::RESET;

    printBoxTop(54, ansi::BR_YELLOW);
    printBoxLine(std::string(ansi::BOLD) + ansi::BR_YELLOW +
        "          THE LICH KING FALLS!" + ansi::RESET, 54, ansi::BR_YELLOW);
    printBoxLine("", 54, ansi::BR_YELLOW);
    printBoxLine(std::string(ansi::DIM) + ansi::ITALIC +
        "  Light floods back into the world as the crown" + ansi::RESET,
        54, ansi::BR_YELLOW);
    printBoxLine(std::string(ansi::DIM) + ansi::ITALIC +
        "  of shattered souls cracks and dissolves. The" + ansi::RESET,
        54, ansi::BR_YELLOW);
    printBoxLine(std::string(ansi::DIM) + ansi::ITALIC +
        "  embers you forged your will in now blaze as a" + ansi::RESET,
        54, ansi::BR_YELLOW);
    printBoxLine(std::string(ansi::DIM) + ansi::ITALIC +
        "  beacon for all who follow." + ansi::RESET,
        54, ansi::BR_YELLOW);
    printBoxLine("", 54, ansi::BR_YELLOW);
    {
        std::ostringstream os;
        os << ansi::BR_CYAN << "  Hero: " << p.getName()
           << ansi::DIM << "  |  "
           << ansi::BR_YELLOW << "Level " << p.getLevel()
           << ansi::DIM << "  |  "
           << ansi::RED << "Kills: " << p.getKills()
           << ansi::DIM << "  |  "
           << ansi::BR_YELLOW << "Gold: " << p.getGold()
           << ansi::RESET;
        printBoxLine(os.str(), 54, ansi::BR_YELLOW);
    }
    printBoxBottom(54, ansi::BR_YELLOW);

    std::cout << "\n" << ansi::DIM
              << "  Thank you for playing Emberforge.\n"
              << ansi::RESET;
    pause();
}

} // namespace emberforge
