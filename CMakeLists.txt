# ============================================================================
#  Emberforge — CMake Build Configuration
#  A dark-fantasy terminal combat simulator & text RPG engine
# ============================================================================
cmake_minimum_required(VERSION 3.16)
project(emberforge
    VERSION   1.0.0
    LANGUAGES CXX
    DESCRIPTION "A terminal-based high-fantasy combat simulator and text RPG engine"
)

# ── C++17 standard ──────────────────────────────────────────────────────────
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# ── Source files ────────────────────────────────────────────────────────────
set(EMBERFORGE_SOURCES
    src/main.cpp
    src/Character.cpp
    src/Player.cpp
    src/Enemy.cpp
    src/CombatEngine.cpp
    src/UI.cpp
    src/GameEngine.cpp
)

# ── Executable ──────────────────────────────────────────────────────────────
add_executable(${PROJECT_NAME} ${EMBERFORGE_SOURCES})

target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_SOURCE_DIR}/include
)

# ── Compiler warnings ──────────────────────────────────────────────────────
if(MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE /W4)
else()
    target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -Wpedantic)
endif()

# ── Install target ──────────────────────────────────────────────────────────
install(TARGETS ${PROJECT_NAME} DESTINATION bin)
