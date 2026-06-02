// ============================================================
// grid_tests.cpp
// Pruebas unitarias para Grid y generateTransitions.
//
// Compilar (desde la raiz del proyecto):
//   g++ -std=c++17 cube/Cube.cpp state/State.cpp \
//       transition/Transition.cpp grid/Grid.cpp \
//       tests/grid_tests.cpp -o grid_tests
//   ./grid_tests
// ============================================================

#include <iostream>
#include <string>
#include <vector>
#include <array>
#include "../grid/Grid.h"

// ------------------------------------------------------------
// Contadores globales de tests
// ------------------------------------------------------------
static int testsPassed = 0;
static int testsFailed = 0;

void reportTest(const std::string& name, bool passed) {
    if (passed) {
        std::cout << "[PASS] " << name << "\n";
        ++testsPassed;
    } else {
        std::cout << "[FAIL] " << name << "\n";
        ++testsFailed;
    }
}

// ------------------------------------------------------------
// Helpers de construccion
// ------------------------------------------------------------

// Layout de referencia para todos los tests:
//
//   .G.G.   (fila 0)
//   G.S.G   (fila 1)
//   .G.#.   (fila 2)
//   ....G   (fila 3)
//
// 4 filas x 5 columnas.
// Inicio  : (1, 2)
// Pared   : (2, 3)
// Oro [0] : (0, 1)
// Oro [1] : (0, 3)
// Oro [2] : (1, 0)
// Oro [3] : (1, 4)
// Oro [4] : (2, 1)
// Oro [5] : (3, 4)
std::vector<std::string> makeTestLayout() {
    return {
        ".G.G.",
        "G.S.G",
        ".G.#.",
        "....G"
    };
}

// Cubo estandar sin oro: top=0, bottom=1, north=2, south=3, east=4, west=5
Cube makeFreshCube() {
    return Cube(0, 1, 2, 3, 4, 5);
}

// State con todas las celdas de oro presentes, cubo fresco.
State makeStateAt(int row, int col) {
    std::array<bool,6> allGold = {true, true, true, true, true, true};
    return State(row, col, makeFreshCube(), allGold);
}

// ============================================================
// TEST 1 - isInside
//
// Verifica limites de la grilla en los cuatro bordes,
// en los extremos validos y fuera de rango.
// ============================================================
void test1_isInside() {
    std::cout << "--- TEST 1: isInside ---\n";

    Grid g(makeTestLayout());

    reportTest("test1a: (0,0) esta dentro",    g.isInside(0, 0));
    reportTest("test1b: (3,4) esta dentro",    g.isInside(3, 4));
    reportTest("test1c: (-1,0) fuera",        !g.isInside(-1, 0));
    reportTest("test1d: (4,0) fuera",         !g.isInside(4, 0));
    reportTest("test1e: (0,-1) fuera",        !g.isInside(0, -1));
    reportTest("test1f: (0,5) fuera",         !g.isInside(0, 5));
    reportTest("test1g: (2,3) dentro [pared]", g.isInside(2, 3));
    reportTest("test1h: (3,0) esta dentro",    g.isInside(3, 0));
}

// ============================================================
// TEST 2 - isBlocked
//
// Verifica que solo '#' sea bloqueada.
// Celda 'S', 'G', '.' NO son paredes.
// ============================================================
void test2_isBlocked() {
    std::cout << "\n--- TEST 2: isBlocked ---\n";

    Grid g(makeTestLayout());

    reportTest("test2a: (2,3) es pared",       g.isBlocked(2, 3));
    reportTest("test2b: (0,0) no es pared",   !g.isBlocked(0, 0));
    reportTest("test2c: (1,2) 'S' no es pared", !g.isBlocked(1, 2));
    reportTest("test2d: (0,1) 'G' no es pared", !g.isBlocked(0, 1));
    reportTest("test2e: (1,0) 'G' no es pared", !g.isBlocked(1, 0));
}

// ============================================================
// TEST 3 - isValidMove
//
// Combina isInside e isBlocked.
// Fuera de limites -> false.
// Pared -> false.
// Todo lo demas -> true.
// ============================================================
void test3_isValidMove() {
    std::cout << "\n--- TEST 3: isValidMove ---\n";

    Grid g(makeTestLayout());

    reportTest("test3a: (-1,0) fuera -> false",  !g.isValidMove(-1, 0));
    reportTest("test3b: (4,0) fuera -> false",   !g.isValidMove(4, 0));
    reportTest("test3c: (0,-1) fuera -> false",  !g.isValidMove(0, -1));
    reportTest("test3d: (2,3) pared -> false",   !g.isValidMove(2, 3));
    reportTest("test3e: (0,0) '.' -> true",       g.isValidMove(0, 0));
    reportTest("test3f: (1,2) 'S' -> true",       g.isValidMove(1, 2));
    reportTest("test3g: (0,1) 'G' -> true",       g.isValidMove(0, 1));
    reportTest("test3h: (0,4) '.' -> true",       g.isValidMove(0, 4));
}

// ============================================================
// TEST 4 - Posicion inicial (startRow / startCol)
//
// La celda 'S' en el layout de referencia esta en (1, 2).
// ============================================================
void test4_startPosition() {
    std::cout << "\n--- TEST 4: Posicion inicial ---\n";

    Grid g(makeTestLayout());

    reportTest("test4a: startRow() == 1", g.startRow() == 1);
    reportTest("test4b: startCol() == 2", g.startCol() == 2);
}

// ============================================================
// TEST 5 - Extraccion de celdas de oro (getGoldCells)
//
// Orden: arriba a abajo, izquierda a derecha.
// Debe coincidir con remainingGold[] en State.
// ============================================================
void test5_goldCells() {
    std::cout << "\n--- TEST 5: Celdas de oro ---\n";

    Grid g(makeTestLayout());
    GoldCells gc = g.getGoldCells();

    reportTest("test5a: gc[0] == (0,1)",
               gc[0].first == 0 && gc[0].second == 1);
    reportTest("test5b: gc[1] == (0,3)",
               gc[1].first == 0 && gc[1].second == 3);
    reportTest("test5c: gc[2] == (1,0)",
               gc[2].first == 1 && gc[2].second == 0);
    reportTest("test5d: gc[3] == (1,4)",
               gc[3].first == 1 && gc[3].second == 3+1);
    reportTest("test5e: gc[4] == (2,1)",
               gc[4].first == 2 && gc[4].second == 1);
    reportTest("test5f: gc[5] == (3,4)",
               gc[5].first == 3 && gc[5].second == 4);
}

// ============================================================
// TEST 6 - generateTransitions: esquina superior izquierda
//
// Desde (0,0):
//   Norte : (-1, 0) fuera -> invalido
//   Sur   : (1, 0) 'G'   -> valido
//   Este  : (0, 1) 'G'   -> valido
//   Oeste : (0,-1) fuera -> invalido
// Resultado: 2 transiciones en orden [Sur, Este].
// ============================================================
void test6_cornerTopLeft() {
    std::cout << "\n--- TEST 6: Esquina superior izquierda ---\n";

    Grid g(makeTestLayout());
    State s = makeStateAt(0, 0);

    std::vector<TransitionResult> transitions = generateTransitions(s, g);

    reportTest("test6a: 2 vecinos desde (0,0)",
               transitions.size() == 2);
    reportTest("test6b: vecino[0] es Sur -> (1,0)",
               transitions[0].nextState.row == 1 && transitions[0].nextState.col == 0);
    reportTest("test6c: vecino[1] es Este -> (0,1)",
               transitions[1].nextState.row == 0 && transitions[1].nextState.col == 1);
}

// ============================================================
// TEST 7 - generateTransitions: celda adyacente a pared
//
// Desde (2,2):
//   Norte : (1, 2) 'S'  -> valido
//   Sur   : (3, 2) '.'  -> valido
//   Este  : (2, 3) '#'  -> invalido (pared)
//   Oeste : (2, 1) 'G'  -> valido
// Resultado: 3 transiciones en orden [Norte, Sur, Oeste].
// ============================================================
void test7_adjacentToWall() {
    std::cout << "\n--- TEST 7: Celda adyacente a pared ---\n";

    Grid g(makeTestLayout());
    State s = makeStateAt(2, 2);

    std::vector<TransitionResult> transitions = generateTransitions(s, g);

    reportTest("test7a: 3 vecinos desde (2,2)",
               transitions.size() == 3);
    reportTest("test7b: vecino[0] es Norte -> (1,2)",
               transitions[0].nextState.row == 1 && transitions[0].nextState.col == 2);
    reportTest("test7c: vecino[1] es Sur -> (3,2)",
               transitions[1].nextState.row == 3 && transitions[1].nextState.col == 2);
    reportTest("test7d: vecino[2] es Oeste -> (2,1)",
               transitions[2].nextState.row == 2 && transitions[2].nextState.col == 1);
}

// ============================================================
// TEST 8 - generateTransitions: celda interior con 4 vecinos
//
// Desde (1,1):
//   Norte : (0,1) 'G' -> valido
//   Sur   : (2,1) 'G' -> valido
//   Este  : (1,2) 'S' -> valido
//   Oeste : (1,0) 'G' -> valido
// Resultado: 4 transiciones.
// ============================================================
void test8_interiorFourNeighbors() {
    std::cout << "\n--- TEST 8: Celda interior (4 vecinos) ---\n";

    Grid g(makeTestLayout());
    State s = makeStateAt(1, 1);

    std::vector<TransitionResult> transitions = generateTransitions(s, g);

    reportTest("test8a: 4 vecinos desde (1,1)",
               transitions.size() == 4);
}

// ============================================================
// TEST 9 - Orientacion correcta y oro en estados generados
//
// Desde (1,1) con cubo fresco Cube(0,1,2,3,4,5):
//
// Rotaciones y caras bottom resultantes:
//   rollNorth : bottom = cara 2 (era north)
//   rollSouth : bottom = cara 3 (era south)
//   rollEast  : bottom = cara 4 (era east)
//   rollWest  : bottom = cara 5 (era west)
//
// Interacciones de oro (todas CASO 1: bottom sin oro):
//   Norte a (0,1) = gc[0]: cara 2 recoge, remainingGold[0]=false
//   Sur   a (2,1) = gc[4]: cara 3 recoge, remainingGold[4]=false
//   Este  a (1,2) = 'S'  : sin celda de oro, sin interaccion
//   Oeste a (1,0) = gc[2]: cara 5 recoge, remainingGold[2]=false
// ============================================================
void test9_orientationAndGold() {
    std::cout << "\n--- TEST 9: Orientacion y oro en estados generados ---\n";

    Grid g(makeTestLayout());
    State s = makeStateAt(1, 1);

    std::vector<TransitionResult> transitions = generateTransitions(s, g);

    // Verificar que se generaron exactamente 4 transiciones
    if (transitions.size() != 4) {
        reportTest("test9: precondicion (4 vecinos)", false);
        return;
    }

    // Caras bottom despues de cada rotacion
    reportTest("test9a: Norte -> bottom = cara 2 (rollNorth)",
               transitions[0].nextState.cube.getBottom() == 2);
    reportTest("test9b: Sur   -> bottom = cara 3 (rollSouth)",
               transitions[1].nextState.cube.getBottom() == 3);
    reportTest("test9c: Este  -> bottom = cara 4 (rollEast)",
               transitions[2].nextState.cube.getBottom() == 4);
    reportTest("test9d: Oeste -> bottom = cara 5 (rollWest)",
               transitions[3].nextState.cube.getBottom() == 5);

    // Norte a (0,1): CASO 1, cara 2 recoge oro
    reportTest("test9e: Norte: faceHasGold(2) == true",
               transitions[0].nextState.cube.faceHasGold(2));
    reportTest("test9f: Norte: remainingGold[0] == false",
               !transitions[0].nextState.remainingGold[0]);

    // Sur a (2,1): CASO 1, cara 3 recoge oro
    reportTest("test9g: Sur: faceHasGold(3) == true",
               transitions[1].nextState.cube.faceHasGold(3));
    reportTest("test9h: Sur: remainingGold[4] == false",
               !transitions[1].nextState.remainingGold[4]);

    // Este a (1,2): 'S', no es celda de oro
    reportTest("test9i: Este: sin interaccion de oro (0 caras con oro)",
               transitions[2].nextState.cube.countGoldFaces() == 0);

    // Oeste a (1,0): CASO 1, cara 5 recoge oro
    reportTest("test9j: Oeste: faceHasGold(5) == true",
               transitions[3].nextState.cube.faceHasGold(5));
    reportTest("test9k: Oeste: remainingGold[2] == false",
               !transitions[3].nextState.remainingGold[2]);
}

// ============================================================
// TEST 10 - Value semantics: estado original no se modifica
//
// generateTransitions debe retornar copias independientes.
// El State de entrada NO debe ser modificado en ningun campo.
// Modificar un vecino tampoco debe afectar el original.
// ============================================================
void test10_valueSemantics() {
    std::cout << "\n--- TEST 10: Value semantics ---\n";

    Grid g(makeTestLayout());
    State original = makeStateAt(1, 1);

    std::vector<TransitionResult> transitions = generateTransitions(original, g);

    // El original no debe haberse modificado
    reportTest("test10a: original.row sin cambios",
               original.row == 1);
    reportTest("test10b: original.col sin cambios",
               original.col == 1);
    reportTest("test10c: original.cube.getBottom() sin cambios (cara 1)",
               original.cube.getBottom() == 1);

    bool allRemainingUnchanged = true;
    for (int i = 0; i < 6; ++i)
        if (!original.remainingGold[i]) { allRemainingUnchanged = false; break; }
    reportTest("test10d: original.remainingGold sin cambios (todo true)",
               allRemainingUnchanged);

    // Modificar un vecino no debe afectar el original
    transitions[0].nextState.row = 99;
    reportTest("test10e: modificar vecino no afecta original.row",
               original.row == 1);
}

// ============================================================
// main
// ============================================================
int main() {
    std::cout << "===========================================\n";
    std::cout << "  Grid Tests\n";
    std::cout << "===========================================\n";

    std::cout << "\n[Layout de referencia]\n";
    Grid(makeTestLayout()).printGrid();
    std::cout << "\n";

    test1_isInside();
    test2_isBlocked();
    test3_isValidMove();
    test4_startPosition();
    test5_goldCells();
    test6_cornerTopLeft();
    test7_adjacentToWall();
    test8_interiorFourNeighbors();
    test9_orientationAndGold();
    test10_valueSemantics();

    std::cout << "\n===========================================\n";
    std::cout << "  Resultado: " << testsPassed << " passed, "
              << testsFailed << " failed\n";
    std::cout << "===========================================\n";

    return (testsFailed == 0) ? 0 : 1;
}
