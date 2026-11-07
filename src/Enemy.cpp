// ============================================================================
//  Emberforge — Enemy Implementations & Factory
//  Five archetypes with scaling stats and unique behavioural AI.
// ============================================================================
#include "Enemy.h"
#include "Random.h"

namespace emberforge {

// ── Enemy base ─────────────────────────────────────────────────────────────
Enemy::Enemy(const std::string& name, int hp, int stamina,
             int attack, int defense, int level,
             EnemyType type, int xpReward, int goldReward,
             const std::string& desc)
    : Character(name, hp, stamina, attack, defense, level)
    , type_(type), xpReward_(xpReward), goldReward_(goldReward)
    , description_(desc)
{}

int Enemy::executeSpecialAction(Character& target) {
    int raw = static_cast<int>(dealDamage() * 1.5);
    return target.takeDamage(raw).damage;
}

std::string Enemy::getColorCode() const {
    switch (type_) {
        case EnemyType::Skeleton:   return ansi::BR_WHITE;
        case EnemyType::DarkKnight: return ansi::BR_RED;
        case EnemyType::Wraith:     return ansi::BR_MAGENTA;
        case EnemyType::FireDrake:  return ansi::BR_YELLOW;
        case EnemyType::LichKing:   return ansi::BR_GREEN;
    }
    return ansi::WHITE;
}

// ── Factory ────────────────────────────────────────────────────────────────
std::unique_ptr<Enemy> Enemy::create(EnemyType type, int playerLevel) {
    switch (type) {
        case EnemyType::Skeleton:   return std::make_unique<Skeleton>(playerLevel);
        case EnemyType::DarkKnight: return std::make_unique<DarkKnight>(playerLevel);
        case EnemyType::Wraith:     return std::make_unique<Wraith>(playerLevel);
        case EnemyType::FireDrake:  return std::make_unique<FireDrake>(playerLevel);
        case EnemyType::LichKing:   return std::make_unique<LichKing>(playerLevel);
    }
    return std::make_unique<Skeleton>(playerLevel);   // fallback
}

// ============================================================================
//  Skeleton — relentless but brittle
// ============================================================================
Skeleton::Skeleton(int lv)
    : Enemy("Skeleton Warrior",
            /*hp=*/55 + lv * 12, /*sta=*/80, /*atk=*/12 + lv * 3,
            /*def=*/4 + lv, lv, EnemyType::Skeleton,
            /*xp=*/20 + lv * 5, /*gold=*/8 + lv * 2,
            "A hollow rattling echoes through the dark.  Bones held together\n"
            "  by ancient malice lurch forward, rusty blade raised high.")
{}

CombatAction Skeleton::chooseAction(int hpPct) {
    int r = Random::get(1, 100);
    if (hpPct < 25 && hasEnoughStamina(cost::DODGE))              return CombatAction::Dodge;
    if (r <= 20 && hasEnoughStamina(cost::SPECIAL_ACTION))        return CombatAction::SpecialAction;
    if (r <= 65 && hasEnoughStamina(cost::STRIKE))                return CombatAction::Strike;
    if (r <= 85 && hasEnoughStamina(cost::HEAVY_STRIKE))          return CombatAction::HeavyStrike;
    if (!hasEnoughStamina(cost::STRIKE))                          return CombatAction::Rest;
    return CombatAction::Strike;
}

int Skeleton::executeSpecialAction(Character& target) {
    int raw = static_cast<int>(dealDamage() * 1.4);
    return target.takeDamage(raw).damage;
}

std::string Skeleton::getAsciiArt() const {
    return
        "      .-.      \n"
        "     (o.o)     \n"
        "      |=|      \n"
        "     __|__     \n"
        "   //.=|=.\\\\  \n"
        "  // .=|=. \\\\ \n"
        "  \\\\  |  //   \n"
        "   \\\\(_)//    \n"
        "    (/|\\)     \n"
        "     ' '       \n";
}

// ============================================================================
//  Dark Knight — balanced warrior
// ============================================================================
DarkKnight::DarkKnight(int lv)
    : Enemy("Dark Knight",
            /*hp=*/80 + lv * 15, /*sta=*/90, /*atk=*/15 + lv * 4,
            /*def=*/8 + lv * 2, lv, EnemyType::DarkKnight,
            /*xp=*/35 + lv * 8, /*gold=*/15 + lv * 4,
            "Obsidian armour drinks the torchlight whole.  Behind the visor,\n"
            "  crimson eyes burn with a knight's broken oath.")
{}

CombatAction DarkKnight::chooseAction(int hpPct) {
    int r = Random::get(1, 100);
    if (hpPct < 30 && hasEnoughStamina(cost::DODGE))              return CombatAction::Dodge;
    if (r <= 20 && hasEnoughStamina(cost::SPECIAL_ACTION))        return CombatAction::SpecialAction;
    if (r <= 50 && hasEnoughStamina(cost::STRIKE))                return CombatAction::Strike;
    if (r <= 75 && hasEnoughStamina(cost::HEAVY_STRIKE))          return CombatAction::HeavyStrike;
    if (r <= 85 && hasEnoughStamina(cost::DODGE))                 return CombatAction::Dodge;
    if (!hasEnoughStamina(cost::STRIKE))                          return CombatAction::Rest;
    return CombatAction::Strike;
}

int DarkKnight::executeSpecialAction(Character& target) {
    int raw = static_cast<int>(dealDamage() * 1.3);
    target.consumeStamina(20);
    return target.takeDamage(raw).damage;
}

std::string DarkKnight::getAsciiArt() const {
    return
        "      _O_      \n"
        "     / | \\    \n"
        "    /__|__\\   \n"
        "   |  |||  |   \n"
        "   | /   \\ |  \n"
        "   |/     \\|  \n"
        "   /       \\  \n"
        "  /    |    \\ \n"
        "      / \\     \n"
        "     /   \\    \n";
}

// ============================================================================
//  Wraith — evasive phantom
// ============================================================================
Wraith::Wraith(int lv)
    : Enemy("Spectral Wraith",
            /*hp=*/45 + lv * 10, /*sta=*/120, /*atk=*/14 + lv * 3,
            /*def=*/3 + lv, lv, EnemyType::Wraith,
            /*xp=*/30 + lv * 7, /*gold=*/12 + lv * 3,
            "The air chills to frost.  A wailing silhouette materialises,\n"
            "  its translucent claws trailing wisps of stolen souls.")
{}

CombatAction Wraith::chooseAction(int hpPct) {
    int r = Random::get(1, 100);
    if (r <= 25 && hasEnoughStamina(cost::DODGE))                 return CombatAction::Dodge;
    if (hpPct < 60 && r <= 60 && hasEnoughStamina(cost::SPECIAL_ACTION)) return CombatAction::SpecialAction;
    if (r <= 65 && hasEnoughStamina(cost::STRIKE))                return CombatAction::Strike;
    if (r <= 80 && hasEnoughStamina(cost::HEAVY_STRIKE))          return CombatAction::HeavyStrike;
    if (hpPct < 40 && hasEnoughStamina(cost::DODGE))              return CombatAction::Dodge;
    if (!hasEnoughStamina(cost::STRIKE))                          return CombatAction::Rest;
    return CombatAction::Strike;
}

int Wraith::executeSpecialAction(Character& target) {
    int raw = static_cast<int>(dealDamage() * 1.4);
    DamageResult res = target.takeDamage(raw);
    heal(res.damage / 2);
    return res.damage;
}

std::string Wraith::getAsciiArt() const {
    return
        "     .-~~-.     \n"
        "    /      \\   \n"
        "   |  O  O  |  \n"
        "   |   __   |  \n"
        "    \\      /   \n"
        "   .-`~~~~`-.  \n"
        "  /  ~~~~~~  \\ \n"
        " |  ~~~~~~~~  |\n"
        "  \\  ~~~~~~  / \n"
        "   `--------`  \n";
}

// ============================================================================
//  Fire Drake — devastating brute
// ============================================================================
FireDrake::FireDrake(int lv)
    : Enemy("Fire Drake",
            /*hp=*/100 + lv * 18, /*sta=*/70, /*atk=*/20 + lv * 5,
            /*def=*/10 + lv * 2, lv, EnemyType::FireDrake,
            /*xp=*/50 + lv * 10, /*gold=*/25 + lv * 5,
            "Sulphurous heat rolls in waves.  Scales the colour of molten\n"
            "  iron flex as the drake rears, jaws brimming with flame.")
{}

CombatAction FireDrake::chooseAction(int hpPct) {
    (void)hpPct;
    int r = Random::get(1, 100);
    if (r <= 30 && hasEnoughStamina(cost::SPECIAL_ACTION))        return CombatAction::SpecialAction;
    if (r <= 60 && hasEnoughStamina(cost::HEAVY_STRIKE))          return CombatAction::HeavyStrike;
    if (r <= 75 && hasEnoughStamina(cost::STRIKE))                return CombatAction::Strike;
    if (r <= 85 && hasEnoughStamina(cost::DODGE))                 return CombatAction::Dodge;
    if (!hasEnoughStamina(cost::STRIKE))                          return CombatAction::Rest;
    return CombatAction::Strike;
}

int FireDrake::executeSpecialAction(Character& target) {
    int raw = static_cast<int>(dealDamage() * 2.2);
    return target.takeDamage(raw).damage;
}

std::string FireDrake::getAsciiArt() const {
    return
        "            ____ /    \\  \n"
        "     /\\_/\\ /  o  \\    \n"
        "    ( o.o )|      >   \n"
        "     > ^ <  \\____/    \n"
        "    /|   |\\   ||      \n"
        "   (_|   |_)  ||      \n"
        "     |   |   /  \\    \n"
        "     |___|  /    \\   \n"
        "     (   ) (______) \n"
        "      \\_/           \n";
}

// ============================================================================
//  Lich King — final boss, adaptive AI
// ============================================================================
LichKing::LichKing(int lv)
    : Enemy("The Lich King",
            /*hp=*/160 + lv * 25, /*sta=*/120, /*atk=*/22 + lv * 6,
            /*def=*/12 + lv * 3, lv, EnemyType::LichKing,
            /*xp=*/100 + lv * 20, /*gold=*/50 + lv * 10,
            "Reality fractures.  A towering figure wreathed in emerald\n"
            "  hellfire descends, crown of shattered souls ablaze.\n"
            "  The Lich King has come to claim your life.")
{}

CombatAction LichKing::chooseAction(int hpPct) {
    int r = Random::get(1, 100);
    // Adaptive — becomes more aggressive when hurt
    if (hpPct < 30) {
        if (r <= 40 && hasEnoughStamina(cost::SPECIAL_ACTION)) return CombatAction::SpecialAction;
        if (r <= 70 && hasEnoughStamina(cost::HEAVY_STRIKE))   return CombatAction::HeavyStrike;
        if (r <= 85 && hasEnoughStamina(cost::STRIKE))         return CombatAction::Strike;
        if (hasEnoughStamina(cost::DODGE))                     return CombatAction::Dodge;
        return CombatAction::Rest;
    }
    if (r <= 25 && hasEnoughStamina(cost::SPECIAL_ACTION)) return CombatAction::SpecialAction;
    if (r <= 50 && hasEnoughStamina(cost::HEAVY_STRIKE))   return CombatAction::HeavyStrike;
    if (r <= 75 && hasEnoughStamina(cost::STRIKE))         return CombatAction::Strike;
    if (r <= 85 && hasEnoughStamina(cost::DODGE))          return CombatAction::Dodge;
    if (!hasEnoughStamina(cost::STRIKE))                   return CombatAction::Rest;
    return CombatAction::Strike;
}

int LichKing::executeSpecialAction(Character& target) {
    int raw = static_cast<int>(dealDamage() * 2.0);
    DamageResult res = target.takeDamage(raw);
    target.consumeStamina(25);
    heal(25);
    return res.damage;
}

std::string LichKing::getAsciiArt() const {
    return
        "      __|__       \n"
        "    _/  |  \\_    \n"
        "   / \\     / \\  \n"
        "  |   (O_O)   |  \n"
        "  |   /| |\\   |  \n"
        "   \\_/ | | \\_/  \n"
        "      /| |\\      \n"
        "     / | | \\     \n"
        "    /  | |  \\    \n"
        "   /___|_|___\\   \n";
}

} // namespace emberforge
