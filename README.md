<div align="center">

```
  _____ __  __ ____  _____ ____  _____ ___  ____   ____ _____
 | ____|  \/  | __ )| ____|  _ \|  ___/ _ \|  _ \ / ___| ____|
 |  _| | |\/| |  _ \|  _| | |_) | |_ | | | | |_) | |  _|  _|
 | |___| |  | | |_) | |___|  _ <|  _|| |_| |  _ <| |_| | |___
 |_____|_|  |_|____/|_____|_| \_\_|   \___/|_| \_\\____|_____|
```

### ⚔️ A Dark Fantasy Combat Engine ⚔️

*Terminal-based high-fantasy combat simulator & text RPG engine written in modern C++17.*

[![C++17](https://img.shields.io/badge/C%2B%2B-17-blue?style=for-the-badge&logo=cplusplus&logoColor=white)](https://en.cppreference.com/w/cpp/17)
[![CMake](https://img.shields.io/badge/CMake-3.16+-064F8C?style=for-the-badge&logo=cmake&logoColor=white)](https://cmake.org/)
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg?style=for-the-badge)](LICENSE)
[![Platform](https://img.shields.io/badge/Platform-Windows%20|%20Linux%20|%20macOS-blueviolet?style=for-the-badge)]()

---

*"In embers forged, in darkness tempered."*

</div>

---

## 🔥 Overview

**Emberforge** is a visually striking, terminal-rendered combat simulator that drops you into a dark-fantasy dungeon crawl. Navigate treacherous encounters against five distinct enemy archetypes, manage your stamina wisely, and fight your way to the Lich King in a strategic, turn-based combat system — all rendered with ANSI-coloured, box-drawn terminal art.

Built as a showcase of **modern C++17** design: clean OOP hierarchies, polymorphic enemy AI, factory patterns, and cross-platform ANSI rendering.

---

## ✨ Features

| Feature | Description |
|---|---|
| 🎨 **Rich Terminal UI** | ANSI-coloured health/stamina bars, Unicode box-drawing frames, fire-gradient title screen |
| ⚔️ **Strategic Combat** | 5 player actions: Strike, Heavy Strike, Dodge, Rest, Use Potion — each with stamina trade-offs |
| 🧠 **Polymorphic Enemy AI** | 5 enemy archetypes with unique behavioural decision trees |
| 📈 **Progression System** | XP, levelling, stat scaling, gold, and potion drops |
| 🏕️ **Campfire Rest** | Heal between encounters at the risk of missing loot |
| 👑 **Boss Fight** | Face the Lich King with adaptive AI in the final encounter |
| 🖥️ **Cross-Platform** | Compiles on Windows (MSVC), Linux (GCC/Clang), and macOS |

---

## 🎮 Gameplay

```
  HP  [████████████████████░░░░░]  82/100       ← Colour-coded health bar
  ST  [████████████░░░░░░░░░░░░░]  48/100       ← Stamina management is key

  ╔══════════════════════════════════════════╗
  ║            YOUR ACTION                   ║
  ║  [1] Strike           (15 ST)            ║
  ║  [2] Heavy Strike     (30 ST)   ← 1.8×  ║
  ║  [3] Dodge            (10 ST)   ← −75%  ║
  ║  [4] Rest            (+25 ST)            ║
  ║  [5] Use Potion       (×3)               ║
  ╚══════════════════════════════════════════╝
```

### Combat Strategy
- **Strike** — Reliable damage at low stamina cost.
- **Heavy Strike** — 1.8× damage for double the stamina. Use when you have an opening.
- **Dodge** — Braces for the next hit, reducing incoming damage by 75%. Crucial against bosses.
- **Rest** — Skip your attack to recover stamina. Leaves you vulnerable.
- **Use Potion** — Restores 40 HP. Potions are limited — spend them wisely.

### Enemy Archetypes

| Enemy | Personality | Danger |
|---|---|---|
| 💀 Skeleton Warrior | Aggressive, relentless strikes | ⭐⭐ |
| 🗡️ Dark Knight | Balanced — heavy attacks + dodging | ⭐⭐⭐ |
| 👻 Spectral Wraith | Evasive phantom, hard to pin down | ⭐⭐⭐ |
| 🐉 Fire Drake | Raw power, devastating flame attacks | ⭐⭐⭐⭐ |
| 👑 The Lich King | Adaptive boss AI — gets more aggressive when hurt | ⭐⭐⭐⭐⭐ |

---

## 🛠️ Installation

### Prerequisites
- A C++17 compatible compiler (GCC 7+, Clang 5+, MSVC 2017+)
- [CMake](https://cmake.org/) 3.16 or later

### Build

```bash
# Clone the repository
git clone https://github.com/yourusername/emberforge.git
cd emberforge

# Create build directory and compile
mkdir build && cd build
cmake ..
cmake --build .

# Run the game
./emberforge        # Linux/macOS
.\emberforge.exe    # Windows
```

### Quick Build (single command)

```bash
cmake -S . -B build && cmake --build build && ./build/emberforge
```

---

## 📁 Project Structure

```
emberforge/
├── CMakeLists.txt            # Build configuration (C++17)
├── README.md
├── .gitignore
├── include/                  # Header files
│   ├── Types.h               # ANSI codes, enums, constants
│   ├── Character.h            # Abstract base class
│   ├── Player.h               # Player character
│   ├── Enemy.h                # Enemy hierarchy (5 archetypes)
│   ├── CombatEngine.h         # Turn-based combat loop
│   ├── UI.h                   # Terminal UI renderer
│   └── GameEngine.h           # Main game controller
├── src/                      # Implementation files
│   ├── main.cpp               # Entry point
│   ├── Character.cpp           # Base combat mechanics
│   ├── Player.cpp              # Player actions & progression
│   ├── Enemy.cpp               # All enemies + factory
│   ├── CombatEngine.cpp        # Combat orchestration
│   ├── UI.cpp                  # ANSI rendering engine
│   └── GameEngine.cpp          # Adventure loop
└── build/                    # Generated build files (gitignored)
```

---

## 🏗️ Architecture

```
                    ┌─────────────────┐
                    │   GameEngine    │  ← Owns the game loop
                    └────────┬────────┘
                             │
                    ┌────────▼────────┐
                    │  CombatEngine   │  ← Orchestrates each fight
                    └───┬─────────┬───┘
                        │         │
               ┌────────▼──┐  ┌──▼────────┐
               │   Player  │  │   Enemy   │  ← Both inherit Character
               └────────┬──┘  └──┬────────┘
                        │         │
                   ┌────▼─────────▼────┐
                   │    Character      │  ← Abstract base (HP, stamina,
                   │    (abstract)     │     damage, dodge mechanics)
                   └───────────────────┘
                                          ┌─ Skeleton
                   Enemy subtypes:  ──────┼─ DarkKnight
                   (polymorphic AI)       ├─ Wraith
                                          ├─ FireDrake
                                          └─ LichKing (boss)
```

### Key Design Patterns
- **Inheritance & Polymorphism** — `Character` → `Player` / `Enemy` → concrete enemy types
- **Factory Method** — `Enemy::create(type, level)` — instantiates and scales enemies
- **Strategy Pattern** — Each enemy's `chooseAction()` implements a unique decision tree
- **Encapsulation** — All state mutations go through named methods, never raw field access

---

## 🤝 Contributing

Contributions are welcome! Here are some ideas:

- **New enemy types** — Create a new class inheriting `Enemy`
- **Player classes** — Add Mage, Rogue, Ranger with different stat profiles
- **Inventory system** — Weapons, armour, and consumables
- **Save/load** — Serialise game state to JSON or binary
- **Sound effects** — Terminal bell sequences or external audio

```bash
# Fork → branch → code → PR
git checkout -b feature/your-feature
# ... make changes ...
git commit -m "feat: add your feature"
git push origin feature/your-feature
```

---

## 📜 License

This project is licensed under the **MIT License** — see the [LICENSE](LICENSE) file for details.

---

<div align="center">

*Forged in embers. Tempered in darkness. Built with C++17.*

**⚔️ Enter the Ashen Citadel. ⚔️**

</div>
