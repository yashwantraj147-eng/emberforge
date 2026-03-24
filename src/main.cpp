// ============================================================================
//  Emberforge — Entry Point
// ============================================================================
#include "GameEngine.h"
#include "UI.h"

int main() {
    emberforge::UI::enableAnsiColors();
    emberforge::GameEngine engine;
    engine.run();
    return 0;
}
