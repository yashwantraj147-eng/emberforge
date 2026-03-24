// ============================================================================
//  Emberforge — Common Types, Constants & ANSI Escape Codes
//  Copyright (c) 2026 Emberforge Contributors — MIT License
// ============================================================================
#pragma once

#include <string>

namespace emberforge {

// ── ANSI escape sequences for rich terminal rendering ──────────────────────
namespace ansi {
    // Reset
    constexpr const char* RESET      = "\033[0m";

    // Styles
    constexpr const char* BOLD       = "\033[1m";
    constexpr const char* DIM        = "\033[2m";
    constexpr const char* ITALIC     = "\033[3m";
    constexpr const char* UNDERLINE  = "\033[4m";
    constexpr const char* BLINK      = "\033[5m";

    // Foreground colors
    constexpr const char* BLACK      = "\033[30m";
    constexpr const char* RED        = "\033[31m";
    constexpr const char* GREEN      = "\033[32m";
    constexpr const char* YELLOW     = "\033[33m";
    constexpr const char* BLUE       = "\033[34m";
    constexpr const char* MAGENTA    = "\033[35m";
    constexpr const char* CYAN       = "\033[36m";
    constexpr const char* WHITE      = "\033[37m";

    // Bright foreground
    constexpr const char* BR_RED     = "\033[91m";
    constexpr const char* BR_GREEN   = "\033[92m";
    constexpr const char* BR_YELLOW  = "\033[93m";
    constexpr const char* BR_BLUE    = "\033[94m";
    constexpr const char* BR_MAGENTA = "\033[95m";
    constexpr const char* BR_CYAN    = "\033[96m";
    constexpr const char* BR_WHITE   = "\033[97m";

    // Background colors
    constexpr const char* BG_RED     = "\033[41m";
    constexpr const char* BG_GREEN   = "\033[42m";
    constexpr const char* BG_YELLOW  = "\033[43m";
    constexpr const char* BG_BLUE    = "\033[44m";
    constexpr const char* BG_MAGENTA = "\033[45m";
    constexpr const char* BG_BLACK   = "\033[40m";
}

// ── Combat action enum ─────────────────────────────────────────────────────
enum class CombatAction {
    Strike,
    HeavyStrike,
    Dodge,
    Rest,
    UsePotion
};

// ── Enemy archetypes ───────────────────────────────────────────────────────
enum class EnemyType {
    Skeleton,
    DarkKnight,
    Wraith,
    FireDrake,
    LichKing      // Final boss
};

// ── Stamina cost table ─────────────────────────────────────────────────────
namespace cost {
    constexpr int STRIKE       = 15;
    constexpr int HEAVY_STRIKE = 30;
    constexpr int DODGE        = 10;
    constexpr int REST_GAIN    = 25;
    constexpr int POTION_HEAL  = 40;
}

} // namespace emberforge
