# ============================================================================
#  Emberforge — CMake Build Configuration
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

# ── Core Library ────────────────────────────────────────────────────────────
set(EMBERFORGE_CORE_SOURCES
    src/Character.cpp
    src/Player.cpp
    src/Enemy.cpp
    src/CombatEngine.cpp
    src/UI.cpp
    src/GameEngine.cpp
)

add_library(emberforge_core STATIC ${EMBERFORGE_CORE_SOURCES})
target_include_directories(emberforge_core PUBLIC
    ${CMAKE_SOURCE_DIR}/include
)

# ── Compiler warnings ──────────────────────────────────────────────────────
if(MSVC)
    target_compile_options(emberforge_core PRIVATE /W4)
else()
    target_compile_options(emberforge_core PRIVATE -Wall -Wextra -Wpedantic)
endif()

# ── Main Executable ─────────────────────────────────────────────────────────
add_executable(${PROJECT_NAME} src/main.cpp)
target_link_libraries(${PROJECT_NAME} PRIVATE emberforge_core)

# ── Tests ───────────────────────────────────────────────────────────────────
option(BUILD_TESTING "Build the testing tree." ON)
if(BUILD_TESTING)
    enable_testing()
    add_executable(emberforge_tests tests/test_main.cpp)
    target_link_libraries(emberforge_tests PRIVATE emberforge_core)
    add_test(NAME UnitTests COMMAND emberforge_tests)
endif()

# ── Install target ──────────────────────────────────────────────────────────
install(TARGETS ${PROJECT_NAME} emberforge_core DESTINATION bin)
